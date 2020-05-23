#include "mathematics.hpp"
#include "primitives/quadric.hpp"
#include <math.h>
#include <glm/glm.hpp>
typedef glm::mat4x4 mat4x4;
typedef glm::vec4 vec4;

void Quadric::pass_through_9_points(Point points[/* 9 */])
{
    /*
        A quadric has nine degrees of freedom; 10 coefficients in the matrix, up to a non-zero multiple.
        A quadric can be formed that passes through nine points in general position, by solving
        a 10x10 linear system,
            [ x_1^2  y_1^2  ... 2z_1w_1 ][a]   [0]
            [   .      .           .    ][.]   [.]
            [   .      .           .    ][.] = [.],
            [   .      .           .    ][.]   [.]
            [ x_9^2  y_9^2  ... 2z_9w_9 ][.]   [.]
            [ 1 ...................... 1][j]   [1]
        and using the (a,...,j) coefficients to form the symmetric matrix
                [a e f g]
                [  b h i]
            A = [    c j],
                [      d]
        which determines the quadric by
            x^T A x = 0,
        where x is a homogeneous point.
        The input points are given in Cartesian coordinates, and w_i above are implicitly 1.
    */
    Mat M(10,10);
    for (int i = 0; i < 9; i++) {
        float &x = points[i].x;
        float &y = points[i].y;
        float &z = points[i].z;
        M[i][0] = x * x;
        M[i][1] = y * y;
        M[i][2] = z * z;
        M[i][3] = 1;
        M[i][4] = 2 * x * y;
        M[i][5] = 2 * x * z;
        M[i][6] = 2 * x * 1;
        M[i][7] = 2 * y * z;
        M[i][8] = 2 * y * 1;
        M[i][9] = 2 * z * 1;
    }
    for (int i = 0; i < 10; i++) {
        M[9][i] = 1.0;
    }
    Vec b(10);
    b[9] = 1.0;

    std::cout << "Solving Mc = b, A = " << M;
    std::cout << "b = " << b;

    Vec c = Ax_b_solve(M, b);
    for (int i = 0; i < 10; i++) coefficients[i] = c[i];
}

bool Quadric::intersect(Ray &in_ray, LocalGeometry *geom)
{
    // To intersect a ray with a quadric, expand
    //     (o + td)^T A (o + td) = o^T A o + t^2 d^T A d + 2to^T A d,
    // then solve for t using the quadratic formula.

    Ray ray = world_to_object(in_ray);
    Point &o = ray.o;
    Vector &d = ray.d;
    
    mat4x4 A_matrix = A();
    vec4 ov(o.x, o.y, o.z, 1);
    vec4 dv(d.x, d.y, d.z, 0);
    
    // Solving at^2 + bt + c = 0.
    float a = glm::dot(dv, A_matrix * dv);
    float b = 2 * glm::dot(ov, A_matrix * dv);
    float c = glm::dot(ov, A_matrix * ov);
    float discriminant = b*b - 4*a*c;
    if (discriminant < 0) return false;
    float sqrt_discriminant = sqrt(discriminant);
    float inv_2a = 0.5 / a;
    float t1 = inv_2a * (-b - sqrt_discriminant);
    float t2 = inv_2a * (-b + sqrt_discriminant);

    //----Check t values.
    float t = t1;

    in_ray.max_t = t;
    geom->primitive = this;
    geom->p = in_ray.o + in_ray.d * in_ray.max_t;
    geom->n = geom->p - object_to_world.position();
    return true;
}
