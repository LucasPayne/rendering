#ifndef ILLUMINATION_LIGHT_H
#define ILLUMINATION_LIGHT_H
#include "illumination/color.hpp"
#include "primitives.hpp"

class Light {
private:
public:
    // Create a ray from an intersection point directed toward the light source.
    // A LocalGeometry is passed since the created ray will probably need to be shifted along the normal.
    virtual Ray light_ray(const LocalGeometry &geom) = 0;
    // Assuming there is no obstruction, give the intensity and color of the light hitting
    // (this is a very rough model ...)
    virtual float illumination(const LocalGeometry &geom, RGB &color) = 0;
};

#endif // ILLUMINATION_LIGHT_H
