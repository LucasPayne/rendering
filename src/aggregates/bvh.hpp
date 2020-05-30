#ifndef PRIMITIVE_AGGREGATE_BVH_H
#define PRIMITIVE_AGGREGATE_BVH_H
#include "primitives.hpp"

// Set this flag if a non-compacted (linked tree structure, probably inefficient) data structure is wanted.
// This is so I can benchmark and see how much better the compacted data structure is.
#define NO_COMPACTIFY 0

struct Node {
    Node(BoundingBox &_box, int _first_primitive, int _num_primitives) {
        // Construct a leaf node (referencing a range of primitives).
        box = _box;
        first_primitive = _first_primitive;
        num_primitives = _num_primitives;
        children[0] = children[1] = NULL;
    }
    Node(int _mid, Node *child_1, Node *child_2) {
        // Construct a branching node.
        children[0] = child_1;
        children[1] = child_2;
        box = enlarged(child_1->box, child_2->box);
        mid = mid;
    }
    bool is_leaf() const {
        return children[0] == NULL;
    } 

    BoundingBox box; // Bound this sub-BVH. 
    Node *children[2]; // Branching nodes.
    int mid; //---testing
    int first_primitive; // Leaf nodes.
    int num_primitives; // Leaf nodes.
};
#define IS_LEAF(NODE) (( NODE )->children[0] == NULL)
#if NO_COMPACTIFY
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
    BVH(const vector<Primitive *> &primitives, bool keep_root = false);
    
    // Aggregate-Primitive interface implementations.
    BoundingBox world_bound() const;
    BoundingBox object_bound() const;
    bool intersect(Ray &ray, LocalGeometry *info) const;
    bool does_intersect(Ray &ray) const;

    int flattened_length() const {
        // How many entries there are (branches and leaves) if flattened into a contiguous array.
        return compacted.size();
    }

    Node *uncompacted_root;
#if NO_COMPACTIFY
    vector<PrimitiveInfo> uncompacted_p_infos;
#endif
    BoundingBox m_box; // Bounds all the internal primitives (this is the same as the bounding box of the root node).
    vector<Primitive *> primitives;
    vector<BVHNode> compacted;
private:
};

#endif // PRIMITIVE_AGGREGATE_BVH_H
