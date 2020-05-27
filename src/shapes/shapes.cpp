#include <iostream>
#include "mathematics.hpp"
#include "shapes.hpp"

// The default world-space bounding box is the bounding box of the transformed object-space
// bounding box (The Transform::operator() method transforms bounding boxes this way).
BoundingBox Shape::world_bound() const
{
    return object_to_world(object_bound());
}

bool Shape::intersect(Ray &ray, LocalGeometry *geom) const
{
    std::cerr << "Non implemented Shape::intersect method has been called.\n";
    exit(EXIT_FAILURE);
    return false;
}
bool Shape::does_intersect(Ray &ray) const
{
    // This method should be overwritten if there is a more efficient way to do the test (without giving local geometry information).
    LocalGeometry geom;
    return intersect(ray, &geom);
};

void Shape::set_transform(const Transform &transform)
{
    object_to_world = transform;
    world_to_object = transform.inverse();
}
