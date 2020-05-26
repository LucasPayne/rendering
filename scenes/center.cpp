#include "core.hpp"
#include <math.h>

Scene *make_scene() {
    Scene *scene = new Scene();

    float r = 2;
    for (int i = 0; i < 16; i++) {
        float theta = i*2*M_PI/16;
        Sphere *sphere = new Sphere(Transform::translate(cos(theta)*r,0,sin(theta)*r), 0.5);
        scene->add_primitive(sphere);
    }
    scene->add_light(new PointLight(Point(0,0,0), 2.f*RGB(1,1,1)));

    return scene;
}
