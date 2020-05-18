#include "imaging/framebuffer.hpp"
#include <fstream>

void FrameBuffer::write_to_ppm(std::string const &filename)
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
