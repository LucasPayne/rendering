#ifndef TEXTURES_H
#define TEXTURES_H
#include "core.hpp"
#include "shapes.hpp"

class TextureMapper {
public:
    virtual void get_uv(const LocalGeometry &geom, float *u, float *v) = 0;
};

class CylinderMapper : public TextureMapper {
public:
    CylinderMapper() {}
    void get_uv(const LocalGeometry &geom, float *u, float *v);
};


class GeometryMapper : public TextureMapper {
public:
    GeometryMapper() {}
    void get_uv(const LocalGeometry &geom, float *u, float *v) {
        *u = geom.u;
        *v = geom.v;
    }
};

class Texture {
public:
    virtual RGB rgb_lookup(const LocalGeometry &geom);
    virtual float float_lookup(const LocalGeometry &geom);
    TextureMapper *mapper;
    void get_uv(const LocalGeometry &geom, float *u, float *v) {
        mapper->get_uv(geom, u, v);
    }
};

class CheckerTexture : public Texture {
public:
    // This texture uses UV coordinates, probably of a rectangle.
    RGB rgb_lookup(const LocalGeometry &geom);
    Texture *textures[2];
    int grid_x;
    int grid_y;
    CheckerTexture(int _grid_x, int _grid_y, Texture *texture_A, Texture *texture_B, TextureMapper *_mapper = NULL);
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
    ConstantTextureRGB(float r, float g, float b) {
        m_rgb = RGB(r,g,b);
    }
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
