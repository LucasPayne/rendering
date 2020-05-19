#ifndef MODELS_H
#define MODELS_H
#include "mathematics.hpp"
#include "primitives.hpp"
#include "illumination/color.hpp"
#include <string>
#include <vector>
#include <stdint.h>

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
    Model(std::vector<Point> &_vertices, std::vector<uint16_t> &_triangles) {
        vertices = _vertices;
        num_vertices = _vertices.size();
        triangles = _triangles;
        num_triangles = _triangles.size() / 3;
        has_normals = false;
    }
};

Model load_OFF_model(std::string const &filename);

#endif // MODELS_H
