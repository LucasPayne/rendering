#include "core.hpp"

Scene *make_scene() {
    Scene *scene = new Scene();

    // for (int i = 0; i < 3; i++) {
    //     Sphere *sphere = new Sphere(Transform::translate(frand_interval(-5,5),frand_interval(-5,5),frand_interval(2,10)), frand_interval(0.2,1.8));
    //     scene->add_primitive(sphere);
    // }
    scene->add_primitive(new Sphere(Transform::translate(-1.3,0.4,2), 0.5));
    scene->add_primitive(new Sphere(Transform::translate(0,0,2), 0.5));
    // scene->add_primitive(new Sphere(Transform::translate(1.3,0,2), 0.5));

    scene->add_light(new PointLight(Point(0,0,0), RGB(1,1,1)));
    // scene->add_light(new PointLight(Point(2,0,2), 300.f*RGB(1,1,1)));
    // scene->add_light(new PointLight(Point(-2,0,2), 300.f*RGB(1,1,1)));

    return scene;
}
