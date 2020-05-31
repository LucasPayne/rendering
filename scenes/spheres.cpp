#include "ray_tracer.hpp"

#define PHONG_NORMALS true

Scene *make_scene() {
    Scene *scene = new Scene();
    vector<Primitive *> primitives(0);


    for (int N = 0; N < 2; N++) {
        int n = 5 + N*10;
        for (int i = 0; i < n; i++) {
        float theta = i*2*M_PI/n;
        float r = 2+N;
        // Texture *sphere_tex = new ConstantTextureRGB(RGB(0.5,0.5,0.5));
        primitives.push_back(new GeometricPrimitive(
                             new Sphere(Transform::translate(r*cos(theta),0.5,r*sin(theta)+2.5), 0.3f + N*0.1f),
                             NULL,
                             NULL,
                             1
                             ));
        }
    }
    scene->add_light(new PointLight(Point(0,1,0), 50.f*RGB(1,1,1)));

    primitives.push_back(new GeometricPrimitive(
                         new Plane(Point(0,-0.5,0), Vector(1,0,0), Vector(0,0,1), 1000, 1000),
			 new ConstantTextureRGB(RGB(1,0.6,0.4)),
                         NULL));

    BVH *bvh = new BVH(primitives);
    scene->add_primitive(bvh);

    return scene;
}
