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
    
    // model = _model;
    model = _model->copy();
    std::cout << "Copied\n";
    model->transform_by(o2w);
    std::cout << "Transformed\n";

    // Set up to use the usual code to create a BVH (don't want to duplicate that here).
    triangles = vector<MeshTriangle>(model->num_triangles);
    geometric_triangles = vector<GeometricPrimitive>(model->num_triangles);
    triangle_pointers = vector<Primitive *>(model->num_triangles);
    for (int i = 0; i < model->num_triangles; i++) {
        triangles[i] = MeshTriangle(this, model->triangles[3*i], model->triangles[3*i+1], model->triangles[3*i+2]);
        geometric_triangles[i] = GeometricPrimitive(&triangles[i]);
        triangle_pointers[i] = &geometric_triangles[i];
    }
    // Construct a usual bounding volume heirarchy around the mesh triangles.
    BVH bvh = BVH(triangle_pointers);
    // Now, make a specific structure for triangles.
}
