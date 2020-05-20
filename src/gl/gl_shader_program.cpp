#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "gl/gl_shader_program.hpp"

// Load a shader.
GLShader::GLShader(GLenum shader_type, std::string const &shader_path)
{
    // https://stackoverflow.com/questions/2912520/read-file-contents-into-a-string-in-c
    FILE *file = fopen(shader_path.c_str(), "r");
    if (file == NULL) {
        std::cerr << "ERROR: Failed to open shader \"" << shader_path << "\".\n";
        exit(EXIT_FAILURE);
    }
    std::cout << "Reading in file ...\n";
    fseek(file, 0, SEEK_END);
    size_t source_size = ftell(file) + 1;
    char *source = new char[source_size];
    rewind(file);
    fread(source, sizeof(char), source_size-1, file);
    source[source_size-1] = '\0';
    std::cout << "Done\n";

    puts(source);
    std::cout << m_gl_shader_id << "\n";
    
    m_gl_shader_type = shader_type;
    m_gl_shader_id = glCreateShader(shader_type);
    if (m_gl_shader_id == 0) {
        std::cerr << "ERROR: Failed to create a shader ID.\n";
        exit(EXIT_FAILURE);
    }
    std::cout << "Created shader\n";
    glShaderSource(m_gl_shader_id, 1, (const GLchar **) &source, NULL);
    delete[] source;
    fclose(file);

    glCompileShader(m_gl_shader_id);
    GLint success;
    glGetShaderiv(m_gl_shader_id, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        std::cerr << "ERROR: Failed to compile shader \"" << shader_path << "\".\n";
        GLint info_length;
        glGetShaderiv(m_gl_shader_id, GL_INFO_LOG_LENGTH, &info_length);
        char *info = new char[info_length + 1];
        glGetShaderInfoLog(m_gl_shader_id, info_length, NULL, info);
        info[info_length] = '\0';
        std::cerr << "--------------------------------------------------------------------------------\n";
        std::cerr << " SHADER ERROR LOG: " << shader_path << "\n";
        std::cerr << "--------------------------------------------------------------------------------\n";
        fputs(info, stderr);
        std::cerr << "--------------------------------------------------------------------------------\n";
        exit(EXIT_FAILURE);
    }
}

GLShaderProgram::GLShaderProgram(std::string const &vertex_shader_path, std::string const &fragment_shader_path)
{
    m_vertex_shader = GLShader(GL_VERTEX_SHADER, vertex_shader_path);
    m_fragment_shader = GLShader(GL_FRAGMENT_SHADER, fragment_shader_path);
    m_gl_shader_program_id = glCreateProgram();
    if (m_gl_shader_program_id == 0) {
        std::cerr << "ERROR: Failed to create shader program.\n";
        exit(EXIT_FAILURE);
    }
    glAttachShader(m_gl_shader_program_id, m_vertex_shader.ID());
    glAttachShader(m_gl_shader_program_id, m_fragment_shader.ID());
    glLinkProgram(m_gl_shader_program_id);
    GLint success;
    glGetProgramiv(m_gl_shader_program_id, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        std::cerr << "ERROR: Failed to link shader program.\n";
        //----print log
        exit(EXIT_FAILURE);
    }
}
