#include "ray_tracer.hpp"

Scene *make_scene() {
    Scene *scene = new Scene();
    vector<Primitive *> primitives(0);

#if 1
{
    Model *bunny = load_OFF_model("models/bunny.off", 1.5, Point(0,1,0), true, true);
    primitives.push_back(new GeometricPrimitive(
                         new TriangleMesh(Transform::translate(3.5,0,4) * Transform::y_rotation(1.5*M_PI/2),
                         bunny),
                         new CheckerTexture(40,5,
                             new ConstantTextureRGB(1,1,1),
                             new ConstantTextureRGB(0,0,1),
                             new CylinderMapper()),
                         NULL,
                         0
                         ));
}
{
#if 0
    Model *bunny = load_OFF_model("models/bunny.off", 1.7, Point(0,1,0), true, true);
    primitives.push_back(new GeometricPrimitive(
                         new TriangleMesh(Transform::translate(-5,0.3,4) * Transform::y_rotation(2.6),
                         bunny),
                         NULL,
                         NULL,
                         0,
                         1.2
                         ));
#endif
    Model *icosahedron = load_OFF_model("models/icosahedron.off", 0.8, Point(0,0,0), false, false);
    Point positions[3] = {
        {-3.7,4,5},
        {0,4.2,4.8},
        {3.7,4,5},
    };
    for (int i = 0; i < 3; i++) {
    primitives.push_back(new GeometricPrimitive(
                         new TriangleMesh(Transform::translate(positions[i]) * Transform::y_rotation(1),
                         icosahedron),
                         new ConstantTextureRGB(RGB(0.76,0.76,1)),
                         NULL, 0, 1.5
                         ));
    }
}
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


    Point sphere_pos(0,1.8,0);
#if 0
    primitives.push_back(new GeometricPrimitive(
                         new Sphere(Transform::translate(sphere_pos), 2),
                         NULL,
                         NULL,
                         1
                         ));
#else
    primitives.push_back(new GeometricPrimitive(
                         new Sphere(Transform::translate(sphere_pos), 2),
                         NULL,
                         NULL,
                         1
                         ));
#endif
    int num_spheres = 12;
    Texture *sphere_tex = new ConstantTextureRGB(RGB(1,0.6,0.4));
    for (int i = 0; i < num_spheres; i++) {
        float r = 2.5;
        float theta = i*2*M_PI/num_spheres;
        primitives.push_back(new GeometricPrimitive(
                             new Sphere(Transform::translate(sphere_pos + Vector(cos(theta)*r,-0.5,sin(theta)*r)), 0.4),
                             sphere_tex,
                             NULL,
                             0,
                             1.3
                             ));
    }


    scene->add_light(new PointLight(Point(-3,10,0), 25.f*RGB(1,1,1)));
    scene->add_light(new PointLight(Point(4,8,4), 10.f*RGB(1,0,0)));

    float size = 15;
    float height = -1;


    // floor
    float room_size = 7;
    int tile = 10;
    primitives.push_back(new GeometricPrimitive(
                         new Plane(Point(0,0,0), Vector(1,0,0), Vector(0,0,1), 2*room_size, 2*room_size),
                         new CheckerTexture(tile,tile,
                            new ConstantTextureRGB(RGB(0.8,0.8,1)),
                            new CheckerTexture(tile*tile,tile*tile,
                               new ConstantTextureRGB(RGB(1,0.8,0.8)),
                               new ConstantTextureRGB(RGB(1,1,1))
		            )
		         ),
                         NULL, 0, 1));
    // roof
    primitives.push_back(new GeometricPrimitive(
                         new Plane(Point(0,1.5*room_size,0), Vector(1,0,0), Vector(0,0,-1), 2*room_size, 2*room_size),
                         new CheckerTexture(tile,tile,
                            new ConstantTextureRGB(RGB(0.8,0.8,1)),
                            new CheckerTexture(tile*tile,tile*tile,
                               new ConstantTextureRGB(RGB(1,0.8,0.8)),
                               new ConstantTextureRGB(RGB(1,1,1))
		            )
		         ),
                         NULL, 0, 1));
    // walls
    primitives.push_back(new GeometricPrimitive(new Plane(Point(-room_size,room_size*0.5,0), Vector(0,1,0), Vector(0,0,-1), 2*room_size, 2*room_size),
                         NULL, NULL, 0));
    primitives.push_back(new GeometricPrimitive(new Plane(Point(room_size,room_size*0.5,0), Vector(0,1,0), Vector(0,0,1), 2*room_size, 2*room_size),
                         NULL, NULL, 0));
    primitives.push_back(new GeometricPrimitive(new Plane(Point(0,room_size*0.5,room_size), Vector(1,0,0), Vector(0,1,0), 2*room_size, 2*room_size),
                         NULL, NULL, 0.4));
    primitives.push_back(new GeometricPrimitive(new Plane(Point(0,room_size*0.5,-room_size), Vector(1,0,0), Vector(0,-1,0), 2*room_size, 2*room_size),
                         NULL, NULL, 0.4));
    

    BVH *bvh = new BVH(primitives);
    scene->add_primitive(bvh);

    return scene;
}
