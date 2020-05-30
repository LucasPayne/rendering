#include "aggregates/primitive_list.hpp"

Primitive *PrimitiveList::add(Primitive *primitive)
{
    primitives.push_back(primitive);
    // Make sure to update the world-space bounding box.
    m_world_bound.enlarge(primitive->world_bound());
    return primitive;
}

BoundingBox PrimitiveList::world_bound() const
{
    // The bounding box is cached in the PrimitiveList, and built up when the list is being added to.
    return m_world_bound;
}

// Intersection routines are just passed down to every primitive.
// However, first the bounding box is checked.
bool PrimitiveList::intersect(Ray &ray, Intersection *out_inter)
{
    if (!world_bound().intersect(ray)) return false;

    bool intersected = false;
    Intersection inter;
    for (Primitive * primitive : primitives) {
        if (primitive->intersect(ray, &inter)) {
            intersected = true;
        }
    }
    if (intersected) *out_inter = inter;
    return intersected;
}
bool PrimitiveList::does_intersect(Ray &ray) const
{
    if (!world_bound().intersect(ray)) return false;

    for (const Primitive * primitive : primitives) {
        if (primitive->does_intersect(ray)) return true;
    }
    return false;
}
