#ifndef RENDERER_H
#define RENDERER_H
#include <vector>
#include "imaging.hpp"
#include "scene.hpp"

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

#endif // RENDERER_H
