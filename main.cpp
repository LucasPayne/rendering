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
// Library includes.
#include <glm/glm.hpp>
typedef glm::vec3 vec3;
typedef glm::mat4x4 mat4x4;
// Project includes.
#include "geometry.h"

typedef vec3 RGB;

class HitInfo;
class Primitive {
private:    
public:    
    Transform object_to_world;
    Transform world_to_object;

    RGB TEST_COLOR; // For example, it may be useful when developing to turn off shading and just see if geometry works.

    Primitive() {}
    Primitive(const Transform &o2w) {
        object_to_world = o2w;
        world_to_object = o2w.inverse();
        TEST_COLOR = RGB(1,0,1);
    }
    virtual bool intersect(Ray &ray, HitInfo *info) = 0;
};
class Sphere : public Primitive {
private:
    float m_radius;
public:
    Sphere(const Transform &o2w, float radius) : Primitive(o2w) {
        m_radius = radius;
    }
    bool intersect(Ray &ray, HitInfo *info);
};


class HitInfo {
private:
public:
    Primitive *primitive;
    Point point;

    HitInfo() {}
    HitInfo(Primitive &_primitive) {
        primitive = &_primitive;
    }
};

bool Sphere::intersect(Ray &in_ray, HitInfo *info)
{
    Ray ray = world_to_object(in_ray);
    std::cout << "Before: " << in_ray << "\n";
    std::cout << "After: " << ray << "\n";

    // Solve the quadratic at^2 + bt + c = 0.
    float a = ray.d.x*ray.d.x + ray.d.y*ray.d.y + ray.d.z*ray.d.z;
    float b = 2*(ray.o.x*ray.d.x + ray.o.y*ray.d.y + ray.o.z*ray.d.z);
    float c = ray.o.x*ray.o.x + ray.o.y*ray.o.y + ray.o.z*ray.o.z - m_radius*m_radius;

    float discriminant = b*b - 4*a*c;
    if (discriminant < 0) return false;
    float inv2a = 1.0 / 2*a;
    float sqrtd = sqrt(discriminant);
    float root1, root2, temp;
    root1 = inv2a * (-b + sqrtd);
    root2 = inv2a * (-b - sqrtd);
    if (root1 > root2) {
        temp = root1;
        root1 = root2;
        root2 = temp;
    }
    if (root1 < ray.min_t || root1 > ray.max_t) {
        // Use root2 instead.
        if (root2 < ray.min_t || root2 > ray.max_t) return false;
        in_ray.max_t = root2;
    } else {
        in_ray.max_t = root1;
    }
    info->primitive = this;
    info->point = object_to_world(ray(in_ray.max_t));
    return true;
}

class Scene {
private:
    // Primitive root;
public:

    std::vector<Primitive *> primitives;

    Scene() {}
    Scene(int num_primitives) {
        primitives = std::vector<Primitive *>(num_primitives);
    }
    void add_primitive(Primitive *prim) {
        primitives.push_back(prim);
    }
};

class Camera {
private:
    int m_resolution_x;
    int m_resolution_y;

    float m_near_plane_distance;
    float m_far_plane_distance;
    float m_near_plane_half_width;
    float m_near_plane_half_height;
    float m_fov; // The field of view is measured horizontally.
    float m_aspect_ratio;

    Point m_position;
public:
    Transform transform;

    Camera() {}
    // Aspect ratio is given as height/width.
    Camera(Point position, Point lookat, float fov, float aspect_ratio)
    {
        // Initialize private data.
        // Field of view is passed in degrees.
        m_fov = (M_PI/180.0) * fov;
        m_position = position;
        m_aspect_ratio = aspect_ratio;
        m_near_plane_distance = 1; // Generated rays are from the camera origin to points on the near plane.
        m_far_plane_distance = 10; // Nothing past the far plane is culled, so this is fine.
        m_near_plane_half_width = cos(0.5 * m_fov);
        m_near_plane_half_height = aspect_ratio * m_near_plane_half_width;

        // Initialize public data.
        transform = Transform::lookat(position, lookat, Vector(0,1,0));
    }
    inline const float fov() const {
        return m_fov;
    }
    inline const float aspect_ratio() const {
        return m_aspect_ratio;
    }
    inline Point lens_point(float x, float y) const {
        // Return a position on the near quad ("lens") in world space, given x,y in
        // coordinates
        // (0,1) [           ] (1,1)
        //       [           ]
        //       [           ]
        // (0,0) [           ] (1,0).
        return transform(Point((2*x - 1)*m_near_plane_half_width,
                               (2*y - 1)*m_near_plane_half_height,
                               m_near_plane_distance));
    }
    inline Point position() const {
        return m_position;
    }
};

