#include "core.hpp"

Scene *make_scene() {
    Scene *scene = new Scene();

    scene->add_primitive(new TriangleMesh(Transform::translate(0,0,6), "models/icosahedron.off"));
    scene->add_light(new PointLight(Point(1,0,-0.2), 5.f*RGB(1,1,1)));

    return scene;
}
