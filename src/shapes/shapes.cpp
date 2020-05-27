#include "mathematics.hpp"
#include "shape.hpp"

bool Shape::intersect(Ray &ray, LocalGeometry *geom)
{
    ERROR("Non implemented Shape::intersect method has been called.");
    return false;
}
bool Shape::does_intersect(Ray &ray)
{
    // This method should be overwritten if there is a more efficient way to do the test (without giving local geometry information).
    LocalGeometry geom;
    return intersect(ray, &geom);
};
