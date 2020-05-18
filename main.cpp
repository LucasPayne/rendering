/*--------------------------------------------------------------------------------
--------------------------------------------------------------------------------*/
// OpenGL context includes.
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// C++ includes.
#include <iostream>
#include <vector>
// C-style includes.
#include <math.h>
#include <stdint.h>
// Library includes.
#include <glm/glm.hpp>
typedef glm::vec3 vec3;
typedef glm::mat4x4 mat4x4;
// Project includes.
#include "geometry.h"

class Scene {
private:
public:
    Scene() {}
};
class Camera {
private:
    Transform transform;
public:
    Camera() {}
    Camera(Point _position, Point _lookat) {
        transform = Transform::lookat(_position, _lookat, Vector(0,1,0));
    }
};

// A World encapsulates the Scene and Camera.
class World {
private:
    Scene &scene;
    Camera &camera;
public:
    World(Scene &_scene, Camera &_camera) :
        scene{_scene},
        camera{_camera}
    {}
};


// This function should be implemented by a linked program, procedurally describing the scene to be rendered.
// Scene *make_scene();
Scene *make_scene() {
    return NULL;
}

void init_gl();
void close_gl();
void init_loop(World &world);
void loop(World &world);

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

void init_loop(World &world)
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
        loop(world);

        glFlush();
        glfwSwapBuffers(window);
    }
}

void loop(World &world)
{
/*
    float inv_image_plane_width = 1.0 / scene.camera.image_plane_width;
    float inv_image_plane_height = 1.0 / scene.camera.image_plane_height;

    float cell_width = scene.camera.image_plane_width / scene.width;
    float cell_height = scene.camera.image_plane_height / scene.height;

    for (int i = 0; i < scene.width; i++) {
        for (int j = 0; j < scene.height; j++) {
            vec3 origin = vec3(0,0,0);
            vec3 direction = scene.camera.top_left + vec3((i + 0.5) * inv_image_plane_width, -(j + 0.5) * inv_image_plane_height, 0);
            Ray ray = Ray(origin, direction);
            vec3 color = sample_eye_ray(scene, ray, cell_width, cell_height);
        }
    }
*/
}
/*
vec3 sample_eye_ray(Scene &scene, Ray ray, float cell_width, float cell_height)
{
    vec3 color = vec3(0,0,0);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            Ray sample_ray = Ray(ray.origin, ray.direction - vec3((i - 2) * cell_width/4, (j - 2) * cell_height/4));
            vec3 sample_color = ray_trace(scene, sample_ray);
            color += sample_color;
        }
    }
    color /= 16;
    return color;
}
*/

vec3 ray_trace(Scene &scene, Ray ray)
{
    return vec3(0,1,0);
}


int main(int argc, char *argv[])
{
    // This program should be linked with an implementation of make_scene,
    // which is specific to the scene being rendered.
    Scene *scene = make_scene();
    // Camera *camera = make_camera();
    // Default to a camera at the origin facing down the Z-axis.
    Camera camera = Camera(Point(0,0,0), Point(0,0,1));
    World world = World(*scene, camera);

    // Initialize the OpenGL context. This is used for graphical visualizations of the ray tracing process.
    init_gl();
    init_loop(world);
    close_gl();
}
