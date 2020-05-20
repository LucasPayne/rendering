#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include "gl/gl_shader_program.hpp"

// Load a shader.
GLShader::GLShader(GLenum shader_type, std::string const &shader_path)
{
    std::ifstream file;
    file.open(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: Shader \"" << shader_path << "\" does not exist.\n";
        exit(EXIT_FAILURE);
    }

    m_gl_shader_type = shader_type;
    m_gl_shader_id = glCreateShader(shader_type);
    if (m_gl_shader_id == 0) {
        std::cerr << "ERROR: Failed to create a shader ID.\n";
        exit(EXIT_FAILURE);
    }
    // https://stackoverflow.com/questions/2912520/read-file-contents-into-a-string-in-c
    std::string source( (std::istreambuf_iterator<char>(file)),
                        (std::istreambuf_iterator<char>()   ));
    glShaderSource(m_gl_shader_id, source.length(), source.c_str(), NULL);
    file.close();

    glCompileShader(m_gl_shader_id);
    GLint success;
    glGetShaderiv(m_gl_shader_id, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        std::cerr << "ERROR: Failed to compile shader \"" << shader_path << "\".";
        //----print log
        exit(EXIT_FAILURE);
    }
}

GLShaderProgram::GLShaderProgram(std::string const &vertex_shader_path, std::string const &fragment_shader_path);
{
    m_vertex_shader = GLShader(GL_VERTEX_SHADER, vertex_shader_path);
    m_fragment_shader = GLShader(GL_FRAGMENT_SHADER, fragment_shader_path);
    m_gl_shader_program_id = glCreateProgram();
    if (m_gl_shader_program_id == 0) {
        std::cerr << "ERROR: Failed to create shader program.\n";
        exit(EXIT_FAILURE);
    }
}
