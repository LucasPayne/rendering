#ifndef ILLUMINATION_POINT_LIGHT
#define ILLUMINATION_POINT_LIGHT
#include "mathematics.hpp"
#include "illumination/color.hpp"

class PointLight : public Light {
private:
    RGB intensity; // Intensity is a wavelength-thing, and that is approximately modelled with RGB.
    Point position;
public:
    PointLight (Point _position, RGB _intensity) :
        Light(Transform::translate(position)),
        position {_position},
        intensity {_intensity}
    { }
    Ray light_ray(const LocalGeometry &geom);
    RGB radiance(const Point &p, Vector *light_vector, VisibilityTester *visibility_tester);
};

#endif // ILLUMINATION_POINT_LIGHT
