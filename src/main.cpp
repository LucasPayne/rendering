/*--------------------------------------------------------------------------------
    main.cpp stands between the two neccessary links -
        make_scene(),
            which makes the initial scene
        ---make_camera(),
            which sets up the initial camera.
            note: Should extend this interface to allow for a coroutine-like call for scene/camera updates, for animation.
        main_program(Renderer *), which does anything with the renderer (probably renders the scene to a file or views it).
--------------------------------------------------------------------------------*/
// C++ includes.
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
// C-style includes.
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
// Project includes.
#include "core.hpp"

//================================================================================
/*--------------------------------------------------------------------------------
    This function is implemented by a linked program, procedurally describing the
    initial scene to be rendered.
--------------------------------------------------------------------------------*/
extern Scene *make_scene();
/*--------------------------------------------------------------------------------
   This function is implemented by a linked program, using the prepared scene for
   whatever purpose (progressive viewing, animation, interactive hardware rendering
   with progressively raytraced overlay, rendering environment maps, etc.)
--------------------------------------------------------------------------------*/
extern void main_program(int argc, char *argv[], Renderer *renderer);
//================================================================================

int main(int argc, char *argv[])
{
    /*--------------------------------------------------------------------------------
        A Renderer encapsulates the stuff needed to render to a FrameBuffer (and do things
        like supersampling, etc.), and write that to images, provide downsampled framebuffers
        to routines for viewing, etc. The renderer acts as a server to the main program, and provides callbacks.
        
        For example, a program which just opens a window and shows a single rendered frame
    ----
        Main program
        Linked scene:
            Provides configuration for a created renderer, which serves the main program.
            The Renderer::next function either returns false (it is finished), or
            switches the renderer context so that further calls will be to do with the next frame (in an animation, simulation, etc.)
    --------------------------------------------------------------------------------*/
    // This program should be linked with an implementation of make_scene,
    // which is specific to the scene being rendered.
    Scene *scene = make_scene();
    // Camera *camera = make_camera();
    // Default to a camera at the origin facing down the Z-axis.
    Camera *camera = new Camera(Point(0,0,0), Point(0,0,1), 60, 0.566);
    Renderer *renderer = new Renderer(scene, camera, 512, 1);
    //--------------------------------------------------------------------------------

    // This main program is for a simple viewer of a single frame being rendered.
    main_program(argc, argv, renderer);
}
