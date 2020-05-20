#ifndef GL_CORE_H
#define GL_CORE_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

// The active OpenGLContext updates these global values.
// This is for convenience, so that having things with time-dependence doesn't require
// passing the context around everywhree.
extern float total_time;
extern float dt;

typedef void (*KeyCallback)(int key, int action);
typedef void (*CursorPositionCallback)(double x, double y);
typedef void (*MouseButtonCallback)(int button, int action);


// A Looper can encpasulate its own data, so is a sort of parameterized function.
// The OpenGLContext holds a list of these.
struct Looper {
    Looper () {}
    virtual void loop() = 0;
};

class OpenGLContext {
private:
    // Make sure there is at most one active context.
    static bool g_context_active;
    static OpenGLContext *g_opengl_context;
    // It would not make sense to give glfw a pointer to a member function of a class.
    // glfw will instead have these as callbacks, which pass the events to the callbacks of the active context.
    static void glfw_reshape(GLFWwindow *window, int width, int height);
    static void glfw_key_callback(GLFWwindow *window, int key,
                           int scancode, int action,
                           int mods);
    static void glfw_cursor_position_callback(GLFWwindow *window, double x, double y);
    static void glfw_mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

    std::vector<KeyCallback> m_key_callbacks;
    std::vector<CursorPositionCallback> m_cursor_position_callbacks;
    std::vector<MouseButtonCallback> m_mouse_button_callbacks;

    std::vector<Looper *> m_loopers;

    int m_resolution_x;
    int m_resolution_y;

    // GLFW state.
    GLFWwindow *m_glfw_window;
    std::string m_glfw_window_name;
    float m_time;
    float m_dt;
    float m_last_time; // Used to compute m_dt.
public:
    OpenGLContext() {}
    OpenGLContext(std::string const &window_name, int res_x = 512, int res_y = 512) :
        m_glfw_window_name(window_name)
    {
        m_resolution_x = res_x;
        m_resolution_y = res_y;
        m_time = 0;
        m_dt = 0;
        m_last_time = 0;
    }

    void open();
    void close();
    void enter_loop();


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

    Looper *add_looper(Looper *looper) {
        m_loopers.push_back(looper);
        return looper;
    }

};

#include "gl/gl_texture.hpp"

#endif // GL_CORE_H
