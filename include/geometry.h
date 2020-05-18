#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <glm/glm.hpp>
#include <cmath>

typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4x4 mat4x4;

typedef vec3 Vector;
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
        return Vector(matrix * vec4(v.x, v.y, v.z, 1));
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
