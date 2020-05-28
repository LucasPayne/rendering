#ifndef GL_GL_TEXTURE_H
#define GL_GL_TEXTURE_H
#include "imaging/framebuffer.hpp"

class GLTexture {
private:
    GLuint m_gl_texture_id;
    int m_bound_to_texture_unit;
public:
    GLTexture() {}
    GLTexture(FrameBuffer &fb, bool linear = false);
    GLuint ID() const {
        return m_gl_texture_id;
    }
    void bind(int texture_unit) {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, m_gl_texture_id);
        m_bound_to_texture_unit = texture_unit;
    }
    void unbind() const {
        //-If something has been bound over this, this will unbind that instead.
        glActiveTexture(GL_TEXTURE0 + m_bound_to_texture_unit);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    void destroy() {
        glDeleteTextures(1, &m_gl_texture_id);
    }
};

#endif // GL_GL_TEXTURE_H
