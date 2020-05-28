#include "imaging/framebuffer.hpp"

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
                // Clamp it to the range [0, 1] first.
                float clamped = (*this)(i, j)[k];
	        if (clamped < 0.f) clamped = 0.f;
	        else if (clamped > 1.f) clamped = 1.f;
                rgb[k] = (int) (255 * clamped);
            }
            file << (int) rgb[0] << " " << (int) rgb[1] << " " << (int) rgb[2] << (i == m_width-1 ? "" : "    ");
        }
        file << "\n";
    }
    file.close();
}


void FrameBuffer::copy_from(const FrameBuffer &fb)
{
    if (m_width != fb.width() || m_height != fb.height()) {
        std::cerr << "ERROR: FrameBuffer::copy_from: Cannot copy from framebuffer with non-matching dimensions.\n";
        exit(EXIT_FAILURE);
    }
    memcpy(&data[0], &fb.data[0], sizeof(RGBA)*m_width*m_height);
}