class FrameBuffer {
private:
    std::vector<RGB> data;
    int m_width;
    int m_height;
public:
    FrameBuffer() {}
    FrameBuffer(int width, int height) {
        m_width = width;
        m_height = height;
        data = std::vector<RGB>(width * height);
    }
    inline RGB operator()(int index_i, int index_j) {
        return data[index_i * m_height + index_j];
    }
    inline void set(int index_i, int index_j, RGB rgb) {
        data[index_i * m_height + index_j] = rgb;
    }
    void write_to_ppm(std::string const &filename)
    {
        /*  Example ppm file:
            P3
            2 9
            255
            255 0 0    255 255 0
            255 0 0    255 255 0
            255 255 0    255 0 0
            255 0 0    255 255 0
            255 0 0    255 255 0
            255 255 0    255 0 0
            255 0 0    255 255 0
            255 0 0    255 255 0
            255 255 0    255 0 0
        */
        std::ofstream file;
        file.open(filename);
        file << "P3\n";
        file << m_width << " " << m_height << "\n";
        file << "255\n";
        uint8_t rgb[3];
        for (int j = 0; j < m_height; j++) {
            for (int i = 0; i < m_width; i++) {
                // Downsample each RGB triple.
                for (int k = 0; k < 3; k++) {
                    rgb[k] = (int) (255 * (*this)(i, j)[k]);
                }
                file << (int) rgb[0] << " " << (int) rgb[1] << " " << (int) rgb[2] << (i == m_width-1 ? "" : "    ");
            }
            file << "\n";
        }
        file.close();
    }
};

// A Renderer encapsulates the Scene and Camera, and other things rendered and used for rendering.
class Renderer {
private:
    int m_horizontal_pixels;
    int m_vertical_pixels;
    // Raster-space--screen-space conversions are made a lot, so cache the inverses of the pixel extents.
    float m_horizontal_pixels_inv;
    float m_vertical_pixels_inv;

    int m_active_frame;
    std::vector<FrameBuffer> m_frames;
public:
    Scene *scene;
    Camera *camera;

    Renderer(Scene *_scene, Camera *_camera, int horizontal_pixels) {
        scene = _scene;
        camera = _camera;
        m_horizontal_pixels = horizontal_pixels;
        m_horizontal_pixels_inv = 1.0 / horizontal_pixels;
        m_vertical_pixels = (int) (camera->aspect_ratio() * horizontal_pixels);
        m_vertical_pixels_inv = 1.0 / m_vertical_pixels;
        // Default to have space for one rendered frame.
        // Initialize this framebuffer to match the resolution.
        m_frames = std::vector<FrameBuffer>(1);
        m_frames[0] = FrameBuffer(m_horizontal_pixels, m_vertical_pixels);
        m_active_frame = 0;
    }
    inline const int pixels_x() const {
        return m_horizontal_pixels;
    }
    inline const int pixels_y() const {
        return m_vertical_pixels;
    }
    inline const float pixels_x_inv() const {
        return m_horizontal_pixels_inv;
    }
    inline const float pixels_y_inv() const {
        return m_vertical_pixels_inv;
    }
    inline void set_pixel(int index_i, int index_j, RGB rgb) {
        m_frames[m_active_frame].set(index_i, index_j, rgb);
    }
    void write_to_ppm(std::string const &filename)
    {
        m_frames[m_active_frame].write_to_ppm(filename);
    }
};


// This function should be implemented by a linked program, procedurally describing the scene to be rendered.
// Scene *make_scene();
Scene *make_scene() {
    Scene *scene = new Scene(0);

    Sphere *sphere = new Sphere(Transform::translate(0,0,3), 0.5);
    sphere->TEST_COLOR = RGB(0.7,0,0);
    scene->add_primitive(sphere);
    sphere = new Sphere(Transform::translate(2,-1.3,5), 1.2);
    sphere->TEST_COLOR = RGB(0.7,0,0);
    scene->add_primitive(sphere);


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
    Renderer *renderer = new Renderer(scene, camera, 128);

    // Initialize the OpenGL context. This is used for graphical visualizations of the ray tracing process.
    init_gl();
    init_loop(renderer);
    close_gl();
}
