#ifndef ILLUMINATION_POINT_LIGHT
#define ILLUMINATION_POINT_LIGHT

class PointLight : public Light {
private:
    Point position;
    float intensity;
    RGB color;
public:
    PointLight () {}
    PointLight (Point _position, float _intensity, RGB _color) {
        position = _position;
        intensity = _intensity;
        color = _color;
    }
    Ray light_ray(const LocalGeometry &geom);
    float illumination(const LocalGeometry &geom, RGB &out_color);
};

#endif // ILLUMINATION_POINT_LIGHT
