#include <iostream>
#include "mathematics/geometry.hpp"
#include <glm/glm.hpp>
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4x4 mat4x4;

/*--------------------------------------------------------------------------------
    Vector
--------------------------------------------------------------------------------*/
std::ostream &operator<<(std::ostream &os, const Vector &vector)
{
    os << "Vector(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
    return os;
}

/*--------------------------------------------------------------------------------
    Point
--------------------------------------------------------------------------------*/
std::ostream &operator<<(std::ostream &os, const Point &point)
{
    os << "Point(" << point.x << ", " << point.y << ", " << point.z << ")";
    return os;
}

/*--------------------------------------------------------------------------------
    Ray
--------------------------------------------------------------------------------*/
std::ostream &operator<<(std::ostream &os, const Ray &ray)
{
    os << "Ray(" << ray.o << ", " << ray.d << ")";
    return os;
}

/*--------------------------------------------------------------------------------
    glm::mat4x4
--------------------------------------------------------------------------------*/
std::ostream &operator<<(std::ostream &os, const mat4x4 &m)
{
    os << "mat4x4(";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            os << "    " << m[i][j] << (j<3?", ":"\n");
        }
    }
    os << ")\n";
}

/*--------------------------------------------------------------------------------
    BoundingBox
--------------------------------------------------------------------------------*/
// Friend methods.
BoundingBox enlarged(const BoundingBox &box, const BoundingBox &other_box)
{
    BoundingBox new_box = box;
    new_box.enlarge(other_box);
    return new_box;
}
BoundingBox enlarged(const BoundingBox &box, const Point &point)
{
    BoundingBox new_box = box;
    new_box.enlarge(point);
    return new_box;
}
