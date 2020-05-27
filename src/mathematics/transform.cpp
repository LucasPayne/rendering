/*================================================================================
    Transform
================================================================================*/
#include <math.h>
#include "mathematics/transform.hpp"

// Use a macro, since the translate function variants only vary in
// where x,y,z come from.
#define TRANSFORM_TRANSLATE(X,Y,Z)\
    mat4x4 m(1,0,0,0,\
             0,1,0,0,\
             0,0,1,0,\
             ( X ),( Y ),( Z ),1);\
    mat4x4 minv(1,0,0,0,\
                0,1,0,0,\
                0,0,1,0,\
                -( X ),-( Y ),-( Z ),1);\
    return Transform(m, minv);
Transform Transform::translate(const Vector &v)
{
    TRANSFORM_TRANSLATE(v.x, v.y, v.z);
}
Transform Transform::translate(const Point &p)
{
    TRANSFORM_TRANSLATE(p.x, p.y, p.z);
}
Transform Transform::translate(float x, float y, float z)
{
    TRANSFORM_TRANSLATE(x, y, z);
}
#undef TRANSFORM_TRANSLATE

Transform Transform::lookat(const Point &origin, const Point &lookat, const Vector &up)
{
    Vector forward = glm::normalize(lookat - origin);
    // For a left-handed coordinate system, this order of cross products seems to be correct.
    // Taking X x Y = Z, this means that in a left-handed system, when looking from above at two vectors
    // the cross product comes toward you if they are _clockwise_.
    Vector right = glm::cross(up, forward);
    Vector true_up = glm::cross(forward, right);
    mat4x4 m(right.x, right.y, right.z, 0,
             true_up.x, true_up.y, true_up.z, 0,
             forward.x, forward.y, forward.z, 0,
             origin.x, origin.y, origin.z, 1);
    return Transform(m);
}

// Multiplying transforms gives their composition, (A,A^-1)*(B,B^-1) = (AB,B^-1 A^-1).
Transform operator*(const Transform &A, const Transform &B)
{
    return Transform(A.matrix * B.matrix, B.inverse_matrix * A.inverse_matrix);
}

Transform Transform::x_rotation(float theta)
{
    float c = cos(theta);
    float s = sin(theta);
    mat4x4 m(1,0,0,0,
             0,c,s,0,
             0,-s,c,0,
             0,0,0,1);
    mat4x4 minv(1,0,0,0,
                0,c,-s,0,
                0,s,c,0,
                0,0,0,1);
    return Transform(m, minv);
}
Transform Transform::y_rotation(float theta)
{
    float c = cos(theta);
    float s = sin(theta);
    mat4x4 m(c,0,s,0,
             0,1,0,0,
             -s,0,c,0,
             0,0,0,1);
    mat4x4 minv(c,0,-s,0,
             0,1,0,0,
             s,0,c,0,
             0,0,0,1);
    return Transform(m, minv);
}
Transform Transform::z_rotation(float theta)
{
    float c = cos(theta);
    float s = sin(theta);
    mat4x4 m(c,s,0,0,
             -s,c,0,0,
             0,0,1,0,
             0,0,0,1);
    mat4x4 minv(c,-s,0,0,
             s,c,0,0,
             0,0,1,0,
             0,0,0,1);
    return Transform(m, minv);
}
Transform Transform::extrinsic_euler_angles_XYZ(float theta_x, float theta_y, float theta_z)
{
    return (Transform::z_rotation(theta_z) * Transform::y_rotation(theta_y)) * Transform::x_rotation(theta_x);
}

Transform Transform::scale(float amount)
{
    float inv_amount = 1.0 / amount;
    mat4x4 m(amount,0,0,0,
             0,amount,0,0,
             0,0,amount,0,
             0,0,0,1);
    mat4x4 minv(inv_amount,0,0,0,
             0,inv_amount,0,0,
             0,0,inv_amount,0,
             0,0,0,1);
    return Transform(m, minv);
}

// Transform an axis-aligned bounding box by bounding its oriented image.
BoundingBox Transform::operator()(const BoundingBox &box) const
{
    const Transform &T = *this;
    // Initialize to a transformed corner.
    BoundingBox box_p(T(box[0]));
    // Enlarge the box minimally to contain each successive transformed point.
    for (int i = 1; i < 8; i++) {
        box_p.enlarge(T(box[i]));
    }
    // The box is not the minimal aabb for the transformed points.
    return box_p;
}

