#ifndef SCENE_H
#define SCENE_H
#include "core.hpp"
#include "illumination.hpp"
#include "primitives.hpp"
#include "aggregates.hpp"

/*--------------------------------------------------------------------------------
    A scene is itself an aggregate primitive. In this way, rendering code
    can just do scene->intersect(ray, &geom).
--------------------------------------------------------------------------------*/
class Scene : public Aggregate {
private:
public:
    PrimitiveList primitives; // The root primitive.
    std::vector<Light *> lights;
    Scene() {
        primitives = PrimitiveList();
        lights = std::vector<Light *>(0);
    }
    void add_primitive(Primitive *prim);
    void add_light(Light *light);
    // Aggregate-primitive interface implementations (just passing to underlying aggregate primitive holding the scene primitives).
    BoundingBox world_bound() const;
    bool intersect(Ray &ray, Intersection *inter);
    bool does_intersect(Ray &ray) const;
};


#endif // SCENE_H
