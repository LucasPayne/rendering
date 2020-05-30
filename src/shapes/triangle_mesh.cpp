#include "shapes/triangle_mesh.hpp"

Point MeshTriangle::operator[](int index) const
{
    return mesh->model->vertices[indices[index]];
}
Point MeshTriangle::a() const {
    return (*this)[0];
}
Point MeshTriangle::b() const {
    return (*this)[1];
}
Point MeshTriangle::c() const {
    return (*this)[2];
}

bool MeshTriangle::intersect(Ray &ray, LocalGeometry *geom) const
{
/*
    // Following pbrt, page 140.
    // Parameterizing the triangle in barycentric coordinates,
    // form the equation
    //    o + td = a(1-b1-b2) + b(b1) + c(b2).
    // ~deaffine~ this by translating a to the origin,
    // giving (o - a) + td = (b - a)b1 + (c - a)b2.
    // This is a 3x3 linear system with three unknowns, so is readily solved with
    // Cramer's rule.
    Vector e1 = b() - a();
    Vector e2 = c() - a();
    Vector s1 = glm::cross(ray.d, e2);
    Vector s2 = glm::cross(ray.d, e1);
    float coeff = 1.0 / glm::dot(s1, e1);
    float t = coeff * glm::dot(s2, e2);
    float b1 = coeff * glm::dot(s2, ray.o - a());
    float b2 = coeff * glm::dot(s2, ray.d);
    if (b1 < 0 || b2 < 0 || b1 + b2 > 1) return false; // does not intersect the triangle.
    Point p = ray(t);
    ray.max_t = t;
    geom->n = glm::cross(b()-a(), c()-a());
    geom->p = p;
    geom->shape = this;
    return true;
*/

    Vector n = glm::cross(c()-a(), b()-a());
    float denom = glm::dot(ray.d, n);
    const float epsilon = 1e-4;
    if (fabs(denom) < epsilon) {
        // The ray is almost parallel to the triangle.
        return false;
    }
    float t = -glm::dot(ray.o - a(), n)/denom;
    if (t < ray.min_t || t > ray.max_t) return false;
    Point p = ray(t);

    float wa = glm::dot(ray.d, glm::cross(b()-ray.o, c()-ray.o));
    float wb = glm::dot(ray.d, glm::cross(c()-ray.o, a()-ray.o));
    float wc = glm::dot(ray.d, glm::cross(a()-ray.o, b()-ray.o));
    if (((wa > 0) != (wb > 0)) || ((wb > 0) != (wc > 0))) return false;
    float winv = 1.0 / (wa + wb + wc);
    wa *= winv;
    wb *= winv;
    wc *= winv;

    ray.max_t = t;
    if (mesh->model->has_normals) {
        // Compute a shading normal instead.
        Vector &na = mesh->model->normals[indices[0]];
        Vector &nb = mesh->model->normals[indices[1]];
        Vector &nc = mesh->model->normals[indices[2]];
        // geom->n = glm::normalize(wa*na + wb*nb + wc*nc);
        geom->n = wa*na + wb*nb + wc*nc; // normalizing here seems to be expensive. Will this be good enough?
    } else {
        geom->n = glm::normalize(n);
    }
    geom->p = p;
    geom->shape = this;
    return true;
}

static inline bool barycentric_triangle_convex(float wa, float wb, float wc)
{
    // tests whether the weights are a convex combination of the triangle points.
    return 0 <= wa && wa <= 1 && 0 <= wb && wb <= 1 && 0 <= wc && wc <= 1;
}
bool MeshTriangle::does_intersect(Ray &ray) const
{
    Vector n = glm::cross(c()-a(), b()-a());
    float denom = glm::dot(ray.d, n);
    const float epsilon = 1e-4;
    if (fabs(denom) < epsilon) {
        // The ray is almost parallel to the triangle.
        return false;
    }
    float t = -glm::dot(ray.o - a(), n)/denom;
    if (t < ray.min_t || t > ray.max_t) return false;
    Point p = ray(t);

    float wa = glm::dot(ray.d, glm::cross(b()-ray.o, c()-ray.o));
    float wb = glm::dot(ray.d, glm::cross(c()-ray.o, a()-ray.o));
    float wc = glm::dot(ray.d, glm::cross(a()-ray.o, b()-ray.o));
    return (((wa > 0) == (wb > 0)) && ((wb > 0) == (wc > 0)));
}
BoundingBox MeshTriangle::object_bound() const
{
    return m_box;
}


