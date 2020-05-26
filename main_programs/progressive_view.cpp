#include "core.hpp"
#include "gl.hpp"
#include <math.h>

//---Input and looper functions probably need state!
Renderer *g_renderer = NULL;
Player *g_player = NULL;

void key(int key, int action)
{
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_Q) {
            g_opengl_context->close();
            exit(EXIT_SUCCESS);
        }
        if (key == GLFW_KEY_R) {
            g_renderer->write_to_ppm("last_render.ppm");
        }
    }
}

bool pretty_much_equal(const Transform &t1, const Transform &t2)
{
    // Test if a transform has changed or not. The definition of "pretty much equal" could differ ...
    mat4x4 diff = t1.matrix - t2.matrix;
    float sum_squares = 0.f;
    for (int i = 0; i < 4; i++) sum_squares += glm::dot(diff[i], diff[i]);
    const float epsilon = 1e-1 * dt; // makes sense to scale this margin due to time passed each frame.
    float ep = sqrt(sum_squares);
    return ep < epsilon;
}


void force_aspect_ratio(int width, int height, double wanted_aspect_ratio)
{
    double aspect_ratio = ((double) height) / width;
    if (aspect_ratio > wanted_aspect_ratio) {
        glViewport(0, (height - wanted_aspect_ratio * width)/2.0, width, wanted_aspect_ratio * width);
    }
    else {
        glViewport((width - height / wanted_aspect_ratio)/2.0, 0, height / wanted_aspect_ratio,  height);
    }
}
void reshape(int width, int height)
{
    float aspect_ratio = 0.566; //------
    force_aspect_ratio(width, height, aspect_ratio);
}

// This timer is set as the should-yield function in the rendering loop.
bool frame_time_has_passed()
{
    #define FRAME_TIME ( 1.0 / 24.0 )
    static bool g_time_set = false;
    static float g_time = 0.f;
    static float g_timer = 0.f;
    if (!g_time_set) {
        g_time = glfwGetTime();
        g_time_set = true;
    }
    float new_time = glfwGetTime();
    g_timer += new_time - g_time;
    g_time = new_time;
    if (g_timer >= FRAME_TIME) {
        g_timer = 0.f;
        return true;
    }
    return false;
    #undef FRAME_TIME
}

class FrameBufferViewerLoop : public Looper {
private:
    bool rendering;
    Renderer *renderer;
    RenderingState rendering_state;
    float time_since_render;
    int num_renders;
    int num_still_renders;
    float average_render_time;
    
    Transform last_camera_transform;

    GLTexture texture;

    GLuint quad_vao;
    GLuint vertices_vbo;
    GLuint texcoords_vbo;

    GLint uniform_location_image;
    GLint uniform_location_transparency;

    GLShaderProgram shader_program;

