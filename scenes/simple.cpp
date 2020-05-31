#include "ray_tracer.hpp"

Scene *make_scene() {
    Scene *scene = new Scene();
    vector<Primitive *> primitives(0);

#if 0
    Model *bunny = load_OFF_model("models/bunny.off", 1, Point(0,1,0), true, PHONG_NORMALS);
    primitives.push_back(new GeometricPrimitive(
                         new TriangleMesh(Transform::translate(3,-0.7,5) * Transform::y_rotation(1.5*M_PI/2),
                         bunny),
                         new FrandTextureRGB(),
                         NULL,
                         0.4
                         ));
#endif

#if 0
    Model *icosahedron = load_OFF_model("models/icosahedron.off", 1, Point(0,0,0), false, false);
    for (int i = 0; i < 3; i++) {
    primitives.push_back(new GeometricPrimitive(
                         new TriangleMesh(Transform::translate(-3+3*i,i==1 ? 3.5 : 3.2,5) * Transform::y_rotation(i*0.3+i*i),
                         icosahedron),
                         //new FrandTextureRGB()));
                         new ConstantTextureRGB(RGB(0.8,0.8,1)),
                         NULL,
                         0
                         ));
    }
#endif


    primitives.push_back(new GeometricPrimitive(
                         new Sphere(Transform::translate(0,0.5,6), 2),
                         NULL,
                         NULL,
                         1
                         ));
    scene->add_light(new PointLight(Point(0,10,0), 50.f*RGB(1,1,1)));

    float size = 15;
    float height = -1;

    // floor
    int tile = 10;
    primitives.push_back(new GeometricPrimitive(
                         new Plane(Point(0,height,6), Vector(1,0,0), Vector(0,0,1), size, size),
                         new CheckerTexture(tile,tile,
                            new ConstantTextureRGB(RGB(0.8,0.8,1)),
                            new CheckerTexture(tile*tile,tile*tile,
                               new ConstantTextureRGB(RGB(1,0.8,0.8)),
                               new ConstantTextureRGB(RGB(1,1,1))
		            )
		         ),
                         NULL, 0, 1));

    // walls
    primitives.push_back(new GeometricPrimitive(new Plane(Point(-5,height,6), Vector(0,1,0), Vector(0,0,-1), size, size),
                         NULL, NULL, 0));
    primitives.push_back(new GeometricPrimitive(new Plane(Point(5,height,6), Vector(0,1,0), Vector(0,0,1), size, size),
                         NULL, NULL, 0));
    primitives.push_back(new GeometricPrimitive(new Plane(Point(0,height,11), Vector(1,0,0), Vector(0,1,0), size, size),
                         NULL, NULL, 0.6));
    primitives.push_back(new GeometricPrimitive(new Plane(Point(0,height,-1), Vector(-1,0,0), Vector(0,1,0), size, size),
                         NULL, NULL, 0.6));

    

    BVH *bvh = new BVH(primitives);
    scene->add_primitive(bvh);

    return scene;
}
