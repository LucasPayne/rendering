#include "core.hpp"
#include <math.h>

Scene *make_scene() {
    Scene *scene = new Scene();

    scene->add_primitive(new TriangleMesh(Transform::translate(0,0,6), "models/bunny.off", 3));

    scene->add_light(new PointLight(Point(3,10,3), 20.f*RGB(1,1,1)));

    return scene;
}
