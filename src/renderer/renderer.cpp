#include <vector>
#include <string>
#include "imaging.hpp"
#include "scene.hpp"
#include "renderer.hpp"
#include <stdio.h>

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
            RGB color(0,0,0);
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

RenderingState Renderer::render(RenderingState state) //---get optional argument working.
{
    static int counter = 0;
    int start_i, start_j;
    if (state.j == pixels_y() - 1) {
        start_i = state.i + 1;
        start_j = 0;
    } else {
        start_i = state.i;
        start_j = state.j + 1;
    }

    float x, y;
    Point p;
    Ray ray;
    for (int i = start_i; i < pixels_x(); i++) {
        for (int j = start_j; j < pixels_y(); j++) {
            start_j = 0; // Make sure that the next loops start at 0.
            x = pixels_x_inv() * i;
            y = 1 - pixels_y_inv() * j;
            p = camera->lens_point(x, y);
            // std::cout << "Tracing ray (" << x << ", " << y << ")\n";
            // std::cout << "Lens point: " << p << "\n";
            ray = Ray(camera->position(), p - camera->position());
            set_pixel(i, j, trace_ray(ray));

            if (rendering_should_yield != NULL && rendering_should_yield()) {
                return RenderingState(i,j);
            }
        }
    }
    return RenderingState(0,0,true); // The rendering has finished.
}

RGB Renderer::trace_ray(Ray ray)
{
    LocalGeometry geom;
    if (intersect_primitive_vector(ray, scene->primitives, &geom)) {
        Vector n = glm::normalize(geom.n);
        // std::cout << geom.p << "\n";
        // std::cout << geom.n << "\n";
        // getchar();
        // return RGB(geom.p.x, geom.p.y,0);
        // return RGB(n.x, n.y, n.z);

        RGB color = RGB(0,0,0);
        for (Light *light : scene->lights) {
            Vector light_vector;
	    VisibilityTester visibility_tester;
            RGB light_radiance = light->radiance(geom.p, &light_vector, &visibility_tester);
            if (visibility_tester.unoccluded(scene)) {
                float cos_theta = glm::dot(light_vector, n);
                color += light_radiance * (cos_theta < 0 ? 0 : cos_theta);
            }
        }
        // Modulate with the diffuse color of the surface.
        color *= RGB(1,1,1);
        return color;
    }
    return RGB(0,1,0);
}
