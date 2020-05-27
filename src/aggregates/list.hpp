#ifndef PRIMITIVES_AGGREGATES_LIST_H
#define PRIMITIVES_AGGREGATES_LIST_H
#include "primitives.hpp"
#include <vector>

class List : public Aggregate {
private:
    float m_height;
    float m_width;
    Point m_position;

    // Remember to update these if the transform is changed!
    Vector m_normal;
    Vector m_x_vector;
    Vector m_y_vector;
public:
    Plane(Point position, Vector x_extent, Vector y_extent, float width, float height)
    {
        // orthonormalize an internal basis for the plane.
        x_extent = glm::normalize(x_extent);
        y_extent -= x_extent * glm::dot(y_extent, x_extent);
        y_extent = glm::normalize(y_extent);
        Vector z_extent = glm::cross(y_extent, x_extent); //--check the sign of this.
        m_x_vector = x_extent;
        m_y_vector = y_extent;
        m_normal = z_extent;

        mat4x4 matrix(
            x_extent.x, x_extent.y, x_extent.z, 0,
            y_extent.x, y_extent.y, y_extent.z, 0,
            z_extent.x, z_extent.y, z_extent.z, 0,
            position.x, position.y, position.z, 1
        );
        set_transform(Transform(matrix));

        m_width = width;
        m_height = height;
    }
    bool intersect(Ray &ray, LocalGeometry *geom);

    inline Vector normal() const { return m_normal; }
    // x and y internal basis vectors of the plane.
    inline Vector x_vector() const { return m_x_vector; }
    inline Vector y_vector() const { return m_y_vector; }
};

#endif // PRIMITIVES_PLANE_H



#endif // PRIMITIVES_AGGREGATES_LIST_H
