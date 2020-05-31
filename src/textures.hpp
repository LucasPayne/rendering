#ifndef TEXTURES_H
#define TEXTURES_H
#include "core.hpp"
#include "shapes.hpp"

class TextureMapping {
public:
    
};

class Texture {
public:
    virtual RGB rgb_lookup(const LocalGeometry &geom);
    virtual float float_lookup(const LocalGeometry &geom);
};

class CheckerTexture : public Texture {
public:
    // This texture uses UV coordinates, probably of a rectangle.
    RGB rgb_lookup(const LocalGeometry &geom);
    Texture *textures[2];
    int grid_x;
    int grid_y;
    CheckerTexture(int _grid_x, int _grid_y, Texture *texture_A, Texture *texture_B) {
        grid_x = _grid_x;
        grid_y = _grid_y;
        textures[0] = texture_A;
        textures[1] = texture_B;
    }
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
