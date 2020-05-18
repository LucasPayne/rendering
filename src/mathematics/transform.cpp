/*================================================================================
    Transform
================================================================================*/
#include "mathematics/transform.hpp"

Transform Transform::translate(const Vector &v)
{
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
Transform Transform::translate(float x, float y, float z)
{
    mat4x4 m(1,0,0,0,
             0,1,0,0,
             0,0,1,0,
             x,y,z,1);
    mat4x4 minv(1,0,0,0,
                0,1,0,0,
                0,0,1,0,
                -x,-y,-z,1);
    return Transform(m, minv);
}
Transform Transform::lookat(const Point &origin, const Point &lookat, const Vector &up)
{
    Vector forward = glm::normalize(lookat - origin);
    Vector right = glm::cross(forward, up);
    Vector true_up = glm::cross(right, forward);
    mat4x4 m(right.x, right.y, right.z, 0,
             true_up.x, true_up.y, true_up.z, 0,
             forward.x, forward.y, forward.z, 0,
             origin.x, origin.y, origin.z, 1);
    return Transform(m);
}
