#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include "mathematics.hpp"

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
    LocalGeometry(Primitive &_primitive) {
        primitive = &_primitive;
    }
    LocalGeometry(Primitive &_primitive, Point _p, Vector _n) {
        primitive = &_primitive;
        p = _p;
        n = _n;
    }
};

#include "primitives/sphere.hpp"

#endif // PRIMITIVES_H
