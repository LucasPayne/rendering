#include "mathematics.hpp"
#include "primitives.hpp"
#include <vector>

bool intersect_primitive_vector(Ray &ray, std::vector<Primitive *> &primitives, LocalGeometry *geom)
{
    LocalGeometry geom_per;
    bool hit_any = false;
    for (Primitive *primitive : primitives) {
        if (primitive->intersect(ray, &geom_per)) {
            *geom = geom_per;
            hit_any = true;
        }
    }
    return hit_any;
}
