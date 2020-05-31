#include "textures.hpp"

static TextureMapper *g_default_mapper = NULL;
static void default_mapper_initialize()
{
    // By default, the uv coordinates available (if any are set by the intersection routine) as
    // the uv parameter coordinates in the LocalGeometry.
    // This mapping can be changed from the default, to give e.g. cylindrical projections.
    g_default_mapper = new GeometryMapper();
}


void CylinderMapper::get_uv(const LocalGeometry &geom, float *u, float *v)
{
    // Transform the intersection point to object space.
    Point op = geom.shape->world_to_object(geom.p);
    // Height along the cylinder.
    *v = op.y;

    float theta = atan2f(op.z, op.x);
    // printf("%.2f\n", theta);
    *u = 0.5 + theta * (0.5 / M_PI);
}


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


RGB CheckerTexture::rgb_lookup(const LocalGeometry &geom)
{
    float u, v;
    get_uv(geom, &u, &v);
    return textures[((int)(u*grid_x) + (int)(v*grid_y)) % 2]->rgb_lookup(geom);
}

CheckerTexture::CheckerTexture(int _grid_x, int _grid_y, Texture *texture_A, Texture *texture_B, TextureMapper *_mapper) {
    // Take a TextureMapper, since this uses UV lookups.
    grid_x = _grid_x;
    grid_y = _grid_y;
    textures[0] = texture_A;
    textures[1] = texture_B;
    if (_mapper == NULL) {
        if (g_default_mapper == NULL) default_mapper_initialize();
        mapper = g_default_mapper;
    } else {
        mapper = _mapper;
    }
}

ImageTextureRGB::ImageTextureRGB(const string &filename, bool linear, TextureMapper *_mapper)
{
    m_image = new TextureBMP(filename.c_str());
    m_linear = linear;

    if (_mapper == NULL) {
        if (g_default_mapper == NULL) default_mapper_initialize();
        mapper = g_default_mapper;
    } else {
        mapper = _mapper;
    }
}
RGB ImageTextureRGB::rgb_lookup(const LocalGeometry &geom)
{
    float u,v;
    get_uv(geom, &u, &v);
    glm::vec3 rgb_v = m_image->getColorAt(u, v);
    return RGB(rgb_v.x, rgb_v.y, rgb_v.z);
}
