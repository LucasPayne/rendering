/*--------------------------------------------------------------------------------
    The bounding volume heirarchy implementation is very much based off of
    pbrt 2e's description and code for bvh construction.
--------------------------------------------------------------------------------*/
#include "aggregates/bvh.hpp"
#include <algorithm>

struct PrimitiveInfo {
    PrimitiveInfo(Primitive *_primitive) {
        box = _primitive->world_bound();
        // Converting to a Vector here since Point arithmetic is a bit restrictive.
        centroid = 0.5f*Vector(box.corners[0].x,box.corners[0].y,box.corners[0].z) +
                   0.5f*Vector(box.corners[1].x,box.corners[1].y,box.corners[1].z);
        primitive = _primitive;
    }
    BoundingBox box;
    Vector centroid;
    Primitive *primitive;
};

// This acts as a "unary predicate" when passed to std::partition.
struct Comparer {
    Comparer(float _compare_to_value, int _splitting_dimension) {
        compare_to_value = _compare_to_value;
        splitting_dimension = _splitting_dimension;
    }
    float compare_to_value;
    int splitting_dimension;
    bool operator()(const PrimitiveInfo &primitive_info) const {
        // printf("splitting along %d %.2f\n", splitting_dimension, compare_to_value);getchar();
        return primitive_info.centroid[splitting_dimension] <= compare_to_value;
    }
};

static Node *BVH_create_node(vector<PrimitiveInfo> &p_infos,
                             int first_primitive,
                             int num_primitives,
                             int *tree_size)
{
    printf("first_primitive: %d\n", first_primitive);
    printf("num_primitives: %d\n", num_primitives);
    printf("tree_size: %d\n", *tree_size);
    printf("num p_infos: %zu\n", p_infos.size());

    (*tree_size)++;
    static int max_num_primitives_in_leaf = 1;
    if (num_primitives <= max_num_primitives_in_leaf) {
        // Leaf node.
        BoundingBox p_box = p_infos[first_primitive].box;
        for (int i = first_primitive+1; i < first_primitive + num_primitives; i++) {
            p_box.enlarge(p_box.enlarge(p_infos[i].box));
        }
        return new Node(p_box, first_primitive, num_primitives);
    }

    // bvh heuristic: Split across the dimension with the greatest extents of centroids of primitive bounding volumes.
    BoundingBox centroid_bounds = BoundingBox();
    for (int i = first_primitive; i < first_primitive + num_primitives; i++) {
        centroid_bounds.enlarge(p_infos[i].centroid);
    }
    Vector min_corner(centroid_bounds.corners[0].x, centroid_bounds.corners[0].y, centroid_bounds.corners[0].z);
    Vector max_corner(centroid_bounds.corners[1].x, centroid_bounds.corners[1].y, centroid_bounds.corners[1].z);

    const float stop_below_centroid_extents = 0.0001f;
    if (   max_corner.x - min_corner.x < stop_below_centroid_extents
        && max_corner.y - min_corner.y < stop_below_centroid_extents
        && max_corner.z - min_corner.z < stop_below_centroid_extents) {
        // Put all remaining primitives in a box.
        BoundingBox p_box = p_infos[first_primitive].box;
        for (int i = first_primitive+1; i < first_primitive + num_primitives; i++) {
            p_box.enlarge(p_box.enlarge(p_infos[i].box));
        }
        return new Node(p_box, first_primitive, num_primitives);
    }


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
    
    Node *child_1 = BVH_create_node(p_infos, first_primitive, mid - first_primitive, tree_size);
    Node *child_2 = BVH_create_node(p_infos, mid, first_primitive+num_primitives - mid, tree_size);
    return new Node(mid, child_1, child_2);
}
static void BVH_delete_node(Node *node)
{
    if (IS_LEAF(node)) delete node;
    else {
        BVH_delete_node(node->children[0]);
        BVH_delete_node(node->children[1]);
        delete node;
    }
}

