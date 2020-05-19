#include "illumination.hpp"
#include "primitives.hpp"
#include <glm/glm.hpp>

Ray PointLight::light_ray(const LocalGeometry &geom)
{
    Ray ray(geom.p, position - geom.p);
    return ray;
}
float PointLight::illumination(const LocalGeometry &geom, RGB &out_color)
{
    const float min_length = 1e-3;
    Vector dir = position - geom.p;
    float length = glm::length(dir);
    if (length < min_length) length = min_length;
    dir = dir / length;
    Vector n = -glm::normalize(geom.n); //-------normals
    float costheta = glm::dot(dir, n);

    float attenuation = 1.0 / (length * length);

    out_color = color;

    // std::cout << intensity << ", " << attenuation << ", " << costheta << "\n";

    return intensity * attenuation * (costheta < 0 ? 0 : costheta);
}
