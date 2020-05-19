#include "scene.hpp"
#include "mathematics.hpp"
#include "primitives.hpp"

bool Scene::hit_by_ray(Ray ray)
{
    // No intersect predicate method, just use the full one.
    LocalGeometry geom;
    return intersect_primitive_vector(ray, primitives, geom);
}
