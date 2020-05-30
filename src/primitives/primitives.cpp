#include "primitives.hpp"

static bool g_initialized_default_textures = false;
static Texture *g_default_diffuse_texture = NULL;
static Texture *g_default_specular_texture = NULL;
static void initialize_default_textures()
{
    g_default_diffuse_texture = new ConstantTextureRGB(RGB(1,1,1));
    g_default_specular_texture = new ConstantTextureFloat(0);
}
GeometricPrimitive::GeometricPrimitive(Shape *_shape,
	                               Texture *_diffuse_texture,
	                               Texture *_specular_texture)
{
    shape = _shape;
    if (_diffuse_texture == NULL) {
        if (!g_initialized_default_textures) initialize_default_textures();
        diffuse_texture = g_default_diffuse_texture;
    } else {
        diffuse_texture = _diffuse_texture;
    }
    if (_specular_texture == NULL) {
        if (!g_initialized_default_textures) initialize_default_textures();
        specular_texture = g_default_specular_texture;
    } else {
        specular_texture = _specular_texture;
    }
}

bool Primitive::intersect(Ray &ray, Intersection *inter) {
    std::cerr << "ERROR: Unimplemented intersect() routine of primitive called.\n";
    exit(EXIT_FAILURE);
}
bool Aggregate::intersect(Ray &ray, Intersection *inter) {
    std::cerr << "ERROR: Unimplemented intersect() routine of aggregate called.\n";
    exit(EXIT_FAILURE);
}
