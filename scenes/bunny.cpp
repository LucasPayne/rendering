#include "core.hpp"
#include <math.h>

Scene *make_scene() {
    Scene *scene = new Scene();

    scene->add_primitive(new TriangleMesh(Transform::translate(0.5,-1,6) * Transform::y_rotation(0.5), load_OFF_model("models/bunny.off", 1.5, Point(0,1,0), true)));
    // scene->add_primitive(new TriangleMesh(Transform::translate(-3,0,6), load_OFF_model("models/dragon.off", 2, Point(0,1,0), true)));

    scene->add_light(new PointLight(Point(3,10,3), 20.f*RGB(1,1,1)));

    return scene;
}
