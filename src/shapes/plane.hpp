#ifndef PRIMITIVES_PLANE_H
#define PRIMITIVES_PLANE_H
#include "shapes.hpp"

class Plane : public Shape {
public:
    Plane(Point position, Vector x_extent, Vector y_extent, float width, float height);

    inline Vector normal() const { return m_normal; }
    // x and y internal basis vectors of the plane.
    inline Vector x_vector() const { return m_x_vector; }
    inline Vector y_vector() const { return m_y_vector; }

    // Shape implementations
    bool intersect(Ray &ray, LocalGeometry *geom) const;
    bool does_intersect(Ray &in_ray) const;
    BoundingBox object_bound() const;

private:
    float m_height;
    float m_width;
    // Store these so they don't have to be computed for getting texture coordinates.
    float m_inv_height;
    float m_inv_width;
    Point m_position;

    //-----Remember to update these if the transform is changed!
    Vector m_normal;
    Vector m_x_vector;
    Vector m_y_vector;
};

#endif // PRIMITIVES_PLANE_H

