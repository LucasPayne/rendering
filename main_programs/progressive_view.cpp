#include "core.hpp"
#include "gl.hpp"

// This timer is set as the should-yield function in the rendering loop.
bool frame_time_has_passed()
{
    static bool g_time_set = false;
    static float g_time = 0.f;
    static float g_timer = 0.f;
    #define FRAME_TIME ( 1.0 / 60.0 )
    if (!g_time_set) g_time = glfwGetTime();
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

    GLTexture texture;

    GLuint quad_vao;
    GLuint vertices_vbo;
    GLuint texcoords_vbo;

    GLint uniform_location_image;

    GLShaderProgram shader_program;
public:
    FrameBufferViewerLoop(Renderer *_renderer)
    {
        rendering = true;
        rendering_state = RenderingState();
        renderer = _renderer;
        texture = GLTexture(*renderer->active_framebuffer());

        shader_program = GLShaderProgram("gl_shaders/passthrough_3U.vert", "gl_shaders/texture.frag");
        uniform_location_image = glGetUniformLocation(shader_program.ID(), "image");

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
    // std::cout << "Yeahhhh boi the texture id is " << texture.ID() << "\n";
    // std::cout << "Yeahhhh boi the shader program id is " << shader_program.ID() << "\n";
    // std::cout << "Yeahhhh boi the quad vao id is " << quad_vao << "\n";

    if (rendering) {
        rendering_state = renderer->render(rendering_state);
        // rendering = rendering_state.done;
    }
    texture.destroy();
    texture = GLTexture(*renderer->active_framebuffer());

    shader_program.bind();
    glUniform1i(uniform_location_image, 0);
    texture.bind(0);
    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void main_program(int argc, char *argv[], Renderer *renderer)
{
    // renderer->render();
    // FrameBuffer fb = renderer->downsampled_framebuffer();

    OpenGLContext context("Progressive view", 512, 512);
    context.open();
    renderer->set_yield_test(frame_time_has_passed);
    context.add_looper(new FrameBufferViewerLoop(renderer));
    context.enter_loop();
    context.close();
}
