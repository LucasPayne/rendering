#include "mathematics/geometry.hpp"

/*--------------------------------------------------------------------------------
    Vector
--------------------------------------------------------------------------------*/
std::ostream &operator<<(std::ostream &os, const Vector &vector)
{
    os << "Vector(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
    return os;
}

/*--------------------------------------------------------------------------------
    Point
--------------------------------------------------------------------------------*/
std::ostream &operator<<(std::ostream &os, const Point &point)
{
    os << "Point(" << point.x << ", " << point.y << ", " << point.z << ")";
    return os;
}

/*--------------------------------------------------------------------------------
    Ray
--------------------------------------------------------------------------------*/
std::ostream &operator<<(std::ostream &os, const Ray &ray)
{
    os << "Ray(" << ray.o << ", " << ray.d << ")";
    return os;
}

/*--------------------------------------------------------------------------------
    glm::mat4x4
--------------------------------------------------------------------------------*/
std::ostream &operator<<(std::ostream &os, const mat4x4 &m)
{
    os << "mat4x4(";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            os << "    " << m[i][j] << (j<3?", ":"\n");
        }
    }
    os << ")\n";
    return os;
}

/*--------------------------------------------------------------------------------
    BoundingBox
--------------------------------------------------------------------------------*/
BoundingBox enlarged(const BoundingBox &box, const BoundingBox &other_box)
{
    BoundingBox new_box = box;
    new_box.enlarge(other_box);
    return new_box;
}
BoundingBox enlarged(const BoundingBox &box, const Point &point)
{
    BoundingBox new_box = box;
    new_box.enlarge(point);
    return new_box;
}

/* pbr 2e, page 194 */
bool BoundingBox::intersect(const Ray &ray, float *out_t0, float *out_t1) const
{
    float t0 = ray.min_t;
    float t1 = ray.max_t;
    float inv_d, new_t0, new_t1;

    #define SWAP(X,Y) {\
        float temp = ( X );\
        ( X ) = ( Y );\
        ( Y ) = temp;\
    }

    //-----This should really be a loop, need to change the Point class a bit.
    inv_d = 1.0 / ray.d.x; // IEEE floating point gives infinite values which should work here.
    new_t0 = inv_d * (corners[0].x - ray.o.x);
    new_t1 = inv_d * (corners[1].x - ray.o.x);
    // Order the new range.
    if (new_t1 < new_t0) SWAP(new_t0, new_t1);
    // Take the intersection of the ranges.
    t0 = new_t0 > t0 ? new_t0 : t0;
    t1 = new_t1 < t1 ? new_t1 : t1;
    if (t1 < t0) return false;

    // Y slabs
    inv_d = 1.0 / ray.d.y;
    new_t0 = inv_d * (corners[0].y - ray.o.y);
    new_t1 = inv_d * (corners[1].y - ray.o.y);
    if (new_t1 < new_t0) SWAP(new_t0, new_t1);
    t0 = new_t0 > t0 ? new_t0 : t0;
    t1 = new_t1 < t1 ? new_t1 : t1;
    if (t1 < t0) return false;

    // Z slabs
    inv_d = 1.0 / ray.d.z;
    new_t0 = inv_d * (corners[0].z - ray.o.z);
    new_t1 = inv_d * (corners[1].z - ray.o.z);
    if (new_t1 < new_t0) SWAP(new_t0, new_t1);
    t0 = new_t0 > t0 ? new_t0 : t0;
    t1 = new_t1 < t1 ? new_t1 : t1;
    if (t1 < t0) return false;

    *out_t0 = t0;
    *out_t1 = t1;
    return true;
    #undef SWAP
}
bool BoundingBox::intersect(const Ray &ray) const
{
    float t0, t1;
    return intersect(ray, &t0, &t1);
}

std::ostream &operator<<(std::ostream &os, const BoundingBox &box)
{
    os << "BoundingBox(" << box.corners[0] << ", " << box.corners[1] << ")";
    return os;
}
