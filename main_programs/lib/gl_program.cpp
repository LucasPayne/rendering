/*--------------------------------------------------------------------------------
    Program schematic to be directly included by interactive OpenGL applications.
    This base opens an OpenGL context and window, starts a loop, keeps track of time,
    handles input, etc.

    This itself implements main_program.

    The main program source file including this must contain implementations of these
    functions:
        gl_program: Wrapped by the main_program given here.
--------------------------------------------------------------------------------*/
// C++ includes.
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
// C-style includes.
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
// Project includes
#include "core.hpp"
// OpenGL context includes.
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Global OpenGL context and GLFW-related variables.
GLFWwindow *window;
float total_time;
float dt;

//================================================================================
/*--------------------------------------------------------------------------------
    The program including this schematic has its own initialization run after gl_init.
--------------------------------------------------------------------------------*/
void gl_program(int argc, char *argv[]);
/*--------------------------------------------------------------------------------
    The loop provided by the user program.
--------------------------------------------------------------------------------*/
void gl_program_loop(Renderer *renderer);

//--callbacks. How would optional callbacks be added?
//================================================================================

void init_loop(Renderer *renderer);
void close_gl();
void init_gl();
static void reshape(GLFWwindow *window, int width, int height);
static void key_callback(GLFWwindow *window, int key,
                int scancode, int action,
                int mods);
static void cursor_position_callback(GLFWwindow *window, double x, double y);
static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);


static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
}
static void cursor_position_callback(GLFWwindow *window, double x, double y)
{
}
static void key_callback(GLFWwindow *window, int key,
                int scancode, int action,
                int mods)
{
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_Q) {
            close_gl();
            exit(EXIT_SUCCESS);
        }
    }
}
static void reshape(GLFWwindow *window, int width, int height)
{
}


void close_gl()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void enter_loop(void (*loop_function)(Renderer *), Renderer *renderer)
{
    static float last_time;
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        last_time = total_time;
        total_time = glfwGetTime();
        dt = total_time - last_time;

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

        loop_function(renderer);

        glFlush();
        glfwSwapBuffers(window);
        
//---
        // renderer->render();
        //renderer->write_to_ppm("test.ppm");
        //exit(EXIT_SUCCESS);
    }
}

void main_program(int argc, char *argv[], Renderer *renderer)
{
    init_gl();
    gl_program(argc, argv, renderer);
    enter_loop(gl_program_loop, renderer);
    close_gl();
}
