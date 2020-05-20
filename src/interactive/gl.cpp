#include "interactive/gl.hpp"

void OpenGLContext::init()
{
    if (!glfwInit()) {
        std::cerr <<  "GLFW error: Something went wrong initializing GLFW.\n";
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(m_resolution_x, m_resolution_y, "Progressive view", NULL, NULL);
    if (window == NULL) {
        std::cerr << "GLFW error: Failed to create a window properly.\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1);

    // Input and event callbacks.
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetFramebufferSizeCallback(window, reshape);
}

void init();
void close();


void OpenGLContext::glfw_reshape(GLFWwindow *window, int width, int height)
{
}

void OpenGLContext::glfw_key_callback(GLFWwindow *window, int key,
                       int scancode, int action,
                       int mods)
{
    for (const KeyCallback &cb : m_key_callbacks) {
        cb(key, action);
    }
}
void OpenGLContext::glfw_cursor_position_callback(GLFWwindow *window, double x, double y)
{
    for (const CursorPositionCallback &cb : m_cursor_position_callbacks) {
        //--- Should convert to more meaningful coordinates.
        cb(x, y);
    }
}
void OpenGLContext::glfw_mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    for (const MouseButtonCallback &cb : m_mouse_button_callbacks) {
        cb(button, action);
    }
}
