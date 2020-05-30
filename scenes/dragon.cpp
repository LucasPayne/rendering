#include "ray_tracer.hpp"

Scene *make_scene() {
    Scene *scene = new Scene();
    vector<Primitive *> primitives(0);

    scene->add_light(new PointLight(Point(0,30,0), 2000.f*RGB(0.5,0.5,0.98)));
    // scene->add_light(new PointLight(Point(60,30,0), 3500.f*RGB(0.98,0.5,0.3)));
    // scene->add_light(new PointLight(Point(-40,30,40), 3000.f*RGB(0.28,0.95,0.3)));

    primitives.push_back(new GeometricPrimitive(new Plane(Point(0,-1,0), Vector(1,0,0), Vector(0,0,1), 1000, 1000)));


    float r = 20;
    int n = 3;
    Model *dragon = load_OFF_model("models/dragon.off", 10, Point(0,0,0), true);
    for (int i = 0; i < n; i++) {
        float theta = 0.5 * i*2*M_PI/n;
        primitives.push_back(new GeometricPrimitive(
                             new TriangleMesh(Transform::translate(r*sin(theta),-6,r*cos(theta)),
                             dragon)));
    }
    

    BVH *bvh = new BVH(primitives);
    scene->add_primitive(bvh);

    return scene;
}
