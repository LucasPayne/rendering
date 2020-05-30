#include "ray_tracer.hpp"

Scene *make_scene() {
    Scene *scene = new Scene();
    vector<Primitive *> primitives(0);

#if 1
    float r = 20;
    int n = 300;
    for (int i = 0; i < n; i++) {
        primitives.push_back(new GeometricPrimitive(new Sphere(Transform::translate(2+r*frand()-5,r*frand(),12+r*frand()-5), 0.8*frand()+0.2)));
        primitives.push_back(new GeometricPrimitive(new Plane(Point(2+r*frand()-5,r*frand(),12+r*frand()-5), Vector(frand()-0.5,frand()-0.5,frand()-0.5),
                             Vector(frand()-0.5,frand()-0.5,frand()-0.5), 1, 1)));
    }
    primitives.push_back(new GeometricPrimitive(new Plane(Point(0,-0.5,0), Vector(1,0,0), Vector(0,0,1), 1000, 1000)));
    scene->add_light(new PointLight(Point(0,30,0), 700.f*RGB(0.6,0.6,0.9)));


    Model *apple = load_OFF_model("models/apple.off", 30, Point(0,0,0), true);
    float R = 7;
    for (int i = 0; i < 8; i++) {
    primitives.push_back(new GeometricPrimitive(
                         new TriangleMesh(Transform::translate(frand()*R,frand()*R,frand()*R),
                         apple)));
    }
#endif
    scene->add_light(new PointLight(Point(-3,5,0), 20.f*RGB(0.6,0.956,0.43)));
    scene->add_light(new PointLight(Point(3,4,2), 16.f*RGB(0.5,0.5,1)));

    Model *bunny = load_OFF_model("models/bunny.off", 1, Point(0,1,0), true);
    primitives.push_back(new GeometricPrimitive(
                         new TriangleMesh(Transform::translate(0,2,0),
                         bunny)));
    

#if 1
    BVH *bvh = new BVH(primitives);
    scene->add_primitive(bvh);
#else
    scene->add_primitive(new PrimitiveList(primitives));
#endif

    return scene;
}
