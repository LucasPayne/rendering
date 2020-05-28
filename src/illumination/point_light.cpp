#include "illumination.hpp"
#include "primitives.hpp"

Ray PointLight::light_ray(const LocalGeometry &geom)
{
    Ray ray(geom.p, position - geom.p);
    return ray;
}
RGB PointLight::radiance(const Point &p, Vector *light_vector, VisibilityTester *visibility_tester)
{
    const float min_length = 1e-3;
    Vector dir = position - p;
    float length = glm::length(dir);
    if (length < min_length) length = min_length;
    *light_vector = dir / length;
    visibility_tester->SetSegment(p, position);
    return intensity / (length * length);
}
