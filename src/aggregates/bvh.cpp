/*--------------------------------------------------------------------------------
    The bounding volume heirarchy implementation is very much based off of
    pbrt 2e's description and code for bvh construction.
--------------------------------------------------------------------------------*/
#include "aggregates/bvh.hpp"
#include <algorithm>

struct PrimitiveInfo {
    PrimitiveInfo(const Primitive *primitive) {
        box = primitive->world_bound();
        // Converting to a Vector here since Point arithmetic is a bit restrictive.
        centroid = 0.5f*Vector(box.min_corner.x,box.min_corner.y,box.min_corner.z) +
                   0.5f*Vector(box.max_corner.x,box.max_corner.y,box.max_corner.z);
    }
    BoundingBox box;
    Vector centroid;
};
struct Node {
    Node(int _first_primitive, int _num_primitives) :
        // Construct a leaf node (referencing a range of primitives).
        first_primitive{_first_primitive},
        num_primitives{_num_primitives}
    {
        children[0] = children[1] = NULL;
    }
    Node(Node *child_1, Node *child_2) {
        // Construct a branching node.
        children[0] = child_1;
        children[1] = child_1;
        box = enlarged(child_1->box, child_2->box);
    }

    BoundingBox box; // Bound this sub-BVH. 
    Node *children[2]; // Branching nodes.
    int first_primitive; // Leaf nodes.
    int num_primitives; // Leaf nodes.
};

// This acts as a "unary predicate" when passed to std::partition.
struct Comparer {
    Comparer(float _compare_to_value, int _splitting_dimension) :
        compare_to_value{_compare_to_value},
        splitting_dimension{_splitting_dimension}
    {}
    float compare_to_value;
    int splitting_dimension;
    bool operator()(const PrimitiveInfo &primitive_info) const {
        return primitive_info.centroid[splitting_dimension] <= compare_to_value;
    }
};

static Node *BVH_create_node(vector<PrimitiveInfo> &p_infos,
                             int first_primitive,
                             int num_primitives)
{
    static int max_num_primitives_in_leaf = 1;
    if (num_primitives <= max_num_primitives_in_leaf) {
        // Leaf node.
        return new Node(first_primitive, num_primitives);
    }

    // bvh heuristic: Split across the dimension with the greatest extents of centroids of primitive bounding volumes.
    BoundingBox centroid_bounds; //--- default constructor?
    for (int i = first_primitive; i < first_primitive + num_primitives; i++) {
        centroid_bounds.enlarge(p_infos[i].centroid);
    }
    Vector min_corner(centroid_bounds.min_corner.x, centroid_bounds.min_corner.y, centroid_bounds.min_corner.z);
    Vector max_corner(centroid_bounds.max_corner.x, centroid_bounds.max_corner.y, centroid_bounds.max_corner.z);
    // Compute the dimension of greatest extent (x:0, y:1, z:2).
    float max_extent = max_corner[0] - min_corner[0];
    int splitting_dimension = 0;
    for (int i = 1; i < 3; i++) {
        float new_extent = max_corner[i] - min_corner[i];
        if (new_extent > max_extent) {
            max_extent = new_extent;
            splitting_dimension = i;
        }
    }
    // bvh heuristic: Along the chosen dimension, choose a splitting value as
    // the middle value of the bounding box of the centroids.
    float split_value = 0.5f*min_corner[splitting_dimension] +
                        0.5f*max_corner[splitting_dimension];

    // Partition the subarray of p_ordered so that it looks like
    //    [...[compares less or equal, compares greater]...].
    PrimitiveInfo *mid_pointer = std::partition(&p_infos[first_primitive], &p_infos[first_primitive + num_primitives - 1]+1,
                                                Comparer(split_value, splitting_dimension));
    int mid = mid_pointer - &p_infos[0];
    
    return new Node(BVH_create_node(p_infos, first_primitive, mid),
                    BVH_create_node(p_infos, mid+1, mid+num_primitives));
}


BVH::BVH(const vector<Primitive *> &primitives)
{
    // Compute a more compact, homogeneous array of primitive information.
    vector<PrimitiveInfo> p_infos;
    p_infos.reserve(primitives.size());
    for (int i = 0; i < primitives.size(); i++) {
        p_infos.push_back(PrimitiveInfo(primitives[i]));
    }
    // Recursively construct a bounding volume heirarchy
    // by building a tree structure, where each node either
    // references two children via pointers or, for a leaf node, a
    // range of primitives in the PrimitiveInfo array.
    // 
    // This means that the PrimitiveInfo array must have subarrays partitioned while the
    // tree is being built, so that a flattened node consists of contiguous primitives.
    //
    // The tree is then converted into the actual data structure that will
    // be traversed.

    // The creation of the initial tree is done recursively. This
    // first call to the root recurs to calls to create its children, etc.
    // While recurring, the array is being rearranged so that nodes only need to hold ranges.

    Node *root = BVH_create_node(p_infos, 0, primitives.size());
}

BoundingBox BVH::world_bound() const
{
    return BoundingBox();
}
bool BVH::intersect(Ray &ray, LocalGeometry *info) const
{
    return false;
}
bool BVH::does_intersect(Ray &ray) const
{
    return false;
}
