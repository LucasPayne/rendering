#include "core.hpp"
#include "gl.hpp"

void main_program(int argc, char *argv[], Renderer *renderer)
{
    OpenGLContext context = OpenGLContext("Progressive view", 512, 512);
    context.open();
    context.enter_loop();
    context.close();
}


