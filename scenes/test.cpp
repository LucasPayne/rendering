#include "core.hpp"

Scene *make_scene() {
    Scene *scene = new Scene();

    scene->add_light(new PointLight(Point(-5,0,0), 14.f*RGB(1,1,1)));

    return scene;
}
