#ifndef TEXTURES_H
#define TEXTURES_H
#include "core.hpp"
#include "shapes.hpp"

class TextureMapping {
    
};

class Texture {
public:
    virtual RGB rgb_lookup(const LocalGeometry &geom);
    virtual float float_lookup(const LocalGeometry &geom);
};


class FrandTextureRGB : public Texture {
public:
    RGB rgb_lookup(const LocalGeometry &geom);
    FrandTextureRGB() {}
};


class ConstantTextureRGB : public Texture {
public:
    RGB rgb_lookup(const LocalGeometry &geom);
    RGB m_rgb;
    ConstantTextureRGB(RGB rgb) :
        m_rgb{rgb}
    {}
};

class ConstantTextureFloat : public Texture {
public:
    float float_lookup(const LocalGeometry &geom);
    float m_value;
    ConstantTextureFloat(float value) :
        m_value{value}
    {}
};

#endif // TEXTURES_H
