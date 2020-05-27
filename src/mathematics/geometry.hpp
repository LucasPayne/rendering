#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <iostream>
#include <glm/glm.hpp>
#include <math.h>

// Some geometric classes wrap a glm implementation.
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4x4 mat4x4;

// Print a glm::mat4x4.
std::ostream &operator<<(std::ostream &os, const mat4x4 &m);

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
struct Point {
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
    inline Point operator-() const {
        return Point(-x,-y,-z);
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
struct Ray {
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

/*================================================================================
    A BoundingBox is axis-aligned in some space (the geometric object doesn't
    neccessarily bound something, but it is called this because that what it is for).

    There is an "algebra of bounding boxes", with the "union" (called enlarge, since
    union is a keyword and its not a set union) of two bounding boxes (or a bounding box
    and a point, treated as a bounding box).

    This is made more natural by making the default constructor give an "empty box", which
    cannot just be a degenerate point. This is done by making it not have correctly ordered corners,
    the min being the maximum possible values of each coordinate, the max being the minimum.
    This means that in unions/enlargments with other bounding boxes, this will
    act as the "identity box".
================================================================================*/
struct BoundingBox {
    BoundingBox () {
        // Return the "identity box"
        min_corner = Point(INFINITY,INFINITY,INFINITY);
        max_corner = Point(-INFINITY,-INFINITY,-INFINITY);
    }
    BoundingBox (const Point &p) {
        // Return the minimum box around the point p.
        min_corner = p;
        max_corner = p;
    }
    BoundingBox (const Point &p1, const Point &p2) {
        // Return the minimum box around p1 and p2.
        // It is not assumed that each coordinate of p1 and p2 is correctly ordered.
        min_corner = Point(min(p1.x, p2.x), min(p1.y, p2.y), min(p1.z, p2.z));
        max_corner = Point(max(p1.x, p2.x), max(p1.y, p2.y), max(p1.z, p2.z));
    }

    // The box corners are indexable.
    inline Point operator[](int index) const {
        #define greatest_x ((index) & 1)
        #define greatest_y (((index) >> 1) & 1)
        #define greatest_z (((index) >> 2) & 1)
        return Point(greatest_x ? max_corner.x : min_corner.x,
                     greatest_y ? may_corner.y : min_corner.y,
                     greatest_z ? maz_corner.z : min_corner.z);
        #undef greatest_x
        #undef greatest_y
        #undef greatest_z
    }

    // Methods to minimally enlarge the box to contain other objects.
    // (the "algebra of bounding boxes")
    inline BoundingBox enlarge(const BoundingBox &other_box) {
        min_corner.x = fmin(min_corner.x, other_box.min_corner.x);
        min_corner.y = fmin(min_corner.y, other_box.min_corner.y);
        min_corner.z = fmin(min_corner.z, other_box.min_corner.z);
        max_corner.x = fmax(max_corner.x, other_box.max_corner.x);
        max_corner.y = fmax(may_corner.y, other_box.max_corner.y);
        max_corner.z = fmax(maz_corner.z, other_box.max_corner.z);
    }
    inline BoundingBox enlarge(const Point &encase_point) {
        min_corner.x = fmin(min_corner.x, encase_point.x);
        min_corner.y = fmin(min_corner.y, encase_point.y);
        min_corner.z = fmin(min_corner.z, encase_point.z);
        max_corner.x = fmax(max_corner.x, encase_point.x);
        max_corner.y = fmax(may_corner.y, encase_point.y);
        max_corner.z = fmax(maz_corner.z, encase_point.z);
    }
    // These friend methods construct a new box instead of editing in-place.
    friend enlarged(const BoundingBox &box, const BoundingBox &other_box);
    friend enlarged(const BoundingBox &box, const Point &point);

    Point min_corner;
    Point max_corner;
};



#endif // GEOMETRY_H
