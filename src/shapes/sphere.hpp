#ifndef SHAPES_SPHERE_H
#define SHAPES_SPHERE_H
#include "shapes.hpp"

class Sphere : public Shape {
private:
    float m_radius;
    Point m_position; // remember to change this is the transform is changed!
public:
    Sphere(const Transform &o2w, float radius) :
        Shape(o2w)
    {
        m_position = o2w(Point(0,0,0));
        m_radius = radius;
    }
    // Shape implementations.
    bool intersect(Ray &ray, LocalGeometry *geom) const;
    bool does_intersect(Ray &ray) const;
    BoundingBox object_bound() const;
};

#endif // PRIMITIVES_SPHERE_H

