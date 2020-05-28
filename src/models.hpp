#ifndef MODELS_H
#define MODELS_H
#include "core.hpp"
#include "primitives.hpp"

// Model with a mesh, vertex attributes (UV coordinates, normals).
struct Model {
    int num_vertices;
    std::vector<Point> vertices;

    // Triangle model values.
    int num_triangles;
    std::vector<uint16_t> triangles; // length is 3*num_triangles.

    // Stored vertex attributes.
    bool has_normals; // Otherwise these will be computed if needed.
    std::vector<Vector> normals;

    Model() {
        num_vertices = 0;
        num_triangles = 0;
        has_normals = false;
    }
    Model(std::vector<Point> &_vertices, int _num_vertices, std::vector<uint16_t> &_triangles, int _num_triangles) {
        vertices = _vertices;
        num_vertices = _num_vertices;
        triangles = _triangles;
        num_triangles = _num_triangles;
        has_normals = false;
    }
    void print_properties() const;
};

Model load_OFF_model(std::string const &filename, float scale = 1.f, Point center = Point(0,0,0), bool invert_winding_order = false);

#endif // MODELS_H
