#ifndef GL_GL_TEXTURE_H
#define GL_GL_TEXTURE_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imaging/framebuffer.hpp"
#include <stdint.h>

class GLTexture {
private:
    GLuint m_gl_texture_id;
public:
    GLTexture () {}
    GLTexture (FrameBuffer &fb)
    {
        int w = fb.width();
        int h = fb.height();
        uint8_t *ubyte_data = new uint8_t[3 * w * h];
        for (int i = 0; i < w; i++) {
            for (int j = 0; j < h; j++) {
                //-- should probably make downsampling to unsigned bytes a method of RGB.
                for (int k = 0; k < 3; k++) {
                    // Clamp it to the range [0, 1] first.
                    float clamped = fb(i, j)[k];
	            if (clamped < 0.f) clamped = 0.f;
	            else if (clamped > 1.f) clamped = 1.f;
                    ubyte_data[3*i*h + 3*j + k] = (int) (255 * clamped);
                }
            }
        }
        glGenTextures(1, &m_gl_texture_id);
        glBindTexture(GL_TEXTURE_2D, m_gl_texture_id);
        glTexStorage2D(GL_TEXTURE_2D, 3, GL_RGB8, w, h);
        glTexSubImage2D(GL_TEXTURE_2D,
                        0,    // First mipmap level
                        0, 0, // x and y offset
                        w, h,
                        GL_RGB, GL_UNSIGNED_BYTE,
                        ubyte_data);
        delete ubyte_data;
        // Set the texture defaults.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    GLuint ID() const {
        return m_gl_texture_id;
    };
};

#endif // GL_GL_TEXTURE_H
