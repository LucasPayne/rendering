#ifndef PRIMITIVES_TRIANGLE_MESH_H
#define PRIMITIVES_TRIANGLE_MESH_H
#include "mathematics.hpp"
#include "primitives.hpp"
#include "models.hpp"
#include <string>

class TriangleMesh : public Primitive {
private:
public:
    Model model;
    TriangleMesh(const Transform &o2w, Model _model) :
        Primitive(o2w)
    {
        model = _model;
    }
    bool intersect(Ray &ray, LocalGeometry *geom);
};

#endif // PRIMITIVES_TRIANGLE_MESH_H
