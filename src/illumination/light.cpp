#include "light.hpp"
#include "primitives.hpp"

// Test if this ray (or ray segment) intersects a primitive.
// Most likely this will be the scene or possibly some other aggregate primitive.
bool VisibilityTester::unoccluded(const Primitive *primitive)
{
    return !primitive->does_intersect(ray);
}