static void flatten_to_triangles_bvh_recur(const BVH &bvh, vector<TriangleNode> &trinodes,
				           Node *node,
                                           int *trinodes_index)
{
    if (node->is_leaf()) {
        for (int i = 0; i < node->num_primitives; i++) {
            trinodes[*trinodes_index + i].box = node->box;
            trinodes[*trinodes_index + i].next_shift = 0; // signify this is a leaf.
            MeshTriangle *mtri = (MeshTriangle *) (((GeometricPrimitive *) bvh.primitives[node->first_primitive + i])->shape);
            trinodes[*trinodes_index + i].a = mtri->indices[0];
            trinodes[*trinodes_index + i].b = mtri->indices[1];
            trinodes[*trinodes_index + i].c = mtri->indices[2];
        }
        (*trinodes_index) += node->num_primitives;
    } else {
        int this_node_index = *trinodes_index;
        trinodes[this_node_index].box = node->box;
        trinodes[this_node_index].axis = node->axis;
        (*trinodes_index) ++;
        flatten_to_triangles_bvh_recur(bvh, trinodes, node->children[0], trinodes_index);
        int next_shift = *trinodes_index - this_node_index;
        trinodes[this_node_index].next_shift = next_shift;
        flatten_to_triangles_bvh_recur(bvh, trinodes, node->children[1], trinodes_index);
    }
}

static void flatten_to_triangles_bvh(BVH &bvh, vector<TriangleNode> &trinodes)
{
    //note: Remember to initialize trinodes to the right size before passing it. No space will be made here.

    int trinodes_index = 0;
    flatten_to_triangles_bvh_recur(bvh, trinodes, bvh.uncompacted_root, &trinodes_index);
}

static void bvh_unravelled_length_recur(Node *node, int *length)
{
    if (node->is_leaf()) {
        (*length) += node->num_primitives;
    } else {
        (*length) ++;
        bvh_unravelled_length_recur(node->children[0], length);
        bvh_unravelled_length_recur(node->children[1], length);
    }
}


static inline bool intersect_box(const TriangleNode &node, const Ray &ray, const Vector &inv_d, const int is_negative[3])
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



static inline bool triangle_intersect(const TriangleMesh *mesh,
                                      const Point &a, const Point &b, const Point &c,
                                      uint16_t index_a, uint16_t index_b, uint16_t index_c, 
                                      Ray &ray, LocalGeometry *geom)
{
    Vector n = glm::cross(c-a, b-a);
    float denom = glm::dot(ray.d, n);
    const float epsilon = 1e-4;
    if (fabs(denom) < epsilon) {
        // The ray is almost parallel to the triangle.
        return false;
    }
    float t = -glm::dot(ray.o - a, n)/denom;
    if (t < ray.min_t || t > ray.max_t) return false;
    Point p = ray(t);

    float wa = glm::dot(ray.d, glm::cross(b-ray.o, c-ray.o));
    float wb = glm::dot(ray.d, glm::cross(c-ray.o, a-ray.o));
    float wc = glm::dot(ray.d, glm::cross(a-ray.o, b-ray.o));
    if (((wa > 0) != (wb > 0)) || ((wb > 0) != (wc > 0))) return false;
    float winv = 1.0 / (wa + wb + wc);
    wa *= winv;
    wb *= winv;
    wc *= winv;

    ray.max_t = t;
    if (mesh->model->has_normals) {
        // Compute a shading normal instead.
        Vector &na = mesh->model->normals[index_a];
        Vector &nb = mesh->model->normals[index_b];
        Vector &nc = mesh->model->normals[index_c];
        geom->n = wa*na + wb*nb + wc*nc; // normalizing here seems to be expensive. Will this be good enough?
    } else {
        geom->n = n; // no normalize, do that in triangles_bvh_intersect.
    }
    geom->p = p;
    return true;
}

