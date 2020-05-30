/*--------------------------------------------------------------------------------
    Plane primitive.
--------------------------------------------------------------------------------*/
#include "shapes/plane.hpp"

Plane::Plane(Point position, Vector x_extent, Vector y_extent, float width, float height)
{
    // Orthonormalize an internal basis for the plane.
    x_extent = glm::normalize(x_extent);
    y_extent -= x_extent * glm::dot(y_extent, x_extent);
    y_extent = glm::normalize(y_extent);
    Vector z_extent = glm::cross(y_extent, x_extent); //--check the sign of this.
    m_x_vector = x_extent;
    m_y_vector = y_extent;
    m_normal = z_extent;

    mat4x4 matrix(
        x_extent.x, x_extent.y, x_extent.z, 0,
        y_extent.x, y_extent.y, y_extent.z, 0,
        z_extent.x, z_extent.y, z_extent.z, 0,
        position.x, position.y, position.z, 1
    );
    set_transform(Transform(matrix));
    m_width = width;
    m_height = height;
}

bool Plane::intersect(Ray &in_ray, LocalGeometry *geom) const
{
    Ray ray = world_to_object(in_ray);
    
    const float epsilon = 1e-5;
    // The ray is almost parallel to the plane, so don't intersect it.
    if (fabs(ray.d.z) < epsilon) return false;

    float t = -ray.o.z / ray.d.z; //---rearrange to avoid divide, at least for the exit-check?
    if (t < ray.min_t || t > ray.max_t) return false; // There is a hit, but it is out of range of the ray segment.
    float x = ray.o.x + ray.d.x * t;
    float y = ray.o.y + ray.d.y * t;
    if (x < -m_width*0.5 || x > m_width*0.5) return false;
    if (y < -m_height*0.5 || y > m_height*0.5) return false;
    
    in_ray.max_t = t;
    geom->p = in_ray.o + in_ray.d * t;
    geom->n = m_normal;
    geom->shape = this;
    return true;
}

BoundingBox Plane::object_bound() const
{
    Point p = Point(0.5 * m_width, 0.5 * m_height, 0); //--check this.
    return BoundingBox(-p, p);
}

