#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include "illumination.hpp"
#include "mathematics.hpp"

class HitInfo;

class Primitive {
private:    
public:    
    Transform object_to_world;
    Transform world_to_object;

    RGB TEST_COLOR; // For example, it may be useful when developing to turn off shading and just see if geometry works.

    Primitive() {}
    Primitive(const Transform &o2w) {
        object_to_world = o2w;
        world_to_object = o2w.inverse();
        TEST_COLOR = RGB(1,0,1);
    }
    virtual bool intersect(Ray &ray, HitInfo *info) = 0;
};


class HitInfo {
private:
public:
    Primitive *primitive;
    Point point;

    HitInfo() {}
    HitInfo(Primitive &_primitive) {
        primitive = &_primitive;
    }
};

#include "primitives/sphere.hpp"

#endif // PRIMITIVES_H
