#ifndef CORE_TRANSFORM_H
#define CORE_TRANSFORM_H
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4x4 mat4x4;
#include "mathematics/geometry.hpp"

/*================================================================================
    A Transform stores a matrix and its inverse.
================================================================================*/
class Transform {
private:
    mat4x4 matrix;
    mat4x4 inverse_matrix;
public:
    Transform() {
        // No-parameter constructor gives the identity Transform.
        memset(&matrix, 0, sizeof(mat4x4));
        memset(&inverse_matrix, 0, sizeof(mat4x4));
        for (int i = 0; i < 4; i++) {
            matrix[i][i] = 1.f;
            inverse_matrix[i][i] = 1.f;
        }
    }
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
    inline Transform inverse() const {
        return Transform(inverse_matrix, matrix);
    }

    // Static methods.
    static Transform lookat(const Point &origin, const Point &lookat, const Vector &up);
    static Transform translate(const Vector &v);
    static Transform translate(float x, float y, float z);
};

#endif // CORE_TRANSFORM_H
