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
    Vector n = glm::cross(b()-a(), c()-a());
    float denom = glm::dot(ray.d, n);
    const float epsilon = 1e-4;
    if (denom < epsilon) {
        // The ray is almost parallel to the triangle.
        return false;
    }
    float t = -glm::dot(ray.o - a(), n)/denom;
    if (t < ray.min_t || t > ray.max_t) return false;
    Point p = ray(t);

    // Test if the point is on the same side of each edge.
    bool mid_side_test = glm::dot(p-b(),c()-b()) < 0;
    if (   glm::dot(p-a(),b()-a()) < 0 != mid_side_test
        || glm::dot(p-c(),a()-c()) < 0 != mid_side_test) return false;

    geom->n = glm::normalize(n);
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
    float wa = glm::dot(ray.d, glm::cross(b()-ray.o, c()-ray.o));
    float wb = glm::dot(ray.d, glm::cross(c()-ray.o, a()-ray.o));
    float wc = glm::dot(ray.d, glm::cross(a()-ray.o, b()-ray.o));
    float winv = 1.0 / (wa + wb + wc);
    wa *= winv;
    wb *= winv;
    wc *= winv;
    if (!barycentric_triangle_convex(wa,wb,wc)) return false;
    Point p = wa*a() + wb*b() + wc*c();
    float d = glm::dot(p - ray.o, ray.d);
    if (d < ray.min_t*ray.min_t || d > ray.max_t*ray.max_t) return false;
    return true;
}
BoundingBox MeshTriangle::object_bound() const
{
    return m_box;
}


bool TriangleMesh::does_intersect(Ray &ray) const
{
    return bvh->does_intersect(ray);
}
bool TriangleMesh::intersect(Ray &ray, LocalGeometry *geom) const
{
    return bvh->intersect(ray, geom);
}
BoundingBox TriangleMesh::object_bound() const
{
    return bvh->world_bound();
}

TriangleMesh::TriangleMesh(const Transform &o2w, Model *_model)
{
    // Copy and transform the model into world space.
    // This can save a lot of ray transformations.
    std::cout << "Creating triangle mesh\n";
    
    model = _model;
    // model = _model->copy();
    std::cout << "Copied\n";
    model->transform_by(o2w);
    std::cout << "Transformed\n";

    triangles = vector<MeshTriangle>(model->num_triangles);
    geometric_triangles = vector<GeometricPrimitive>(model->num_triangles);
    triangle_pointers = vector<Primitive *>(model->num_triangles);
    
    for (int i = 0; i < model->num_triangles; i++) {
        triangles[i] = MeshTriangle(this, &model->triangles[3*i]);
        geometric_triangles[i] = GeometricPrimitive(&triangles[i]);
        triangle_pointers[i] = &geometric_triangles[i];
    }
    //---could reorder the triangles to the same order as the bvh nodes are packed.

    std::cout << "Constructing BVH for mesh\n";
    bvh = new BVH(triangle_pointers);
    std::cout << "Constructed BVH";
}
