/*--------------------------------------------------------------------------------
    Sphere primitive.
--------------------------------------------------------------------------------*/
#include <math.h>
#include "mathematics.hpp"
#include "primitives.hpp"
#include "primitives/sphere.hpp"

bool Sphere::intersect(Ray &in_ray, LocalGeometry *geom)
{
    Ray ray = world_to_object(in_ray);

    Point &o = ray.o;
    Vector &d = ray.d;

    // Solve the quadratic at^2 + bt + c = 0.
    float a = d.x*d.x + d.y*d.y + d.z*d.z;
    float b = 2*(o.x*d.x + o.y*d.y + o.z*d.z);
    float c = o.x*o.x + o.y*o.y + o.z*o.z - m_radius*m_radius;

    float discriminant = b*b - 4*a*c;
    if (discriminant < 0) return false;
    float inv2a = 1.0 / (2*a);
    float sqrtd = sqrt(discriminant);
    float root1, root2, temp;
    root1 = inv2a * (-b + sqrtd);
    root2 = inv2a * (-b - sqrtd);
    if (root1 > root2) {
        temp = root1;
        root1 = root2;
        root2 = temp;
    }
    std::cout << "root 1: " << root1 << "\n";
    std::cout << "root 2: " << root2 << "\n";

    if (root1 < ray.min_t || root1 > ray.max_t) {
        // Use root2 instead.
        if (root2 < ray.min_t || root2 > ray.max_t) return false;
        in_ray.max_t = root2;
    } else {
        in_ray.max_t = root1;
    }
    geom->primitive = this;
    geom->p = in_ray.o + in_ray.d * in_ray.max_t;
    geom->n = geom->p - object_to_world.position();
    return true;
}