static inline bool triangle_does_intersect(const TriangleMesh *mesh,
                                           const Point &a, const Point &b, const Point &c,
                                           Ray &ray)
{
    Vector n = glm::cross(c-a, b-a);
    float denom = glm::dot(ray.d, n);
    const float epsilon = 1e-4;
    if (fabs(denom) < epsilon) {
        // The ray is almost parallel to the triangle.
        return false;
    }
    float t = -glm::dot(ray.o - a, n)/denom;
    if (t < ray.min_t || t > ray.max_t) return false;
    Point p = ray(t);

    float wa = glm::dot(ray.d, glm::cross(b-ray.o, c-ray.o));
    float wb = glm::dot(ray.d, glm::cross(c-ray.o, a-ray.o));
    float wc = glm::dot(ray.d, glm::cross(a-ray.o, b-ray.o));
    return (((wa > 0) == (wb > 0)) && ((wb > 0) == (wc > 0)));
}
static inline bool triangles_bvh_intersect(const TriangleMesh *mesh, const vector<TriangleNode> &triangles_bvh, Ray &ray, LocalGeometry *geom)
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
        if (intersect_box(triangles_bvh[index], ray, inv_d, is_negative)) {
            if (triangles_bvh[index].next_shift == 0) {
                // Leaf node.
                do {
                    // Intersect with the triangles in this leaf.
                    uint16_t index_a = triangles_bvh[index].a;
                    uint16_t index_b = triangles_bvh[index].b;
                    uint16_t index_c = triangles_bvh[index].c;
                    Point a,b,c;
                    a = mesh->model->vertices[index_a];
                    b = mesh->model->vertices[index_b];
                    c = mesh->model->vertices[index_c];
                    if (triangle_intersect(mesh, a, b, c, index_a, index_b, index_c, ray, geom)) any_intersection = true;
                    index ++;
                    // Either the loop terminates at the end of the array or when a branching node is reached.
                } while (index < mesh->triangles_bvh_length && triangles_bvh[index].next_shift == 0);
                
                index = todo[todo_now--];
            } else {
                if (is_negative[triangles_bvh[index].axis]) {
                    todo_now ++;
                    todo[todo_now] = index + 1;
                    index += triangles_bvh[index].next_shift;
                } else {
                    todo_now ++;
                    todo[todo_now] = index + triangles_bvh[index].next_shift;
                    // Due to the unravelled order, the next node is the first primitive of the first child.
                    index ++;
                }
            }
        } else {
            index = todo[todo_now--];
        }
    } while (todo_now >= 0);
    if (any_intersection) {
        geom->shape = mesh;
        // already should be at least approximately normal if using Phong normals.
        // If not, save some computation (?) by normalizing once the final LocalGeometry is found.
        if (mesh->model->has_normals) geom->n = glm::normalize(geom->n);
    }
    return any_intersection;
}
static inline bool triangles_bvh_does_intersect(const TriangleMesh *mesh, const vector<TriangleNode> &triangles_bvh, Ray &ray)
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
        if (intersect_box(triangles_bvh[index], ray, inv_d, is_negative)) {
            if (triangles_bvh[index].next_shift == 0) {
                // Leaf node.
                do {
                    // Intersect with the triangles in this leaf.
                    Point &a = mesh->model->vertices[triangles_bvh[index].a];
                    Point &b = mesh->model->vertices[triangles_bvh[index].b];
                    Point &c = mesh->model->vertices[triangles_bvh[index].c];
                    if (triangle_does_intersect(mesh, a, b, c, ray)) return true;
                    index ++;
                    // Either the loop terminates at the end of the array or when a branching node is reached.
                } while (index < mesh->triangles_bvh_length && triangles_bvh[index].next_shift == 0);
                
                index = todo[todo_now--];
            } else {
                // Branching node.
                #if 0
                todo_now ++;
                todo[todo_now] = index + triangles_bvh[index].next_shift;
                // Due to the unravelled order, the next node is the first primitive of the first child.
                index ++;
                #else
                if (is_negative[triangles_bvh[index].axis]) {
                    todo_now ++;
                    todo[todo_now] = index + 1;
                    index += triangles_bvh[index].next_shift;
                } else {
                    todo_now ++;
                    todo[todo_now] = index + triangles_bvh[index].next_shift;
                    // Due to the unravelled order, the next node is the first primitive of the first child.
                    index ++;
                }
                #endif
            }
        } else {
            index = todo[todo_now--];
        }
    } while (todo_now >= 0);
    return false;
}

bool TriangleMesh::intersect(Ray &ray, LocalGeometry *geom) const
{
    return triangles_bvh_intersect(this, triangles_bvh, ray, geom);
}
bool TriangleMesh::does_intersect(Ray &ray) const
{
    return triangles_bvh_does_intersect(this, triangles_bvh, ray);
    // //---specialize this.
    // LocalGeometry geom;
    // return intersect(ray, &geom);
}
BoundingBox TriangleMesh::object_bound() const
{
    return m_world_bound;
}

TriangleMesh::TriangleMesh(const Transform &o2w, Model *_model)
{
    // Copy and transform the model into world space.
    // This can save a lot of ray transformations.
    std::cout << "Creating triangle mesh\n";
    
    // model = _model;
    model = _model->copy();
    std::cout << "Copied\n";
    model->transform_by(o2w);
    std::cout << "Transformed\n";

    // Set up to use the usual code to create a BVH (don't want to duplicate that here).
    vector<MeshTriangle> triangles = vector<MeshTriangle>(model->num_triangles);
    vector<GeometricPrimitive> geometric_triangles(model->num_triangles);
    vector<Primitive *> triangle_pointers(model->num_triangles);
    for (int i = 0; i < model->num_triangles; i++) {
        triangles[i] = MeshTriangle(this, model->triangles[3*i], model->triangles[3*i+1], model->triangles[3*i+2]);
        geometric_triangles[i] = GeometricPrimitive(&triangles[i]);
        triangle_pointers[i] = &geometric_triangles[i];
    }
    // Construct a usual bounding volume heirarchy around the mesh triangles.
    BVH bvh = BVH(triangle_pointers, true);
    int unravelled_length = 0;
    bvh_unravelled_length_recur(bvh.uncompacted_root, &unravelled_length);
    triangles_bvh = vector<TriangleNode>(unravelled_length);
    printf("flattened: %d\n", bvh.flattened_length());
    printf("unravelled: %d\n", unravelled_length);
    triangles_bvh_length = unravelled_length;

    printf("Flattening to triangles ...\n");
    flatten_to_triangles_bvh(bvh, triangles_bvh);
    printf("Flattened!\n");
    m_world_bound = bvh.world_bound();
    
    //----Destroy the bvh!
}
