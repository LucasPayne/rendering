#include "core.hpp"
#include <math.h>

Scene *make_scene() {
    Scene *scene = new Scene();

    float r = 2;
    PrimitiveList *sphere_list = new PrimitiveList();
    for (int i = 0; i < 16; i++) {
        float theta = i*2*M_PI/16;

        GeometricPrimitive *sphere = new GeometricPrimitive(new Sphere(Transform::translate(cos(theta)*r,0,sin(theta)*r), 0.5));
        sphere_list->add(sphere);
    }
    scene->add_primitive(sphere_list);
    // scene->add_primitive(new TriangleMesh(Transform::translate(0,0.2,0),
    //                                       load_OFF_model("models/bunny.off", 0.5, Point(0,1,0), true)));

    scene->add_primitive(new GeometricPrimitive(new Plane(Point(0,-0.5,0), Vector(1,0,0), Vector(0,0,1), 10, 10)));
    scene->add_primitive(new GeometricPrimitive(new Plane(Point(-5,0,0), Vector(0,1,0), Vector(0,0,1), 10, 10)));
    scene->add_primitive(new GeometricPrimitive(new Plane(Point(5,0,0), Vector(0,1,0), Vector(0,0,-1), 10, 10)));
    scene->add_primitive(new GeometricPrimitive(new Plane(Point(0,0,5), Vector(1,0,0), Vector(0,1,0), 10, 10)));

    scene->add_primitive(new GeometricPrimitive(new Sphere(Transform::translate(0,20,0), 4)));


    scene->add_light(new PointLight(Point(0,4,0), 20.f*RGB(0.6,0.6,0.9)));
    scene->add_light(new PointLight(Point(-3,1,0), 20.f*RGB(1,0.6,0.5)));

    // scene->add_primitive(new TriangleMesh(Transform::translate(0,2,0) * Transform::y_rotation(0.7), load_OFF_model("models/icosahedron.off")));

    return scene;
}
