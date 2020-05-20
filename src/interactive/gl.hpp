#ifndef INTERACTIVE_GL_H
#define INTERACTIVE_GL_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

typedef void (*KeyCallback)(int key, int action);
typedef void (*CursorPositionCallback)(double x, double y);
typedef void (*MouseButtonCallback)(int button, int action);

class OpenGLContext {
private:
    std::vector<KeyCallback> m_key_callbacks;
    std::vector<CursorPositionCallback> m_cursor_position_callbacks;
    std::vector<MouseButtonCallback> m_mouse_button_callbacks;

    void glfw_reshape(GLFWwindow *window, int width, int height);
    void glfw_key_callback(GLFWwindow *window, int key,
                           int scancode, int action,
                           int mods);
    void glfw_cursor_position_callback(GLFWwindow *window, double x, double y);
    void glfw_mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

    std::string const m_window_name;
    int m_resolution_x;
    int m_resolution_y;

public:
    OpenGLContext() {}
    OpenGLContext(std::string const &window_name, int res_x = 512, int res_y = 512) {
        m_window_name = window_name;
        m_resolution_x = res_x;
        m_resolution_y = res_y;
    }

    void init();
    void close();

    KeyCallback add_key_callback(KeyCallback callback) {
        m_key_callbacks.push_back(callback);
        return callback;
    }
    CursorPositionCallback add_cursor_position_callback(CursorPositionCallback callback) {
        m_cursor_position_callbacks.push_back(callback);
        return callback;
    }
    MouseButtonCallback add_mouse_button_callback(MouseButtonCallback callback) {
        m_mouse_button_callbacks.push_back(callback);
        return callback;
    }
};

#endif // INTERACTIVE_GL_H
