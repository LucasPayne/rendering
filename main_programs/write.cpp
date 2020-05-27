/*
Write the rendered image straight to a file.
*/
#include "core.hpp"
#include <string>
#include <string.h>

void main_program(int argc, char *argv[], Renderer *renderer)
{
    const char *default_filename = "last_render.ppm";
    const char *filename = default_filename;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-t") && i+1 < argc) filename = argv[i + 1];
    }
    renderer->render_direct();
    renderer->write_to_ppm(filename);
}
