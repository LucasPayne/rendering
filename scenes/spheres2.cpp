#include "core.hpp"

Scene *make_scene() {
    Scene *scene = new Scene();

    scene->add_primitive(new Sphere(Transform::translate(0,0,5), 0.5));
    scene->add_primitive(new Sphere(Transform::translate(0,0,14), 2));
    scene->add_light(new PointLight(Point(1,0,0), 10.f*RGB(1,1,1)));

    return scene;
}
