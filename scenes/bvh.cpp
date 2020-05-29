#include "ray_tracer.hpp"

Scene *make_scene() {
    Scene *scene = new Scene();
    vector<Primitive *> primitives(0);

    float r = 20;
    int n = 300;
    for (int i = 0; i < n; i++) {
        primitives.push_back(new GeometricPrimitive(new Sphere(Transform::translate(2+r*frand()-5,r*frand(),12+r*frand()-5), 0.8*frand()+0.2)));
        primitives.push_back(new GeometricPrimitive(new Plane(Point(2+r*frand()-5,r*frand(),12+r*frand()-5), Vector(frand()-0.5,frand()-0.5,frand()-0.5),
                             Vector(frand()-0.5,frand()-0.5,frand()-0.5), 1, 1)));
    }
    primitives.push_back(new GeometricPrimitive(new Plane(Point(0,-0.5,0), Vector(1,0,0), Vector(0,0,1), 1000, 1000)));
    scene->add_light(new PointLight(Point(0,30,0), 700.f*RGB(0.6,0.6,0.9)));

#if 1
    BVH *bvh = new BVH(primitives);
    scene->add_primitive(bvh);
#else
    scene->add_primitive(new PrimitiveList(primitives));
#endif

    return scene;
}
