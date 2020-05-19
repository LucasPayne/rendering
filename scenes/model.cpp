#include "core.hpp"
#include <math.h>

Scene *make_scene() {
    Scene *scene = new Scene();

    scene->add_primitive(new TriangleMesh(Transform::translate(0,0,6) * Transform::y_rotation(M_PI/3), "models/icosahedron.off"));
    scene->add_primitive(new TriangleMesh(Transform::translate(-2,0,4), "models/icosahedron.off"));
    scene->add_primitive(new TriangleMesh(Transform::translate(2,0,4) * Transform::x_rotation(M_PI/4.333), "models/icosahedron.off"));

    scene->add_primitive(new Sphere(Transform::translate(0,1.5,4), 0.6));
    scene->add_primitive(new Sphere(Transform::translate(0,0,7), 2.3));
    scene->add_primitive(new Sphere(Transform::translate(0,-1.5,4), 0.6));

    scene->add_light(new PointLight(Point(1,0,-0.2), 5.f*RGB(1,1,1)));
    scene->add_light(new PointLight(Point(-3,0,-0.5), 5.f*RGB(0.8,0.8,1)));

    return scene;
}
