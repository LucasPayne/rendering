#include "ray_tracer.hpp"

Scene *make_scene() {
    Scene *scene = new Scene();
    vector<Primitive *> primitives(0);

    scene->add_light(new PointLight(Point(0,10,0), 50.f*RGB(1,0.956,0.43)));
    // scene->add_light(new PointLight(Point(0,2,5), 20.f*RGB(0.5,0.5,1)));

    Model *bunny = load_OFF_model("models/bunny.off", 1, Point(0,1,0), true, true);
    primitives.push_back(new GeometricPrimitive(
                         new TriangleMesh(Transform::translate(0,-0.7,5) * Transform::y_rotation(1.5*M_PI/2),
                         bunny)));
    Model *dragon = load_OFF_model("models/dragon.off", 1.3, Point(0,0,0), true, true);
    primitives.push_back(new GeometricPrimitive(
                         new TriangleMesh(Transform::translate(-3,-0.7,5) * Transform::y_rotation(1),
                         dragon)));

    float size = 15;
    float height = -1;
    primitives.push_back(new GeometricPrimitive(new Plane(Point(0,height,2), Vector(1,0,0), Vector(0,0,1), size, size)));
    primitives.push_back(new GeometricPrimitive(new Plane(Point(-5,height,2), Vector(0,1,0), Vector(0,0,1), size, size)));
    primitives.push_back(new GeometricPrimitive(new Plane(Point(5,height,2), Vector(0,1,0), Vector(0,0,-1), size, size)));
    primitives.push_back(new GeometricPrimitive(new Plane(Point(0,height,7), Vector(1,0,0), Vector(0,1,0), size, size)));

    

    BVH *bvh = new BVH(primitives);
    scene->add_primitive(bvh);

    return scene;
}
