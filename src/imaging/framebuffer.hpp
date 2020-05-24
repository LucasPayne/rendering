#ifndef IMAGING_FRAMEBUFFER_H
#define IMAGING_FRAMEBUFFER_H
#include "illumination.hpp"
#include <vector>
#include <string>
#include <iostream>

class FrameBuffer {
private:
    std::vector<RGBA> data;
    int m_width;
    int m_height;
public:
    FrameBuffer() {}
    FrameBuffer(int width, int height) {
        m_width = width;
        m_height = height;
        data = std::vector<RGBA>(width * height);
    }
    inline RGBA operator()(int index_i, int index_j) {
        return data[index_i * m_height + index_j];
    }
    inline void set(int index_i, int index_j, RGBA rgb) {
        data[index_i * m_height + index_j] = rgb;
    }
    int width() const {
        return m_width;
    }
    int height() const {
        return m_height;
    }
    void write_to_ppm(std::string const &filename);

    void clear(RGBA color = RGBA(0,0,0,0)) {
        //- could optimize for zero-setting.
        for (int i = 0; i < m_width; i++) {
            for (int j = 0; j < m_height; j++) {
                data[i*m_height + j] = color;
            }
        }
    }
    void copy_from(const FrameBuffer &fb);
};
#endif // IMAGING_FRAMEBUFFER_H