// Debugging
static void print_node(Node *node, int depth = 0)
{
    // getchar();
    for (int i = 0; i < depth; i++) printf("  ");
    bool is_leaf = node->children[0] == NULL;
    if (is_leaf) {
        printf("first: %d, n: %d\n", node->first_primitive, node->num_primitives);
    } else {
        printf("split %d:\n", node->mid);
        print_node(node->children[0], depth + 1);
        print_node(node->children[1], depth + 1);
    }
}
static void print_compacted(vector<BVHNode> &compacted, int index = 0, int depth = 0)
{
    for (int i = 0; i < depth; i++) printf("  ");
    if (compacted[index].num_primitives == 0) {
        printf("branch:\n");
        print_compacted(compacted, index + 1, depth + 1);
        print_compacted(compacted, compacted[index].second_child_offset, depth + 1);
    } else {
        printf("first: %d, n: %d\n", compacted[index].primitives_offset, compacted[index].num_primitives);
    }
}

static uint32_t BVH_compactify_recur(vector<BVHNode> &compacted, Node *node, uint32_t start)
{
    if (node == NULL) {
        fprintf(stderr, "BVP construction error: A node in the initially created data structure is null!\n");
        exit(EXIT_FAILURE);
    }
    if (IS_LEAF(node)) {
        // printf("Packed leaf\n");
        compacted[start].box = node->box;
        compacted[start].primitives_offset = node->first_primitive;
        compacted[start].num_primitives = node->num_primitives;
        return start;
    } else {
        // printf("Branching first\n");
        // printf("start: %d, compacted.size: %zu\n", start, compacted.size());
        compacted[start].box = node->box;
        compacted[start].num_primitives = 0; // This is what signifies this is a branching node.
        compacted[start].second_child_offset = BVH_compactify_recur(compacted, node->children[0], start + 1) + 1;
        // printf("Branching second\n");
        return BVH_compactify_recur(compacted, node->children[1], compacted[start].second_child_offset);
    }
}
static void BVH_compactify(Node *root, vector<BVHNode> &compacted)
{
    BVH_compactify_recur(compacted, root, 0);
}

