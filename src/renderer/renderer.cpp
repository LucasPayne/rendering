#include "renderer.hpp"
#include "multithreading.hpp"

// Trace a ray through the primitive (probably the scene itself,
// but since the scene is a primitive, why not allow this to be any primitive).
RGB ray_trace(Ray &ray, Scene *scene, const Primitive *primitive)
{
    LocalGeometry geom;
    if (primitive->intersect(ray, &geom)) {
        Vector n = glm::normalize(geom.n); //--need to normalize? Should just leave it to the primitive.
        // Initialize the returned color to an ambient (hack) term.
        RGB ambient(0.1,0.1,0.1);
        RGB color = ambient;
        // Compute direct lighting.
        for (Light *light : scene->lights) {
            Vector light_vector;
            VisibilityTester visibility_tester;
            RGB light_radiance = light->radiance(geom.p, &light_vector, &visibility_tester);
            if (visibility_tester.unoccluded(primitive)) {
                float cos_theta = glm::dot(light_vector, n);
                color += light_radiance * (cos_theta < 0 ? 0 : cos_theta);
            }
        }
        // Modulate with the diffuse color of the surface.
        color *= RGB(1,1,1);
        return color;
    } else {
        const RGB background_color(0.97, 0.7, 0.96);
        return background_color;
    }
}

void Renderer::downsample_to_framebuffer(FrameBuffer *downsampled_fb)
{
    if (   downsampled_fb->width()  != m_downsampled_horizontal_pixels
        || downsampled_fb->height() != m_downsampled_vertical_pixels) {
        std::cerr << "ERROR: Attempted to downsample to incorrectly-sized framebuffer.";
        exit(EXIT_FAILURE);
    }
    FrameBuffer &fb = m_frames[m_active_frame];
    int ss = m_supersample_width;
    float inv_num_samples = 1.0 / (ss * ss);
    for (int i = 0; i < m_downsampled_horizontal_pixels; i++) {
        for (int j = 0; j < m_downsampled_vertical_pixels; j++) {
            RGBA color(0,0,0,0);
            for (int ssi = 0; ssi < ss; ssi++) {
                for (int ssj = 0; ssj < ss; ssj++) {
                    int ip = ss * i + ssi;
                    int jp = ss * j + ssj;
                    color += fb(ip, jp);
                }
            }
            color *= inv_num_samples;
            downsampled_fb->set(i, j, color);
        }
    }
}
FrameBuffer Renderer::downsampled_framebuffer()
{
    FrameBuffer downsampled_fb(m_downsampled_horizontal_pixels, m_downsampled_vertical_pixels);
    downsample_to_framebuffer(&downsampled_fb);
    return downsampled_fb;
}
void Renderer::write_to_ppm(std::string const &filename)
{
    // Write to an image file, but downsample first.
    FrameBuffer downsampled_fb = downsampled_framebuffer();
    downsampled_fb.write_to_ppm(filename);
}

// This render function is intended for just rendering an image in one pass.
RenderingState Renderer::render_direct(RenderingState state)
{
    // Initialize values used to generate rays.
    int width = pixels_x();
    int height = pixels_y();
    Vector camera_right_extent = camera->imaging_plane_width() * camera->camera_to_world(Vector(1,0,0));
    Vector camera_down_extent = camera->imaging_plane_height() * camera->camera_to_world(Vector(0,-1,0));
    Point origin = camera->position();
    Vector shifted_camera_top_left = camera->lens_point(0,1) - origin;
    
    // There is some complication in the loop so that this function can yield and be reentered.
    bool started_j = false;
    bool yielding = false;
    for (int i = state.i; i < width; i++) {
        for (int j = started_j ? 0 : state.j; j < height; j++) {
            started_j = true;
            if (yielding) return RenderingState(i,j);

            // Generate the ray.
	    float x = pixels_x_inv() * i;
	    float y = pixels_y_inv() * j;
            Ray ray(origin, shifted_camera_top_left + x*camera_right_extent + y*camera_down_extent);

            // Ray trace.
            RGB color = ray_trace(ray, scene, scene);

            // Update the pixel color.
            set_pixel(i, j, color);
        }
        // If the renderering should yield, set the yielding flag so the next stage of the loop can be entered, calculating the start state
        // for when this routine is reentered.
        if (rendering_should_yield != NULL && rendering_should_yield()) yielding = true;
    }
    return RenderingState(0,0,true);
}

