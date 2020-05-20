#include "core.hpp"
#include <math.h>

Scene *make_scene() {
    Scene *scene = new Scene();

    int n = 10;
    for (int i = 0; i < n; i++) {
        float theta = 2*M_PI*i*(1.0/n);
        float c = cos(theta);
        float s = sin(theta);
        scene->add_primitive(new Sphere(Transform::translate(1.2*c,0.2*s,1.2*s), 0.06));
        scene->add_primitive(new Sphere(Transform::translate(2*c,0,2*s), 0.4));
        // scene->add_primitive(new Sphere(Transform::translate(6*c,0,6*s), 2.5));
    }

    scene->add_light(new PointLight(Point(0,1,0), 1.f*RGB(1,1,1)));
    scene->add_light(new PointLight(Point(0,-1,0), 1.f*RGB(1,1,1)));

    return scene;
}
