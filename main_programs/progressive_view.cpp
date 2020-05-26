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
    if (ep < epsilon) std::cout << "Not equal\n";
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
    float average_render_time;
    bool moved_since_last_render;
    
    Transform last_camera_transform;

    GLTexture texture;

    GLuint quad_vao;
    GLuint vertices_vbo;
    GLuint texcoords_vbo;

    GLint uniform_location_image;
    GLint uniform_location_transparency;

    GLShaderProgram shader_program;

    FrameBuffer last_framebuffer;
public:
    FrameBufferViewerLoop(Renderer *_renderer)
    {
        rendering = true;
        rendering_state = RenderingState();
        time_since_render = 0;
        average_render_time = 0;
        num_renders = 0;
        renderer = _renderer;
        moved_since_last_render = false;
        last_framebuffer = FrameBuffer(renderer->active_framebuffer()->width(), renderer->active_framebuffer()->height());
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

    // Camera control and movement through the "player".
    //--------------------------------------------------------------------------------
    last_camera_transform = renderer->camera->camera_to_world;
    g_player->update();

#if 1
    renderer->camera->set_transform(g_player->get_transform());
#else
    static float theta_y = 0.f;
    float look_speed = 0.7;
    if (alt_arrow_key_down(Left)) {
        theta_y += look_speed * dt;
    }
    if (alt_arrow_key_down(Right)) {
        theta_y -= look_speed * dt;
    }
    // renderer->camera->set_transform(Transform::y_rotation(theta_y) * Transform::translate(Vector(0,0,-total_time)));
    renderer->camera->set_transform(Transform::y_rotation(theta_y));
#endif
    std::cout << renderer->camera->world_to_camera.matrix;
    //--------------------------------------------------------------------------------

    time_since_render += dt;


    // currently this value is not used.
    if (!pretty_much_equal(last_camera_transform, renderer->camera->camera_to_world)) moved_since_last_render = true;

    if (!rendering) {
        // well, it is still actually rendering ... but it is assumed the camera is almost still, so
        // finish the image off without using blocks, in case the camera just moves slightly.
        rendering_state = renderer->render(rendering_state, false);
        // Start rendering again if the camera moves.
        if (!pretty_much_equal(last_camera_transform, renderer->camera->world_to_camera)) {
            rendering = true;
        }
    }
    if (rendering) {
        rendering_state = renderer->render(rendering_state, true, 2);
        if (rendering_state.done) {
            rendering_state = RenderingState(0,0,0,0);
            last_framebuffer.copy_from(*renderer->active_framebuffer());

            if (pretty_much_equal(last_camera_transform, renderer->camera->world_to_camera)) {
                rendering = false;
            } else {
                // If the camera is not (almost) still, clear the render, which is saved in last_framebuffer,
                // so movements can blend.
                renderer->clear_active_framebuffer();
            }
	    num_renders ++;
	    average_render_time = average_render_time * ((num_renders - 1) * (1.0 / num_renders)) + (time_since_render * 1.0 / num_renders);
	    time_since_render = 0;
            moved_since_last_render = false;
        }
    }

    bool no_fade = false;
    // if (last_camera_transform.pretty_much_equal(renderer->camera->world_to_camera)) {
    //     no_fade = true;
    //     time_since_render = 0; // this isn't true, but it is what is visually wanted.
    // }

    shader_program.bind();
    glUniform1i(uniform_location_image, 0);
    glBindVertexArray(quad_vao);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    texture.destroy();
    texture = GLTexture(last_framebuffer);
    texture.bind(0);
    // float lowest_transparency = 1;
    // if (no_fade) {
    //     glUniform1f(uniform_location_transparency, 1);
    // } else {
    //     int tlevels = 10;
    //     glUniform1f(uniform_location_transparency, ((int) (tlevels*(1 - time_since_render / average_render_time))) / (1.0 * tlevels));
    //     // glUniform1f(uniform_location_transparency, average_render_time == 0 ? 0 : lowest_transparency + (1-lowest_transparency) * (1 - time_since_render / average_render_time));
    // }
    float fade_speed = 1;
    //glUniform1f(uniform_location_transparency, 1 - fade_speed * time_since_render / average_render_time);
    glUniform1f(uniform_location_transparency, 1);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    texture.destroy();
    texture = GLTexture(*renderer->active_framebuffer());
    texture.bind(0);
    // glUniform1f(uniform_location_transparency, 1);
    int tlevels = 3;
    float transparency = floor(tlevels * fade_speed * time_since_render / average_render_time) * (1.0 / tlevels);
    //float transparency = fade_speed * time_since_render / average_render_time;
    // glUniform1f(uniform_location_transparency, transparency);
    glUniform1f(uniform_location_transparency, 1);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


}

void main_program(int argc, char *argv[], Renderer *renderer)
{
    g_player = new Player(0,0,0, 0,0, 1);
    g_player->look_with_mouse = false;

    // renderer->render();
    // FrameBuffer fb = renderer->downsampled_framebuffer();
    g_renderer = renderer; //make it globally available...

    OpenGLContext context("Progressive view", 512, 512);
    context.open();
    renderer->set_yield_test(frame_time_has_passed);
    context.add_looper(new FrameBufferViewerLoop(renderer));
    context.add_reshape_callback(reshape);
    context.add_key_callback(key);
    context.enter_loop();
    context.close();
}
