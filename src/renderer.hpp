#ifndef RENDERER_H
#define RENDERER_H
#include "core.hpp"
#include "imaging.hpp"
#include "scene.hpp"

// The Renderer can act as a coroutine, allowing for example progressive viewing.
struct RenderingState {
    int pi, pj, i, j; // Indices in the render loop.
    
    bool done;
    RenderingState() {
        // Initial state.
        i = 0;
        j = 0;
        pi = 0;
        pj = 0;
        done = false;
    }
    RenderingState(int _i, int _j, bool _done = false) {
        // Rendering state for Renderer::render_direct.
        i = _i;
        j = _j;
        done = _done;
    }
    RenderingState(int _pi, int _pj, int _i, int _j, bool _done = false) {
        // Rendering state for Renderer::render.
        pi = _pi;
        pj = _pj;
        i = _i;
        j = _j;
        done = _done;
    }
};

// A Renderer encapsulates the Scene and Camera, and other things rendered and used for rendering.
class Renderer {
public:
    Scene *scene;
    Camera *camera;

    Renderer(Scene *_scene, Camera *_camera, int horizontal_pixels, int supersample_width = 1) {
        rendering_should_yield = NULL;
        scene = _scene;
        camera = _camera;
        m_supersample_width = supersample_width;

        m_horizontal_pixels = m_supersample_width * horizontal_pixels;
        m_downsampled_horizontal_pixels = horizontal_pixels;
        m_horizontal_pixels_inv = 1.0 / m_horizontal_pixels;

        m_downsampled_vertical_pixels = (int) (camera->aspect_ratio() * m_downsampled_horizontal_pixels);
        m_vertical_pixels = m_supersample_width * m_downsampled_vertical_pixels;
        m_vertical_pixels_inv = 1.0 / m_vertical_pixels;
        // Default to have space for one rendered frame.
        // Initialize this framebuffer to match the resolution.
        m_frames = std::vector<FrameBuffer>(1);
        m_frames[0] = FrameBuffer(m_supersample_width * m_horizontal_pixels, m_supersample_width * m_vertical_pixels);
        m_active_frame = 0;
    }
    // If the yield test is set, a callback is triggered in the rendering loop, so the caller can force rendering to stop at a certain point,
    // then resume with another call to render() passing the state previously returned by render().
    void set_yield_test(bool (*yield_test)()) {
        rendering_should_yield = yield_test;
    }

    RenderingState render(RenderingState state = RenderingState(), bool use_blocks = false, int exit_subblock = 0);
    RenderingState render(bool use_blocks = false, int exit_subblock = 0) {
        render(RenderingState(), use_blocks, exit_subblock);
    }
    void render_direct();

    FrameBuffer downsampled_framebuffer();
    // Alternatively, downsample to a framebuffer provided by the caller.
    void downsample_to_framebuffer(FrameBuffer *framebuffer);
    void write_to_ppm(std::string const &filename);

    inline const int pixels_x() const {
        return m_horizontal_pixels;
    }
    inline const int downsampled_pixels_x() const {
        return m_downsampled_horizontal_pixels;
    }
    inline const int pixels_y() const {
        return m_vertical_pixels;
    }
    inline const int downsampled_pixels_y() const {
        return m_downsampled_vertical_pixels;
    }
    inline const float pixels_x_inv() const {
        return m_horizontal_pixels_inv;
    }
    inline const float pixels_y_inv() const {
        return m_vertical_pixels_inv;
    }
    inline void set_pixel(int index_i, int index_j, RGB rgb) {
        m_frames[m_active_frame].set(index_i, index_j, RGBA(rgb,1));
    }
    inline void set_pixel_block(int index_i, int index_j, int to_index_i, int to_index_j, RGB rgb) {
        // Safe-guard the block range.
        if (to_index_i >= m_horizontal_pixels) to_index_i = m_horizontal_pixels - 1;
        if (to_index_j >= m_vertical_pixels) to_index_j = m_vertical_pixels - 1;
        m_frames[m_active_frame].set_block(index_i, index_j, to_index_i, to_index_j, RGBA(rgb,1));
    }
    FrameBuffer *active_framebuffer() {
        return &m_frames[m_active_frame];
    }
    void clear_active_framebuffer(RGBA color = RGBA(0,0,0,0)) {
        m_frames[m_active_frame].clear(color);
    }
    void print_properties() const;

private:
    // Renderer private data.
    int m_downsampled_horizontal_pixels;
    int m_downsampled_vertical_pixels;
    int m_horizontal_pixels;
    int m_vertical_pixels;
    int m_supersample_width; // A square of supersamples are used, with this width x width.

    // Raster-space--screen-space conversions are made a lot, so cache the inverses of the pixel extents.
    float m_horizontal_pixels_inv;
    float m_vertical_pixels_inv;

    int m_active_frame;
    std::vector<FrameBuffer> m_frames;
    bool (*rendering_should_yield)(); //= NULL?
};


#endif // RENDERER_H
