#include "core.hpp"
#include "gl.hpp"

class RenderLoop : public Looper {
    int m_n;
public:
    RenderLoop(int n) : m_n{n} {}
    void loop();
};
void RenderLoop::loop() {
    std::cout << "Yeahhhh boi the number is " << m_n << "\n";
}

void main_program(int argc, char *argv[], Renderer *renderer)
{

    OpenGLContext context = OpenGLContext("Progressive view", 512, 512);
    context.add_looper(new RenderLoop(44));
    context.open();

    renderer->render();
    FrameBuffer fb = renderer->downsampled_framebuffer();

    GLTexture tex(fb);

    context.enter_loop();
    context.close();
}
