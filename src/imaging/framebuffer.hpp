#ifndef IMAGING_FRAMEBUFFER_H
#define IMAGING_FRAMEBUFFER_H
#include "illumination.hpp"
#include <vector>
#include <string>
#include <iostream>

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
    int width() const {
        return m_width;
    }
    int height() const {
        return m_height;
    }
    void write_to_ppm(std::string const &filename);
};
#endif // IMAGING_FRAMEBUFFER_H
