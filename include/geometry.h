#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <iostream>
#include <glm/glm.hpp>
#include <cmath>

typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4x4 mat4x4;

typedef vec3 Vector;
inline std::ostream &operator<<(std::ostream &os, const Vector &vector)
{
    os << "Vector(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
    return os;
}

class Point {
public:
    float x, y, z;
    Point() {}
    Point(float _x, float _y, float _z) : x{_x}, y{_y}, z{_z} {}
    // Construct a point from a vec4. This does perspective division if necessary.
    Point(vec4 v) {
        if (v.w == 1.0) {
            x = v.x;
            y = v.y;
            z = v.z;
        } else {
            float winv = 1.0 / v.w;
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
};
inline std::ostream &operator<<(std::ostream &os, const Point &point)
{
    os << "Point(" << point.x << ", " << point.y << ", " << point.z << ")";
    return os;
}

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
inline std::ostream &operator<<(std::ostream &os, const Ray &ray)
{
    os << "Ray(" << ray.o << ", " << ray.d << ")";
    return os;
}

class Transform {
private:
    mat4x4 matrix;
    mat4x4 inverse_matrix;
public:
    Transform() {}
    Transform(mat4x4 _matrix) :
        matrix {_matrix},
        inverse_matrix {glm::inverse(_matrix)}
    {}
    // It may be more efficient for the caller to provide the inverse matrix, e.g. when constructing translation matrices.
    Transform(mat4x4 _matrix, mat4x4 _inverse_matrix) :
        matrix {_matrix},
        inverse_matrix {_inverse_matrix}
    {}

    inline Point operator()(const Point &p) const {
        return Point(matrix * vec4(p.x, p.y, p.z, 1));
    }
    inline Vector operator()(const Vector &v) const {
        return Vector(matrix * vec4(v.x, v.y, v.z, 0));
    }
    inline Ray operator()(const Ray &ray) const {
        return Ray((*this)(ray.o), (*this)(ray.d));
    }
    Transform inverse() const {
        return Transform(inverse_matrix, matrix);
    }

// Static methods
    static Transform translate(const Vector &v) {
        mat4x4 m(1,0,0,0,
                 0,1,0,0,
                 0,0,1,0,
                 v.x,v.y,v.z,1);
        mat4x4 minv(1,0,0,0,
                    0,1,0,0,
                    0,0,1,0,
                    -v.x,-v.y,-v.z,1);
	/*
        mat4x4 m(1,0,0,v.x,
                 0,1,0,v.y,
                 0,0,1,v.z,
                 0,0,0,1);
        mat4x4 minv(1,0,0,-v.x,
                    0,1,0,-v.y,
                    0,0,1,-v.z,
                    0,0,0,1);
        */
        return Transform(m, minv);
    }
    static Transform translate(float x, float y, float z) {
        mat4x4 m(1,0,0,0,
                 0,1,0,0,
                 0,0,1,0,
                 x,y,z,1);
        mat4x4 minv(1,0,0,0,
                    0,1,0,0,
                    0,0,1,0,
                    -x,-y,-z,1);
        /*
        mat4x4 m(1,0,0,x,
                 0,1,0,y,
                 0,0,1,z,
                 0,0,0,1);
        mat4x4 minv(1,0,0,-x,
                    0,1,0,-y,
                    0,0,1,-z,
                    0,0,0,1);
        */
        return Transform(m, minv);
    }
    static Transform lookat(const Point &origin, const Point &lookat, const Vector &up) {
        Vector forward = glm::normalize(lookat - origin);
        Vector right = glm::cross(forward, up);
        Vector true_up = glm::cross(right, forward);
        mat4x4 m(right.x, right.y, right.z, 0,
                 true_up.x, true_up.y, true_up.z, 0,
                 forward.x, forward.y, forward.z, 0,
                 origin.x, origin.y, origin.z, 1);
        return Transform(m);
    }
};

#endif // GEOMETRY_H
