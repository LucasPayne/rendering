#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include "mathematics.hpp"
#include <vector>

class LocalGeometry;

class Primitive {
private:    
public:    
    Transform object_to_world;
    Transform world_to_object;

    Primitive() {}
    Primitive(const Transform &o2w) {
        object_to_world = o2w;
        world_to_object = o2w.inverse();
    }
    virtual bool intersect(Ray &ray, LocalGeometry *info) = 0;
};


class LocalGeometry {
private:
public:
    Primitive *primitive;
    Point p;
    Vector n;

    LocalGeometry() {}
    LocalGeometry(Primitive &_primitive, Point _p, Vector _n) {
        primitive = &_primitive;
        p = _p;
        n = _n;
    }
};

// Routine to intersect with a vector of primitives. This acts as a basic list, whose members are exhaustively searched.
// -
// This should be a type of primitive instead of requiring a separate routine.
bool intersect_primitive_vector(Ray &ray, std::vector<Primitive *> &primitives, LocalGeometry *geom);

#include "primitives/sphere.hpp"
#include "primitives/triangle_mesh.hpp"

#endif // PRIMITIVES_H