    FrameBuffer last_downsampled_framebuffer;
    FrameBuffer downsampled_framebuffer;
public:
    FrameBufferViewerLoop(Renderer *_renderer)
    {
        rendering = true;
        rendering_state = RenderingState();
        time_since_render = 0;
        average_render_time = 0;
        num_renders = 0;
        renderer = _renderer;
        // Initialize the two framebuffers.
        //--- to save a copy each frame, these buffers could be swapped each frame.
        downsampled_framebuffer = FrameBuffer(renderer->downsampled_pixels_x(), renderer->downsampled_pixels_y());
        last_downsampled_framebuffer = FrameBuffer(renderer->downsampled_pixels_x(), renderer->downsampled_pixels_y());
        texture = GLTexture(*renderer->active_framebuffer());

        shader_program = GLShaderProgram("gl_shaders/passthrough_3U.vert", "gl_shaders/texture.frag");
        uniform_location_image = glGetUniformLocation(shader_program.ID(), "image");
        uniform_location_transparency = glGetUniformLocation(shader_program.ID(), "transparency");

        glGenVertexArrays(1, &quad_vao);
        glBindVertexArray(quad_vao);

        glEnableVertexAttribArray(0); // Positions.
        glEnableVertexAttribArray(1); // Texture coordinates.

        glGenBuffers(1, &vertices_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);
        const float vertices[3 * 4] = {
            -1,-1,0.5,
            1,-1,0.5,
            1,1,0.5,
            -1,1,0.5,
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

        glGenBuffers(1, &texcoords_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
        const float texcoords[2 * 4] = {
            0,1,
            1,1,
            1,0,
            0,0,
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

    }
    void loop();
};
void FrameBufferViewerLoop::loop() {
    // note:
    //    As long as the image is being rendered, the logic of this loop is a free-for-all
    //    attempt to make the image being synthesized kind of look good while still or moving.

    time_since_render += dt;
    // Camera control and movement through the "player".
    //--------------------------------------------------------------------------------
    last_camera_transform = renderer->camera->camera_to_world;
        
    g_player->update();
    renderer->camera->set_transform(g_player->get_transform());
    //--------------------------------------------------------------------------------
    bool camera_moved_this_frame = !pretty_much_equal(last_camera_transform, renderer->camera->camera_to_world);

    if (rendering) {
        if (pretty_much_equal(last_camera_transform, renderer->camera->camera_to_world)) {
    	    // Camera is now still, change logic.
    	    num_still_renders = 0;
    	    rendering = false;
        }
    }
    if (!rendering) {
        // When "not rendering", the camera is assumed to be still enough that the renderering just fills in smaller details.
        if (num_still_renders == 0) {
            // Use blocks if finishing the rendering for the first time.
            rendering_state = renderer->render(rendering_state, true);
        } else {
            // Since slight shifts to the camera could have been made, keep filling in small details.
            rendering_state = renderer->render(rendering_state, false);
        }
        if (rendering_state.done) num_still_renders ++;

        // Start movement-rendering again if the camera moves.
        if (!pretty_much_equal(last_camera_transform, renderer->camera->camera_to_world)) {
            rendering = true;
        }
    }
    if (rendering) {
        // Render with blocks, at a courser level while moving.
        rendering_state = renderer->render(rendering_state, true, 3);
        if (rendering_state.done) {
            // If the render cycle is complete, either keep rendering while moving,
            // or switch to the other rendering mode, where the camera is assumed still.
            rendering_state = RenderingState(0,0,0,0);

            if (pretty_much_equal(last_camera_transform, renderer->camera->camera_to_world)) {
                // Camera is now still, change logic.
                num_still_renders = 0;
                rendering = false;
            } else {
                // If the camera is not (almost) still, clear the render, which is saved in last_downsampled_framebuffer,
                // so movements can blend.
                last_downsampled_framebuffer.copy_from(downsampled_framebuffer);
                renderer->clear_active_framebuffer();
            }
            // Some statistics that might be wanted when editing this camera behaviour.
            // Accumulate the average render time.
            //     average = sum(render_times) / num_renders
            // To accumulate the average without saving the previous render times, rearrange in terms of the above,
            //     (sum(render_times) + new_render_time) / (num_renders + 1) =
            //          new_render_time / (num_renders + 1) + (num_renders/(num_renders + 1))*average.
            average_render_time = time_since_render/(num_renders + 1) + (num_renders * (1.0 / num_renders))*average_render_time;
	    num_renders ++;
	    time_since_render = 0;
        }
    }
    renderer->downsample_to_framebuffer(&downsampled_framebuffer);

    shader_program.bind();
    glUniform1i(uniform_location_image, 0);
    glBindVertexArray(quad_vao);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    texture.destroy();
    texture = GLTexture(last_downsampled_framebuffer);
    texture.bind(0);
    glUniform1f(uniform_location_transparency, 1);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    texture.destroy();
    texture = GLTexture(downsampled_framebuffer);
    texture.bind(0);
    glUniform1f(uniform_location_transparency, 1);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void main_program(int argc, char *argv[], Renderer *renderer)
{

    // renderer->render();
    // FrameBuffer fb = renderer->downsampled_framebuffer();
    g_renderer = renderer; //make it globally available...

    OpenGLContext context("Progressive view", 512, 512);
    context.open();
    renderer->set_yield_test(frame_time_has_passed);

    g_player = new Player(0,0,0, 0,0, 3);
    g_player->look_with_mouse = false;
    context.add_input_listener(g_player);

    context.add_looper(new FrameBufferViewerLoop(renderer));
    context.add_reshape_callback(reshape);
    context.add_key_callback(key);
    context.enter_loop();
    context.close();
}
