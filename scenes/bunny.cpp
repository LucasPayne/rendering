#include "ray_tracer.hpp"

#define PHONG_NORMALS true

Scene *make_scene() {
    Scene *scene = new Scene();
    vector<Primitive *> primitives(0);


    Model *bunny = load_OFF_model("models/bunny.off", 1, Point(0,1,0), true, PHONG_NORMALS);
    primitives.push_back(new GeometricPrimitive(
                         new TriangleMesh(Transform::translate(0,-0.7,5) * Transform::y_rotation(1.5*M_PI/2),
                         bunny)));
    Model *dragon = load_OFF_model("models/dragon.off", 2, Point(0,0,0), true, PHONG_NORMALS);
    primitives.push_back(new GeometricPrimitive(
                         new TriangleMesh(Transform::translate(3.45,-1.4,5) * Transform::y_rotation(0.5),
                         dragon)));
    Model *apple = load_OFF_model("models/apple.off", 20, Point(0,0,0), true, PHONG_NORMALS);
    primitives.push_back(new GeometricPrimitive(
                         new TriangleMesh(Transform::translate(-3,-0.7,5) * Transform::y_rotation(1),
                         apple)));
    Model *icosahedron = load_OFF_model("models/icosahedron.off", 1, Point(0,0,0), false, false);
    for (int i = 0; i < 3; i++) {
    primitives.push_back(new GeometricPrimitive(
                         new TriangleMesh(Transform::translate(-3+3*i,3.2,5) * Transform::y_rotation(i*0.3+i*i),
                         icosahedron),
                         new FrandTextureRGB()));
    }
    scene->add_light(new PointLight(Point(0,10,0), 50.f*RGB(1,1,1)));
    scene->add_light(new PointLight(Point(-3,4,2), 15.f*RGB(0.4,0.4,1)));

    // scene->add_light(new PointLight(Point(0,10,0), 50.f*RGB(1,0.956,0.43)));
    // scene->add_light(new PointLight(Point(0,2,5), 20.f*RGB(0.5,0.5,1)));

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
