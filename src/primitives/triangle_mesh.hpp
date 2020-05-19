#ifndef PRIMITIVES_TRIANGLE_MESH_H
#define PRIMITIVES_TRIANGLE_MESH_H
#include "primitives.hpp"
#include "models.hpp"
#include <string>

class TriangleMesh : public Primitive {
private:
public:
    Model model;
    TriangleMesh(const Transform &o2w, std::string const &filename) :
        Primitive(o2w)
    {
        // Assume that this is the path of an OFF file.
        model = load_OFF_model(filename);
    }
    bool intersect(Ray &ray, LocalGeometry *geom);
};

#endif // PRIMITIVES_TRIANGLE_MESH_H
