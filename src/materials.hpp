#ifndef MATERIALS_H
#define MATERIALS_H

#define MAX_NUM_MATERIAL_TEXTURES 4

class Material {
    Texture *textures[MAX_NUM_MATERIAL_TEXTURES];
    
};


class FlatDiffuseMaterial : public Material {
    RGB color;
    FlatDiffuseMaterial(RGB _color) :
        color{_color}
    {}
};

#endif // MATERIALS_H
