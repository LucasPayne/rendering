#include "ray_tracer.hpp"

Scene *make_scene() {
    Scene *scene = new Scene();

    vector<Primitive *> primitives(0);


    //int n = 3;
    int n = 1;
    for (int I = -n; I <= n; I++) {
        for (int J = -n; J <= n; J++) {
            for (int K = 0; K < 10; K++) {
                float r = 2 + K*0.2;
                PrimitiveList *sphere_list = new PrimitiveList();
                for (int i = 0; i < 16; i++) {
                    float theta = i*2*M_PI/16;

                    //GeometricPrimitive *sphere = new GeometricPrimitive(new Sphere(Transform::translate(I*15 + cos(theta)*r,K,J*15 + sin(theta)*r), 0.5));
                    //sphere_list->add(sphere);
                    primitives.push_back(new GeometricPrimitive(new Sphere(Transform::translate(I*15 + cos(theta)*r,K,J*15 + sin(theta)*r), 0.5)));
                }
                //scene->add_primitive(sphere_list);
            }
        }
    }
    // scene->add_primitive(new TriangleMesh(Transform::translate(0,0.2,0),
    //                                       load_OFF_model("models/bunny.off", 0.5, Point(0,1,0), true)));

    // scene->add_primitive(new GeometricPrimitive(new Plane(Point(0,-0.5,0), Vector(1,0,0), Vector(0,0,1), 1000, 1000)));
    primitives.push_back(new GeometricPrimitive(new Plane(Point(0,-0.5,0), Vector(1,0,0), Vector(0,0,1), 1000, 1000)));

    // scene->add_primitive(new GeometricPrimitive(new Plane(Point(0,-0.5,0), Vector(1,0,0), Vector(0,0,1), 10, 10)));
    // scene->add_primitive(new GeometricPrimitive(new Plane(Point(-5,0,0), Vector(0,1,0), Vector(0,0,1), 10, 10)));
    // scene->add_primitive(new GeometricPrimitive(new Plane(Point(5,0,0), Vector(0,1,0), Vector(0,0,-1), 10, 10)));
    // scene->add_primitive(new GeometricPrimitive(new Plane(Point(0,0,5), Vector(1,0,0), Vector(0,1,0), 10, 10)));

    // scene->add_primitive(new GeometricPrimitive(new Sphere(Transform::translate(0,20,0), 4)));

    scene->add_light(new PointLight(Point(0,30,0), 700.f*RGB(0.6,0.6,0.9)));


    // scene->add_light(new PointLight(Point(0,4,0), 20.f*RGB(0.6,0.6,0.9)));
    // scene->add_light(new PointLight(Point(-3,1,0), 20.f*RGB(1,0.6,0.5)));

    // scene->add_primitive(new TriangleMesh(Transform::translate(0,2,0) * Transform::y_rotation(0.7), load_OFF_model("models/icosahedron.off")));

    BVH *bvh = new BVH(primitives);
    scene->add_primitive(bvh);
    // scene->add_primitive(new PrimitiveList(primitives));

    return scene;
}
