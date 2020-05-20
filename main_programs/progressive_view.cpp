#include "core.hpp"
#include "gl.hpp"

class FrameBufferViewerLoop : public Looper {
    GLTexture texture;

    GLuint quad_vao;
    GLuint vertices_vbo;
    GLuint texcoords_vbo;

    GLShaderProgram shader_program;

public:
    FrameBufferViewerLoop(FrameBuffer &fb)
    {
        texture = GLTexture(fb);

        shader_program = GLShaderProgram("gl_shaders/passthrough_3U.vert", "gl_shaders/texture.frag");

        glGenVertexArrays(1, &quad_vao);
        glBindVertexArray(quad_vao);

        glEnableVertexAttribArray(0); // Positions.
        glEnableVertexAttribArray(1); // Texture coordinates.
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);

        glGenBuffers(1, &vertices_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);
        const float vertices[3 * 4] = {
            -1,-1,0.5,
            1,-1,0.5,
            1,1,0.5,
            -1,1,0.5,
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &texcoords_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
        const float texcoords[2 * 4] = {
            0,0,
            1,0,
            1,1,
            0,1,
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    void loop();
};
void FrameBufferViewerLoop::loop() {
    std::cout << "Yeahhhh boi the texture id is " << texture.ID() << "\n";
}

void main_program(int argc, char *argv[], Renderer *renderer)
{
    renderer->render();
    FrameBuffer fb = renderer->downsampled_framebuffer();

    OpenGLContext context("Progressive view", 512, 512);
    context.open();
    context.add_looper(new FrameBufferViewerLoop(fb));

    context.enter_loop();
    context.close();
}
