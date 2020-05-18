#include <iostream>
#include "mathematics/geometry.hpp"

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
