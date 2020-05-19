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
#include "core.hpp"

// This function is implemented by a linked program, procedurally describing the scene to be rendered.
Scene *make_scene();

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
bool intersect_primitive_vector(Ray &ray, std::vector<Primitive *> &primitives, LocalGeometry &geom)
{
    LocalGeometry geom_per;
    bool hit_any = false;
    for (Primitive *primitive : primitives) {
        if (primitive->intersect(ray, &geom_per)) {
            geom = geom_per;
            hit_any = true;
        }
    }
    return hit_any;
}

bool ray_hits_surface(Scene &scene, Ray ray)
{
    // No intersect predicate method, just use the full one.
    LocalGeometry geom;
    return intersect_primitive_vector(ray, scene.primitives, geom);

}
RGB trace_ray(Scene &scene, Ray ray)
{
    LocalGeometry geom;
    if (intersect_primitive_vector(ray, scene.primitives, geom)) {
        RGB color = RGB(0,0,0);
        for (Light *light : scene.lights) {
            Ray light_ray = light->light_ray(geom);
            if (!ray_hits_surface(scene, light_ray)) {
                RGB light_color;
                // "Illumination", not anything scientific.
                float incoming_illumination = light->illumination(geom, light_color);
                std::cout << "incoming: " << incoming_illumination << ", color: " << light_color << "\n";
                color += incoming_illumination * light_color;
            }
        }
        // Modulate with the diffuse color of the surface.
        color *= RGB(1,0,0);
        return color;
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
