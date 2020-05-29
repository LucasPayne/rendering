#include "ray_tracer.hpp"

Scene *make_scene() {
    Scene *scene = new Scene();

    vector<Primitive *> primitives(0);

    int n = 10;
    for (int I = -n; I <= n; I++) {
        vector<Primitive *> tower(0);
        for (int J = -n; J <= n; J++) {
            for (int K = 0; K < 10; K++) {
                float r = 2 + K*0.2;
                for (int i = 0; i < 16; i++) {
                    float theta = i*2*M_PI/16;
                    tower.push_back(new GeometricPrimitive(new Sphere(Transform::translate(I*15 + cos(theta)*r,K,J*15 + sin(theta)*r), 0.5)));
                }
            }
        }
        primitives.push_back(new BVH(tower));
    }

    primitives.push_back(new GeometricPrimitive(new Plane(Point(0,-0.5,0), Vector(1,0,0), Vector(0,0,1), 1000, 1000)));

    scene->add_light(new PointLight(Point(0,60,0), 3500.f*RGB(0.6,0.6,0.9)));

    BVH *bvh = new BVH(primitives);
    scene->add_primitive(bvh);

    return scene;
}
