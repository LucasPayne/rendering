/*--------------------------------------------------------------------------------
    main.cpp stands between the two neccessary links -
        make_scene(),
            which makes the initial scene
        ---make_camera(),
            which sets up the initial camera.
            note: Should extend this interface to allow for a coroutine-like call for scene/camera updates, for animation.
        main_program(Renderer *), which does anything with the renderer (probably renders the scene to a file or views it).
--------------------------------------------------------------------------------*/
#include "ray_tracer.hpp"


static void prerender_tests(Renderer *renderer)
{
    // A place to put random testing stuff, like printouts, before the render.
    // (Should be empty if nothing is being checked/tested).
}

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
        
        For example, a program which just opens a window and shows a single rendered frame.
    ----
        Main program
        Linked scene:
            Provides configuration for a created renderer, which serves the main program.
            The Renderer::next function either returns false (it is finished), or
            switches the renderer context so that further calls will be to do with the next frame (in an animation, simulation, etc.)
    --------------------------------------------------------------------------------*/
    // Process generic arguments.
    #define arg_error(STRING) {\
        std::cout << "Error in argument parsing: " << ( STRING ) << "\n";\
        exit(EXIT_FAILURE);\
    }
    int horizontal_pixels = 480;
    int supersampling_width = 1;  // These are the defaults.
    Point camera_position(0,0,0);
    float camera_azimuth = 0;
    float camera_altitude = 0;
    bool override_num_threads = false;
    unsigned int num_threads; // only used if overridden.
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-r") == 0) {
            if (i+1 >= argc
                || sscanf(argv[i+1], "%d", &horizontal_pixels) == EOF) arg_error("-r must be followed by a valid resolution.");
        }
        else if (strcmp(argv[i], "-s") == 0) {
            if (i+1 >= argc
                || sscanf(argv[i+1], "%d", &supersampling_width) == EOF) arg_error("-s must be followed by a valid supersampling width.");
        }
        else if (strcmp(argv[i], "-c") == 0) {
            if (i+1 >= argc || sscanf(argv[i+1], "%f,%f,%f,%f,%f", &camera_position.x,&camera_position.y,&camera_position.z,
                                                                   &camera_azimuth,&camera_altitude) == EOF) {
                arg_error("-c must be followed by 5 comma-separated floats, for the position, azimuth, then altitude.\n");
            }
        }
        else if (strcmp(argv[i], "-p") == 0) {
            if (i+1 >= argc
                || sscanf(argv[i+1], "%u", &num_threads) == EOF) arg_error("-p must be followed by a valid number of threads.");
            override_num_threads = true;
        }
    }
    std::cout << "System specs:\n";
    std::cout << "    num cores: " << num_system_cores() << "\n";

    float caz = cos(camera_azimuth);
    float saz = sin(camera_azimuth);
    float cal = cos(camera_altitude);
    float sal = sin(camera_altitude);
    Vector right = Vector(caz, 0, saz);
    Vector straight_forward = Vector(-saz, 0, caz);
    Vector straight_up = glm::cross(straight_forward, right);
    Vector forward = cal*straight_forward + sal*straight_up;
    Point camera_look_at = camera_position + forward;

    Camera *camera = new Camera(camera_position, camera_look_at, 60, 0.566);

    // This program should be linked with an implementation of make_scene,
    // which is specific to the scene being rendered.
    Scene *scene = make_scene();
    // Camera *camera = make_camera();
    // Default to a camera at the origin facing down the Z-axis.
    std::cout << "Creating renderer:\n";
    std::cout << "------------------------------------------------------\n";
    Renderer *renderer = new Renderer(scene, camera, horizontal_pixels, supersampling_width);
    renderer->print_properties();
    std::cout << "------------------------------------------------------\n";

    prerender_tests(renderer);

    // Pass the arguments after -- (optional) to the linked program. This is so
    // arguments don't clash between renderer settings and whatever the linked program does.
    int pass_argc = 1;
    #define MAX_ARGS 64
    char *pass_argv[MAX_ARGS];
    pass_argv[0] = argv[0];
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--") == 0) {
            for (int j = i+1; j < argc; j++) {
                if (pass_argc >= MAX_ARGS) arg_error("Exceeded maximum number of arguments.");
                pass_argv[pass_argc] = argv[j];
                pass_argc += 1;
            }
            break;
        }
    }
    if (override_num_threads) {
        init_multithreading(true, num_threads);
    } else {
        // By default, probably use the number of system cores.
        init_multithreading();
    }
    main_program(pass_argc, pass_argv, renderer);
}
