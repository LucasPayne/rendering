/*================================================================================
    Transform
================================================================================*/
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
