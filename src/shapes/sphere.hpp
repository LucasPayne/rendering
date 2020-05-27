#ifndef SHAPES_SPHERE_H
#define SHAPES_SPHERE_H
#include "shapes.hpp"

class Sphere : public Shape {
private:
    float m_radius;
public:
    Sphere(const Transform &o2w, float radius) :
        Shape(o2w)
    {
        m_radius = radius;
    }
    // Shape implementations.
    bool intersect(Ray &ray, LocalGeometry *geom) const;
    BoundingBox object_bound() const;
};

#endif // PRIMITIVES_SPHERE_H

