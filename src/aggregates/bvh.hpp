#ifndef PRIMITIVE_AGGREGATE_BVH_H
#define PRIMITIVE_AGGREGATE_BVH_H
#include "primitives.hpp"

// Set this flag if a non-compacted (linked tree structure, probably inefficient) data structure is wanted.
// This is so I can benchmark and see how much better the compacted data structure is.
#define NO_COMPACTIFY 0

#if NO_COMPACTIFY
struct Node;
struct PrimitiveInfo;
#endif

struct BVHNode {
    BVHNode() {}
    BoundingBox box; //size: 6 floats, 24 bytes
    union {
        uint32_t primitives_offset;
        uint32_t second_child_offset;
    };
    uint8_t num_primitives;
    uint8_t __pad[3];
};

class BVH : public Aggregate {
public:
    BVH() {}
    BVH(const vector<Primitive *> &primitives);
    
    // Aggregate-Primitive interface implementations.
    BoundingBox world_bound() const;
    BoundingBox object_bound() const;
    bool intersect(Ray &ray, LocalGeometry *info) const;
    bool does_intersect(Ray &ray) const;

#if NO_COMPACTIFY
    Node *uncompacted_root;
    vector<PrimitiveInfo> uncompacted_p_infos;
#endif
private:
    BoundingBox m_box; // Bounds all the internal primitives (this is the same as the bounding box of the root node).
    vector<Primitive *> primitives;
    vector<BVHNode> compacted;
};

#endif // PRIMITIVE_AGGREGATE_BVH_H
