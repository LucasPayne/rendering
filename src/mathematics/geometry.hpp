#ifndef GEOMETRY_H
#define GEOMETRY_H
#include "core.hpp"

// Print a glm::mat4x4.
std::ostream &operator<<(std::ostream &os, const mat4x4 &m);

/*================================================================================
    A Vector is a directional vector.
-
    This is currently just the glm vec3.
================================================================================*/
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
// These make sense for barycentric combinations.
inline Point operator*(float t, const Point &p)
{
    return Point(t*p.x, t*p.y, t*p.z);
}
inline Point operator*(const Point &p, float t)
{
    return Point(t*p.x, t*p.y, t*p.z);
}
inline Point operator+(const Point &p, const Point &q)
{
    return Point(p.x*q.x, p.y*q.y, p.z*q.z);
}


// inconvenient and ugly, should just not use glm vectors ... 
inline Vector PointToVector(const Point &p) { return Vector(p.x,p.y,p.z); }

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
        corners[0] = Point(INFINITY,INFINITY,INFINITY);
        corners[1] = Point(-INFINITY,-INFINITY,-INFINITY);
    }
    BoundingBox (const Point &p) {
        // Return the minimum box around the point p.
        corners[0] = p;
        corners[1] = p;
    }
    BoundingBox (const Point &p1, const Point &p2) {
        // Return the minimum box around p1 and p2.
        // It is not assumed that each coordinate of p1 and p2 is correctly ordered.
        corners[0] = Point(fmin(p1.x, p2.x), fmin(p1.y, p2.y), fmin(p1.z, p2.z));
        corners[1] = Point(fmax(p1.x, p2.x), fmax(p1.y, p2.y), fmax(p1.z, p2.z));
    }

    // The box corners are indexable (all 8 of them).
    inline Point operator[](int index) const {
        #define greatest_x ((index) & 1)
        #define greatest_y (((index) >> 1) & 1)
        #define greatest_z (((index) >> 2) & 1)
        return Point(greatest_x ? corners[1].x : corners[0].x,
                     greatest_y ? corners[1].y : corners[0].y,
                     greatest_z ? corners[1].z : corners[0].z);
        #undef greatest_x
        #undef greatest_y
        #undef greatest_z
    }

    // Methods to minimally enlarge the box to contain other objects.
    // (the "algebra of bounding boxes")
    inline BoundingBox enlarge(const BoundingBox &other_box) {
        corners[0].x = fmin(corners[0].x, other_box.corners[0].x);
        corners[0].y = fmin(corners[0].y, other_box.corners[0].y);
        corners[0].z = fmin(corners[0].z, other_box.corners[0].z);
        corners[1].x = fmax(corners[1].x, other_box.corners[1].x);
        corners[1].y = fmax(corners[1].y, other_box.corners[1].y);
        corners[1].z = fmax(corners[1].z, other_box.corners[1].z);
    }
    inline BoundingBox enlarge(const Point &encase_point) {
        corners[0].x = fmin(corners[0].x, encase_point.x);
        corners[0].y = fmin(corners[0].y, encase_point.y);
        corners[0].z = fmin(corners[0].z, encase_point.z);
        corners[1].x = fmax(corners[1].x, encase_point.x);
        corners[1].y = fmax(corners[1].y, encase_point.y);
        corners[1].z = fmax(corners[1].z, encase_point.z);
    }
    // These friend methods construct a new box instead of editing in-place.
    friend BoundingBox enlarged(const BoundingBox &box, const BoundingBox &other_box);
    friend BoundingBox enlarged(const BoundingBox &box, const Point &point);

    // Bounding box intersection with a ray. If they intersect, the
    // range along the ray that is on or inside the box is returned.
    // (This is the intersection of the range of the ray segment and the range that the line determined by the ray
    //  intersects with the box.)
    bool intersect(const Ray &ray, float *t0, float *t1) const;
    bool intersect(const Ray &ray) const;
    
    Point corners[2]; // min, max

    inline Point min_corner() const { return corners[0]; };
    inline Point max_corner() const { return corners[1]; };
};
// Print a BoundingBox.
std::ostream &operator<<(std::ostream &os, const BoundingBox &box);

#endif // GEOMETRY_H
