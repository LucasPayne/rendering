/*--------------------------------------------------------------------------------
--------------------------------------------------------------------------------*/
// OpenGL context includes.
#include <glad/glad.h>
#include <GLFW/glfw3.h>
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
// Library includes.
#include <glm/glm.hpp>
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4x4 mat4x4;
// Project includes.
#include "illumination.hpp"
#include "mathematics.hpp"
#include "primitives.hpp"
#include "imaging.hpp"
#include "scene.hpp"
#include "renderer.hpp"

#define frand() ((1.0 / (RAND_MAX + 1.0)) * rand())
#define frand_interval(A,B) (( A ) + (( B ) - ( A ))*frand())

// This function should be implemented by a linked program, procedurally describing the scene to be rendered.
// Scene *make_scene();
Scene *make_scene() {
    Scene *scene = new Scene(0);

    for (int i = 0; i < 30; i++) {
        Sphere *sphere = new Sphere(Transform::translate(frand_interval(-5,5),frand_interval(-5,5),frand_interval(2,10)), frand_interval(0.2,1.8));
        sphere->TEST_COLOR = RGB(frand(), frand(), frand());
        scene->add_primitive(sphere);
    }

    return scene;
}

void init_gl();
void close_gl();
void init_loop(Renderer *renderer);
void loop(Renderer &renderer);

// Global OpenGL context and GLFW-related variables.
GLFWwindow *window;
float total_time;
float dt;
int g_width = 512;
int g_height = 512;

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

void init_gl()
{
    if (!glfwInit()) {
        std::cerr <<  "GLFW error: Something went wrong initializing GLFW.\n";
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(g_width, g_height, "Ray tracer", NULL, NULL);
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

void close_gl()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void init_loop(Renderer *renderer)
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
        loop(*renderer);

        glFlush();
        glfwSwapBuffers(window);
    }
}


// Intersect with a vector of primitives, exhaustively checking every one.
bool intersect_primitive_vector(Ray &ray, std::vector<Primitive *> &primitives, HitInfo &info)
{
    int len = primitives.size();
    HitInfo info_per;
    bool hit_any = false;
    for (int i = 0; i < len; i++) {
        if (primitives[i]->intersect(ray, &info_per)) {
            info = info_per;
            hit_any = true;
        }
    }
    return hit_any;
}


RGB trace_ray(Scene &scene, Ray ray)
{
    HitInfo info;
    if (intersect_primitive_vector(ray, scene.primitives, info)) {
        return info.primitive->TEST_COLOR;
    }
    return RGB(0,1,0);
}
void loop(Renderer &renderer)
{
    Camera &camera = *renderer.camera;
    Scene &scene = *renderer.scene;

    float x, y;
    Point p;
    Ray ray;
    for (int i = 0; i < renderer.pixels_x(); i++) {
        for (int j = 0; j < renderer.pixels_y(); j++) {
            x = renderer.pixels_x_inv() * i;
            y = renderer.pixels_y_inv() * j;
            p = camera.lens_point(x, y);
            ray = Ray(camera.position(), p - camera.position());
            renderer.set_pixel(i, j, trace_ray(scene, ray));
        }
    }
    renderer.write_to_ppm("test.ppm");
    exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[])
{
    // This program should be linked with an implementation of make_scene,
    // which is specific to the scene being rendered.
    Scene *scene = make_scene();
    // Camera *camera = make_camera();
    // Default to a camera at the origin facing down the Z-axis.
    Camera *camera = new Camera(Point(0,0,0), Point(0,0,1), 60, 0.566);
    Renderer *renderer = new Renderer(scene, camera, 512);

    // Initialize the OpenGL context. This is used for graphical visualizations of the ray tracing process.
    init_gl();
    init_loop(renderer);
    close_gl();
}
