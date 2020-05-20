#include "gl.hpp"
#include <iostream>

float total_time;
float dt;

bool OpenGLContext::g_context_active = false;
OpenGLContext *OpenGLContext::g_opengl_context = NULL;

void OpenGLContext::open()
{
    if (g_context_active) {
        std::cerr <<  "ERROR: Only one OpenGL context can be open at a time!\n";
        exit(EXIT_FAILURE);
    }
    if (!glfwInit()) {
        std::cerr <<  "GLFW error: Something went wrong initializing GLFW.\n";
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_glfw_window = glfwCreateWindow(m_resolution_x, m_resolution_y, m_glfw_window_name.c_str(), NULL, NULL);
    if (m_glfw_window == NULL) {
        std::cerr << "GLFW error: Failed to create a window properly.\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(m_glfw_window);
    gladLoadGL(); //------
    glfwSwapInterval(1);

    // Input and event callbacks.
    glfwSetKeyCallback(m_glfw_window, glfw_key_callback);
    glfwSetMouseButtonCallback(m_glfw_window, glfw_mouse_button_callback);
    glfwSetCursorPosCallback(m_glfw_window, glfw_cursor_position_callback);
    glfwSetFramebufferSizeCallback(m_glfw_window, glfw_reshape);

    g_context_active = true;
    g_opengl_context = this;
}
void OpenGLContext::close()
{
    if (!g_context_active) {
        //- This check does not guarantee that the closed context is the currently open context.
        std::cerr <<  "ERROR: Tried to close OpenGLContext, none is open!\n";
        exit(EXIT_FAILURE);
    }
    glfwDestroyWindow(m_glfw_window);
    glfwTerminate();
    g_context_active = false;
}
void OpenGLContext::enter_loop()
{
    while (!glfwWindowShouldClose(m_glfw_window))
    {
        glfwPollEvents();

        m_last_time = m_time;
        m_time = glfwGetTime();
        m_dt = m_time - m_last_time;

        // Clearing: window clear to background color, viewport clear to the foreground color.
        glClearColor(0,0,0,1);
        glDisable(GL_SCISSOR_TEST);
        glClear(GL_COLOR_BUFFER_BIT);

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glEnable(GL_SCISSOR_TEST);
        glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);
        glClearColor(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        // loop_function(renderer);
        printf("Goin!\n");

        glFlush();
        glfwSwapBuffers(m_glfw_window);
        
//---
        // renderer->render();
        //renderer->write_to_ppm("test.ppm");
        //exit(EXIT_SUCCESS);
    }
}

void OpenGLContext::glfw_reshape(GLFWwindow *window, int width, int height)
{
}

void OpenGLContext::glfw_key_callback(GLFWwindow *window, int key,
                       int scancode, int action,
                       int mods)
{
    for (const KeyCallback &cb : g_opengl_context->m_key_callbacks) {
        cb(key, action);
    }
}
void OpenGLContext::glfw_cursor_position_callback(GLFWwindow *window, double x, double y)
{
    for (const CursorPositionCallback &cb : g_opengl_context->m_cursor_position_callbacks) {
        //--- Should convert to more meaningful coordinates.
        cb(x, y);
    }
}
void OpenGLContext::glfw_mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    for (const MouseButtonCallback &cb : g_opengl_context->m_mouse_button_callbacks) {
        cb(button, action);
    }
}
