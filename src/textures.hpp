#ifndef TEXTURES_H
#define TEXTURES_H
#include "core.hpp"
#include "shapes.hpp"

#include "ext/TextureBMP.h"

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
    ConstantTextureRGB(RGB rgb) :
        m_rgb{rgb}
    {}
    ConstantTextureRGB(float r, float g, float b) {
        m_rgb = RGB(r,g,b);
    }
private:
    RGB m_rgb;
};

class ConstantTextureFloat : public Texture {
public:
    float float_lookup(const LocalGeometry &geom);
    ConstantTextureFloat(float value) :
        m_value{value}
    {}
private:
    float m_value;
};

class ImageTextureRGB : public Texture {
public:
    ImageTextureRGB(string const &filename, bool linear = false, TextureMapper *_mapper = NULL);
    RGB rgb_lookup(const LocalGeometry &geom);
private:
    bool m_linear; // false: nearest
    TextureBMP *m_image;
};

#endif // TEXTURES_H