// This render function is intended for interactive rendering, where a lower-quality image is shown while moving,
// and is improved on while the camera is not moving
//
// ---- disregarding animated objects, in which case selective rerendering might be neat, and the ability to pause all objects/simulation.
RenderingState Renderer::render(RenderingState state, bool use_blocks, int exit_subblock)
{
    // Initialize values used to generate rays.
    Point origin = camera->position();

    // Information for the block-grid pattern used for generating rays and updating pixels.
    int pattern[8] = { 0, 4, 2, 6, 1, 3, 5, 7 };
    int sizes[8] = { 8, 8, 4, 4, 2, 2, 1, 1 };
    int p_n = 8;
    int max_p_n;
    if (exit_subblock == 0) max_p_n = 8;
    else if (exit_subblock == 1) max_p_n = 6;
    else if (exit_subblock == 2) max_p_n = 4;
    else if (exit_subblock == 3) max_p_n = 2;
    else if (exit_subblock == 4) max_p_n = 0;
    else {
        std::cerr << "ERROR: The renderer can be made to stop at a certain resolution of sub-blocks of pixels, but an invalid one was given.\n";
        exit(EXIT_FAILURE);
    }

    // Coroutine stuff
    int start_i = state.i;
    int start_j = state.j;
    int start_pi = state.pi;
    int start_pj = state.pj;
    bool yielding = false;
    bool i_entered = false;
    bool j_entered = false;
    bool pj_entered = false;
    
    // A lot of complication in the loop is here because it needs to act as a coroutine,
    // and also scatter rendered pixels giving an image of increasing resolution without double-tracing
    // any pixel.
    for (int pi = start_pi; pi < max_p_n; pi++) {
        for (int pj = pj_entered ? 0 : start_pj; pj < max_p_n; pj++) {
            pj_entered = true;
            for (int i = i_entered ? pattern[pi] : start_i; i < pixels_x(); i += p_n) {
                i_entered = true;
                for (int j = j_entered ? pattern[pj] : start_j; j < pixels_y(); j += p_n) {
                    j_entered = true;
                    if (yielding) return RenderingState(pi,pj,i,j);

                    // Generate the ray.
                    float x = pixels_x_inv() * i;
                    float y = 1 - pixels_y_inv() * j;
                    Point p = camera->lens_point(x, y);
                    Ray ray(origin, p - origin);

                    // Ray trace.
                    RGB color = ray_trace(ray, scene, scene);

                    // Update the pixel or block of pixels.
                    if (use_blocks) set_pixel_block(i, j, i+sizes[pi]-1, j+sizes[pi]-1, color);
                    else set_pixel(i, j, color);

                    // Check if the render loop should yield. (- should this be done every iteration?)
                    if (rendering_should_yield != NULL && rendering_should_yield()) {
                        // Let the loop itself figure out what the next state is.
                        yielding = true;
                    }
                }
            }
        }
    }
    return RenderingState(0,0,0,0,true); // The rendering has finished.
}

void Renderer::print_properties() const
{
    std::cout << "renderer properties:" << "\n";
    std::cout << "    horizontal_pixels: " << m_horizontal_pixels << "\n";
    std::cout << "    vertical_pixels: " << m_vertical_pixels << "\n";
    std::cout << "    super_sample_width: " << m_supersample_width << "\n";
    std::cout << "    downsampled_horizontal_pixels: " << m_downsampled_horizontal_pixels << "\n";
    std::cout << "    downsampled_vertical_pixels: " << m_downsampled_vertical_pixels << "\n";
}
