#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <iostream>
#include <glm/glm.hpp>
#include <cmath>
// Some geometric classes wrap a glm implementation.
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4x4 mat4x4;

/*================================================================================
    A Vector is a directional vector.
-
    This is currently just the glm vec3.
================================================================================*/
typedef vec3 Vector;
// Print a Vector.
std::ostream &operator<<(std::ostream &os, const Vector &vector);

/*================================================================================
   A Point is a position. The difference between Points is a Vector.
   p + v: Vectors can be added to Points, but points cannot be added.
================================================================================*/
class Point {
public:
    float x, y, z;
    Point() {
        x = y = z = 0.f;
    }
    Point(float _x, float _y, float _z) :
        x{_x},
        y{_y},
        z{_z} {
    }
    Point(vec3 v) :
        x{v.x},
        y{v.y},
        z{v.z} {
    }
    // Construct a point from a vec4. This does perspective division if necessary.
    Point(vec4 v) {
        if (v.w == 1.f) {
            x = v.x;
            y = v.y;
            z = v.z;
        } else {
            float winv = 1.f / v.w;
            x = winv * x;
            y = winv * y;
            z = winv * z;
        }
    }
    inline Vector operator-(const Point &other_p) const {
        return Vector(x - other_p.x, y - other_p.y, z - other_p.z);
    }
    inline Point operator+(const Vector &v) const {
        return Point(x + v.x, y + v.y, z + v.z);
    }
    inline Point &operator+=(const Vector &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return (*this);
    }
    inline Point &operator-=(const Vector &v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return (*this);
    }
};



// Print a Point.
std::ostream &operator<<(std::ostream &os, const Point &point);

/*===============================================================================
    A Ray has an origin, and a (not neccessarily normalized) direction. It
    can be thought of as a function r(t) giving points along the ray.

    This is not a pure geometric ray, but one to be used for ray tracing.
===============================================================================*/
class Ray {
public:
    Point o;
    Vector d;
    float min_t;
    float max_t;

    Ray() {}
    Ray(const Point &_origin, const Vector &_direction) :
        o{_origin}, d{_direction}
    {
        min_t = 1e-5;
        max_t = INFINITY;
    }

    inline void normalize() {
        glm::normalize(d);
    }

    // Evaluate the ray with its corresponding parameterization, e.g. r(5).
    inline Point operator()(float t) const {
        return o + t*d;
    }
};
// Print a Ray.
std::ostream &operator<<(std::ostream &os, const Ray &ray);

// Print a glm::mat4x4.
std::ostream &operator<<(std::ostream &os, const mat4x4 &m);


#endif // GEOMETRY_H
