#ifndef PRIMITIVES_TRIANGLE_MESH_H
#define PRIMITIVES_TRIANGLE_MESH_H
#include "core.hpp"
#include "shapes.hpp"
#include "primitives.hpp"
#include "models.hpp"
#include "aggregates.hpp"

struct TriangleNode {
    BoundingBox box; // 6 floats, 24 bytes
    uint16_t a;
    uint16_t b;
    uint16_t c;
    uint16_t next_shift; //0 if this is a branch.
    TriangleNode() {}
};



class TriangleMesh;

// This class is specifically for a triangle of a mesh.
class MeshTriangle : public Shape {
public:
    uint16_t indices[3];
    Point operator[](int index) const;
    Point a() const;
    Point b() const;
    Point c() const;
    bool intersect(Ray &ray, LocalGeometry *geom) const;
    bool does_intersect(Ray &ray) const;
    BoundingBox object_bound() const;

    MeshTriangle() {}
    MeshTriangle(TriangleMesh *_mesh, int ta, int tb, int tc)
    {
        indices[0] = ta;
        indices[1] = tb;
        indices[2] = tc;
        mesh = _mesh;

        // Form the bounding box around the three vertices.
        Vector minp = PointToVector(a());
        Vector maxp = PointToVector(a());
        for (int i = 1; i < 3; i++) {
            Vector b = PointToVector((*this)[i]);
            for (int j = 0; j < 3; j++) {
                if (b[j] < minp[j]) minp[j] = b[j];
                if (b[j] > maxp[j]) maxp[j] = b[j];
            }
        }
        m_box = BoundingBox(Point(minp), Point(maxp));
        
    }
private:
    TriangleMesh *mesh;
    BoundingBox m_box;
};

class TriangleMesh : public Shape {
public:
    Model *model;

    TriangleMesh(const Transform &o2w, Model *_model);

    bool intersect(Ray &ray, LocalGeometry *geom) const;
    bool does_intersect(Ray &ray) const;
    BoundingBox object_bound() const;
private:
    vector<TriangleNode> triangles_bvh;
};

#endif // PRIMITIVES_TRIANGLE_MESH_H