BVH::BVH(const vector<Primitive *> &_primitives, bool keep_root)
{
    // Compute a more compact, homogeneous array of primitive information.
    vector<PrimitiveInfo> p_infos;
    p_infos.reserve(_primitives.size());
    for (int i = 0; i < _primitives.size(); i++) {
        p_infos.push_back(PrimitiveInfo(_primitives[i]));
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

    int tree_size = 0;
    Node *root = BVH_create_node(p_infos, 0, _primitives.size(), &tree_size);
    
    // print_node(root);
    // A side-effect of BVH_create_node is that the PrimitiveInfo array in the order of a depth first traversal
    // from the root node (with arbitrary order for primitives at the leaves).
    //
    // Convert this into an array of primitive pointers, with the exact same order.
    primitives = vector<Primitive *>();
    primitives.reserve(p_infos.size());
    for (const PrimitiveInfo &info : p_infos) {
        primitives.push_back(info.primitive);
    }
    #if NO_COMPACTIFY
    uncompacted_p_infos = p_infos;
    m_box = root->box;
    uncompacted_root = root;
    #else
    compacted = vector<BVHNode>(tree_size);
    BVH_compactify(root, compacted);
    m_box = root->box;
    if (keep_root) {
        // special case use: BVHs are processed into another form to be more optimal for triangle meshes.
        // The tree representation is more convenient to work on.
        uncompacted_root = root;
    } else {
        BVH_delete_node(root);
        uncompacted_root = NULL;
    }
    // print_compacted(compacted);
    // getchar();
    #endif
}

BoundingBox BVH::world_bound() const
{
    return m_box;
}


#if NO_COMPACTIFY
// Inefficient implementations that just traverse the data structure created while the BVH was being built.

static void recursive_intersect(const BVH *bvh, Ray &ray, Node *node, LocalGeometry *info, bool *any)
{
    bool is_leaf = node->children[0] == NULL;
    if (is_leaf) {
        for (int i = node->first_primitive; i < node->first_primitive+node->num_primitives; i++) {
            if (bvh->uncompacted_p_infos[i].primitive->intersect(ray, info)) *any = true;
        }
    } else {
        if (!node->box.intersect(ray)) return;
        recursive_intersect(bvh, ray, node->children[0], info, any);
        recursive_intersect(bvh, ray, node->children[1], info, any);
    }
}
bool BVH::intersect(Ray &ray, LocalGeometry *info) const
{
    bool any = false;
    LocalGeometry info_p;
    recursive_intersect(this, ray, uncompacted_root, &info_p, &any);

    if (any) {
        *info = info_p;
        return true;
    } else {
        return false;
    }
}
bool BVH::does_intersect(Ray &ray) const
{
    LocalGeometry geom;
    return intersect(ray, &geom);
}
#else
// Hopefully more efficient methods that traverse a compacted data structure, with optimizations
// such as precomputations for ray-bounding box intersections.


static inline bool intersect_box(const BVHNode &node, const Ray &ray, const Vector &inv_d, const int is_negative[3])
{
    // Optimized function used for box tests in the BVH.
    // Check box intersection.
    float t0x, t1x, t0y, t1y, t0z, t1z;
    // X
    t0x = (node.box.corners[is_negative[0]].x - ray.o.x) * inv_d.x;
    t1x = (node.box.corners[1-is_negative[0]].x - ray.o.x) * inv_d.x;
    // Y
    t0y = (node.box.corners[is_negative[1]].y - ray.o.y) * inv_d.y;
    t1y = (node.box.corners[1-is_negative[1]].y - ray.o.y) * inv_d.y;
    if (t0x > t1y || t0y > t1x) {
        // No intersection, the X and Y interval intersection is degenerate.
        return false;
    }
    // Take the intersection of the interval. (t0x,t1x are now used as the whole box interval).
    if (t0y > t0x) t0x = t0y;
    if (t1y < t1x) t1x = t1y;
    // Z
    t0z = (node.box.corners[is_negative[2]].z - ray.o.z) * inv_d.z;
    t1z = (node.box.corners[1-is_negative[2]].z - ray.o.z) * inv_d.z;
    if (t0x > t1z || t0z > t1x) {
        // No intersection, the box interval is degenerate.
        return false;
    }
    // Take the intersection again. Now t0x,t1x is the interval of the ray's line intersecting the box.
    if (t0z > t0x) t0x = t0z;
    if (t1z < t1x) t1x = t1z;
    // Check if the intersection with the ray segment is degenerate.
    if (t0x > ray.max_t || t1x < ray.min_t) {
        // If either of these is true then the intersection must be degenerate. The converse is also true.
        return false;
    }
    return true;
}


bool BVH::intersect(Ray &ray, LocalGeometry *out_geom) const
{
    // Precomputations
    Vector inv_d(1.f / ray.d.x, 1.f / ray.d.y, 1.f / ray.d.z);
    Point origin = ray(ray.min_t);
    int is_negative[3] = { ray.d.x < 0, ray.d.y < 0, ray.d.z < 0 };

    bool any_intersection = false;
    uint32_t todo[128];
    int todo_now = 0;
    todo[0] = 0;
    int index = 0;
    do {
        if (intersect_box(compacted[index], ray, inv_d, is_negative)) {
            if (compacted[index].num_primitives == 0) {
                // Branching node.
                todo_now ++;
                todo[todo_now] = compacted[index].second_child_offset;
                index ++;
            } else {
                // Leaf node.
                int n = compacted[index].primitives_offset+compacted[index].num_primitives;
                for (int i = compacted[index].primitives_offset;
                         i < n;
                         i++) {
                    if (primitives[i]->intersect(ray, out_geom)) any_intersection = true;
                }
                index = todo[todo_now--];
            }
        } else {
            index = todo[todo_now--];
        }
    } while (todo_now >= 0);
    return any_intersection;
}
bool BVH::does_intersect(Ray &ray) const
{
    // Precomputations
    Vector inv_d(1.f / ray.d.x, 1.f / ray.d.y, 1.f / ray.d.z);
    Point origin = ray(ray.min_t);
    int is_negative[3] = { ray.d.x < 0, ray.d.y < 0, ray.d.z < 0 };

    uint32_t todo[128];
    int todo_now = 0;
    todo[0] = 0;
    int index = 0;
    do {
        if (intersect_box(compacted[index], ray, inv_d, is_negative)) {
            if (compacted[index].num_primitives == 0) {
                // Branching node.
                todo_now ++;
                todo[todo_now] = compacted[index].second_child_offset;
                index ++;
            } else {
                // Leaf node.
                int n = compacted[index].primitives_offset+compacted[index].num_primitives;
                for (int i = compacted[index].primitives_offset;
                         i < n;
                         i++) {
                    if (primitives[i]->does_intersect(ray)) return true;
                }
                index = todo[todo_now--];
            }
        } else {
            index = todo[todo_now--];
        }
    } while (todo_now >= 0);
    return false;
}
#endif // NO_COMPACTIFY
