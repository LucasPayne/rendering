#include "core.hpp"
#include "gl.hpp"
#include <math.h>
#include "framebuffer_viewer_loop.hpp"

void FrameBufferViewerLoop::loop() {
    // note:
    //    As long as the image is being rendered, the logic of this loop is a free-for-all
    //    attempt to make the image being synthesized kind of look good while still or moving.
    rendering_state = renderer->render_direct(rendering_state);
    if (!rendering_state.done) renderer->downsample_to_framebuffer(&downsampled_framebuffer);

    shader_program.bind();
    glUniform1i(uniform_location_image, 0);
    glBindVertexArray(quad_vao);

    texture.destroy();
    texture = GLTexture(downsampled_framebuffer);
    texture.bind(0);
    glUniform1f(uniform_location_transparency, 1);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void main_program(int argc, char *argv[], Renderer *renderer)
{
    OpenGLContext context = framebuffer_viewer_context(renderer, "View");
    context.enter_loop();
    context.close();
}
