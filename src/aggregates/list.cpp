#include "primitives.hpp"
#include <vector>

Primitive *PrimitiveList::add(Primitive *primitive)
{
    primitives.push_back(primitive);
    // Make sure to update the world-space bounding box.
    m_bounding_box.enlarge(primitive->world_bound());
    return primitive;
}

BoundingBox PrimitiveList::world_bound()
{
    // The bounding box is cached in the PrimitiveList, and built up when the list is being added to.
    return m_bounding_box;
}

// Intersection routines are just passed down to every primitive.
bool PrimitiveList::intersect(Ray &ray, LocalGeometry *out_geom)
{
    bool intersected = false;
    LocalGeometry geom;
    for (const Primitive * &primitive : primitives) {
        LocalGeometry primitive_geom;
        if (primitive->intersect(ray, &primitive_geom)) {
            intersected = true;
            geom = primitive_geom; // Copy this over, since maybe a primitive that is not intersected with may still alter the geom structure
                                   // (although this shouldn't be done in primitive code).
        }
    }
    if (intersected) *out_geom = geom;
    return intersected;
}
bool PrimitiveList::does_intersect(Ray &ray)
{
    for (const Primitive * &primitive : primitives) {
        if (primitive->does_intersect(ray)) return true;
    }
    return false;
}
