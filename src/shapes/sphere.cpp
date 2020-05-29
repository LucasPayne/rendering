/*--------------------------------------------------------------------------------
    Sphere primitive.
--------------------------------------------------------------------------------*/
#include "shapes.hpp"

bool Sphere::intersect(Ray &in_ray, LocalGeometry *geom) const
{
    Ray ray = world_to_object(in_ray);

    float a,b,c;
    a = ray.d.x*ray.d.x + ray.d.y*ray.d.y + ray.d.z*ray.d.z;
    b = 2*(ray.o.x*ray.d.x + ray.o.y*ray.d.y + ray.o.z*ray.d.z);
    c = ray.o.x*ray.o.x + ray.o.y*ray.o.y + ray.o.z*ray.o.z - m_radius*m_radius;
    float discriminant = b*b - 4*a*c;
    if (discriminant < 0) return false;
    float sqrt_d = sqrt(discriminant);
    float inv_2a = 1.0 / (2*a);
    float t = (-b - sqrt_d)*inv_2a;
    if (t < ray.min_t || t > ray.max_t) {
        t = (-b + sqrt_d)*inv_2a;
        if (t < ray.min_t || t > ray.max_t) return false;
    }
    geom->shape = this;
    in_ray.max_t = t;
    geom->p = in_ray(t);
    geom->n = geom->p - m_position;
    return true;
}

bool Sphere::does_intersect(Ray &ray) const
{
    float a,b,c;
    Vector m = ray.o - m_position;
    a = ray.d.x*ray.d.x + ray.d.y*ray.d.y + ray.d.z*ray.d.z;
    b = 2*(m.x*ray.d.x + m.y*ray.d.y + m.z*ray.d.z);
    c = m.x*m.x + m.y*m.y + m.z*m.z - m_radius*m_radius;
    float discriminant = b*b - 4*a*c;
    if (discriminant < 0) return false;
    float sqrt_d = sqrt(discriminant);
    float t2a = -b - sqrt_d;
    float min_t2a = 2*a*ray.min_t;
    float max_t2a = 2*a*ray.max_t;
    if (t2a < min_t2a || t2a > max_t2a) {
        t2a = -b + sqrt_d;
        return t2a >= min_t2a && t2a <= max_t2a;
    }
    return true;
}


BoundingBox Sphere::object_bound() const
{
    Point p = Point(m_radius, m_radius, m_radius);
    return BoundingBox(-p, p);
}
