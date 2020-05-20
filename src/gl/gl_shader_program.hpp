#ifndef GL_GL_SHADER_PROGRAM_H
#define GL_GL_SHADER_PROGRAM_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class GLShader {
private:
    GLuint m_gl_shader_id;
    GLenum m_gl_shader_type;
public:
    GLShader() {}
    GLShader(GLShaderType shader_type, std::string const &shader_path);

    GLuint ID() const {
        return m_gl_shader_id;
    }
    GLenum type() const {
        return m_gl_shader_type;
    }
};

class GLShaderProgram {
private:
    GLShader m_vertex_shader;
    GLShader m_fragment_shader;

    GLuint m_gl_shader_program_id;
public:
    GLShaderProgram() {};
    GLShaderProgram(std::string const &vertex_shader_path, std::string const &fragment_shader_path);
    GLuint ID() const {
        return m_gl_shader_program_id;
    }
};

#endif // GL_GL_SHADER_PROGRAM_H
