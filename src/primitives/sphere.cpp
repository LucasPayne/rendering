/*--------------------------------------------------------------------------------
    Sphere primitive.
--------------------------------------------------------------------------------*/
#include <math.h>
#include "mathematics.hpp"
#include "primitives.hpp"
#include "primitives/sphere.hpp"

bool Sphere::intersect(Ray &in_ray, HitInfo *info)
{
    Ray ray = world_to_object(in_ray);

    // Solve the quadratic at^2 + bt + c = 0.
    float a = ray.d.x*ray.d.x + ray.d.y*ray.d.y + ray.d.z*ray.d.z;
    float b = 2*(ray.o.x*ray.d.x + ray.o.y*ray.d.y + ray.o.z*ray.d.z);
    float c = ray.o.x*ray.o.x + ray.o.y*ray.o.y + ray.o.z*ray.o.z - m_radius*m_radius;

    float discriminant = b*b - 4*a*c;
    if (discriminant < 0) return false;
    float inv2a = 1.0 / 2*a;
    float sqrtd = sqrt(discriminant);
    float root1, root2, temp;
    root1 = inv2a * (-b + sqrtd);
    root2 = inv2a * (-b - sqrtd);
    if (root1 > root2) {
        temp = root1;
        root1 = root2;
        root2 = temp;
    }
    if (root1 < ray.min_t || root1 > ray.max_t) {
        // Use root2 instead.
        if (root2 < ray.min_t || root2 > ray.max_t) return false;
        in_ray.max_t = root2;
    } else {
        in_ray.max_t = root1;
    }
    info->primitive = this;
    info->point = object_to_world(ray(in_ray.max_t));
    return true;
}
