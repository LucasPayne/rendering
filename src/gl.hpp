#ifndef GL_CORE_H
#define GL_CORE_H
// Everything using the OpenGL/GLFW classes should include this.
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "core.hpp"

// The active OpenGLContext updates these global values.
// This is for convenience, so that having things with time-dependence doesn't require
// passing the context around everywhree.
extern float total_time;
extern float dt;

typedef void (*KeyCallback)(int key, int action);
typedef void (*CursorPositionCallback)(double x, double y);
typedef void (*MouseButtonCallback)(int button, int action);
typedef void (*ReshapeCallback)(int width, int height);

class InputListener {
private:
public:
    bool listening;
    virtual void key_callback(int key, int action) {}
    virtual void cursor_position_callback(double x, double y) {}
    virtual void cursor_move_callback(double x, double y) {}
    virtual void mouse_button_callback(int button, int action) {}
};

// A Looper can encpasulate its own data, so is a sort of parameterized function.
// The OpenGLContext holds a list of these.
struct Looper {
    Looper () {}
    virtual void loop() = 0;
};

class OpenGLContext {
private:
    // It would not make sense to give glfw a pointer to a member function of a class.
    // glfw will instead have these as callbacks, which pass the events to the callbacks of the active context.
    static void glfw_reshape(GLFWwindow *window, int width, int height);
    static void glfw_key_callback(GLFWwindow *window, int key,
                           int scancode, int action,
                           int mods);
    static void glfw_cursor_position_callback(GLFWwindow *window, double x, double y);
    static void glfw_mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

    // These callbacks are just function pointers, so cannot be associated to objects with data.
    std::vector<KeyCallback> m_key_callbacks;
    std::vector<CursorPositionCallback> m_cursor_position_callbacks;
    std::vector<MouseButtonCallback> m_mouse_button_callbacks;
    std::vector<ReshapeCallback> m_reshape_callbacks;
    // A class derived from InputListener implements callback functions, so can have data, etc.
    std::vector<InputListener *> m_input_listeners;

    std::vector<Looper *> m_loopers;

    int m_initial_resolution_x;
    int m_initial_resolution_y;
    int m_window_width;
    int m_window_height;

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
        m_initial_resolution_x = res_x;
        m_initial_resolution_y = res_y;
        m_window_width = res_x;
        m_window_height = res_x;
        m_time = 0;
        m_dt = 0;
        m_last_time = 0;
    }

    void open();
    void close();
    void enter_loop();

    InputListener *add_input_listener(InputListener *input_listener) {
        m_input_listeners.push_back(input_listener);
        return input_listener;
    }
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
    ReshapeCallback add_reshape_callback(ReshapeCallback callback) {
        m_reshape_callbacks.push_back(callback);
        return callback;
    }

    Looper *add_looper(Looper *looper) {
        m_loopers.push_back(looper);
        return looper;
    }
    inline int window_width() const { return m_window_width; }
    inline int window_height() const { return m_window_height; }
};

// There can only be one.
extern bool g_context_active;
extern OpenGLContext *g_opengl_context;

#include "gl/gl_texture.hpp"
#include "gl/gl_shader_program.hpp"
#include "gl/gl_input.hpp"

#endif // GL_CORE_H
