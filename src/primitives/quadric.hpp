#ifndef PRIMITIVES_QUADRIC_H
#define PRIMITIVES_QUADRIC_H
#include "mathematics.hpp"
#include "primitives.hpp"
#include <glm/glm.hpp>
typedef glm::mat4x4 mat4x4;
#include <string.h>

class Quadric : public Primitive {
public:
    // a,b,c,d,e,f,g,h,i,j
    // The quadric is formed as
    //    [a e f g]
    //    [  b h i]
    // X^T[    c j]X = X^T A X = 0.
    //    [      d]

    // This routine redefines this quadric to pass through the 9 given points, if they are in general position.
    void pass_through_9_points(Point points[/* 9 */]);

    mat4x4 A() const {
        // Return the symmetric matrix defining the quadric.
        float a,b,c,d,e,f,g,h,i,j;
        a = coefficients[0]; b = coefficients[1]; c = coefficients[2]; d = coefficients[3]; e = coefficients[4];
        f = coefficients[5]; g = coefficients[6]; h = coefficients[7]; i = coefficients[8]; j = coefficients[9];
        float vals[16] = {
            a, e, f, g,
            e, b, h, i,
            f, h, c, j,
            g, i, j, d,
        };
        mat4x4 matrix;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                matrix[i][j] = vals[4*i + j];
            }
        }
        return matrix;
    }

    Quadric(const Transform &o2w) :
        Primitive(o2w)
    {
        // This constructor gives the unit sphere.
        memset(coefficients, 0, sizeof(coefficients));
        coefficients[0] = 1;
        coefficients[1] = 1;
        coefficients[2] = 1;
        coefficients[3] = -1;
    }
    bool intersect(Ray &ray, LocalGeometry *geom);

    float coefficients[10];
private:
};

#endif // PRIMITIVES_QUADRIC_H
