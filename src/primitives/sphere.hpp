#ifndef PRIMITIVES_SPHERE_H
#define PRIMITIVES_SPHERE_H
#include "primitives.hpp"

class Sphere : public Primitive {
private:
    float m_radius;
public:
    Sphere(const Transform &o2w, float radius) :
        Primitive(o2w)
    {
        m_radius = radius;
    }
    bool intersect(Ray &ray, LocalGeometry *geom);
};

#endif // PRIMITIVES_SPHERE_H

