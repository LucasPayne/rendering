#include "textures.hpp"

RGB Texture::rgb_lookup(const LocalGeometry &geom)
{
    return RGB(0,1,1);
}
float Texture::float_lookup(const LocalGeometry &geom)
{
    return 0;
}

RGB ConstantTextureRGB::rgb_lookup(const LocalGeometry &geom)
{
    return m_rgb;
}
float ConstantTextureFloat::float_lookup(const LocalGeometry &geom)
{
    return m_value;
}


RGB FrandTextureRGB::rgb_lookup(const LocalGeometry &geom)
{
    return RGB(frand(), frand(), frand());
}
