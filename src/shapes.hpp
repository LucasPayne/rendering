#ifndef SHAPE_H
#define SHAPE_H
#include <vector>
#include "mathematics.hpp"

// Local "differential geometry" of a shape, given by Shape::intersect().
class LocalGeometry;

class Shape {
public:
    Shape (const Transform &o2w) :
        object_to_world{o2w}, world_to_object{o2w.inverse()}
    {}
    
    // Shape interface
    // Pure virtual functions: Must be implemented by derived classes.
    // --------------------------------------------------------------------------------
    virtual BoundingBox object_bound() const = 0;

    // Virtual functions: Can be overridden by derived classes.
    // To avoid the neccessity of including an empty/error-giving implementation for pure
    // virtual functions, some are made non-pure and have a default error-giving implementation
    // (such as intersect(), since some for some shapes this does not make sense).
    // --------------------------------------------------------------------------------
    // By default this bounds the transformed object-space bounding box.
    // This may be much worse than is possible, but is still a conservative bounding box.
    virtual BoundingBox world_bound() const;
    // These are the same methods as in the Primitive class. Most likely a geometric primitive
    // will pass calls to their intersection functions to a shape.
    virtual bool can_intersect() const { return true; }; // Derived shapes which are self-refining can override this.
    virtual bool intersect(Ray &ray, LocalGeometry *geom); // Defaults to error.
    virtual bool does_intersect(Ray &ray); // Defaults to calling intersect() and ignoring everything except whether it intersects.

    // Shape refinement is primarily for triangle meshes and things tessellated into triangles.
    // virtual void refine(vector<Reference<Shape> > &refined) const;

    // Public methods.
    void set_transform(const Transform &transform);
    // Public data
    Transform object_to_world, world_to_object;
};

/*
// Shape Declarations
class Shape : public ReferenceCounted {
public:
    // Shape Interface
    Shape(const Transform *o2w, const Transform *w2o, bool ro);
    virtual ~Shape();
    virtual BBox ObjectBound() const = 0;
    virtual BBox WorldBound() const;
    virtual bool CanIntersect() const;
    virtual void Refine(vector<Reference<Shape> > &refined) const;
    virtual bool Intersect(const Ray &ray, float *tHit,
                           float *rayEpsilon, DifferentialGeometry *dg) const;
    virtual bool IntersectP(const Ray &ray) const;
    virtual void GetShadingGeometry(const Transform &obj2world,
            const DifferentialGeometry &dg,
            DifferentialGeometry *dgShading) const {
        *dgShading = dg;
    }
    virtual float Area() const;
    virtual Point Sample(float u1, float u2, Normal *Ns) const {
        Severe("Unimplemented Shape::Sample() method called");
        return Point();
    }
    virtual float Pdf(const Point &Pshape) const {
        return 1.f / Area();
    }
    virtual Point Sample(const Point &P, float u1, float u2,
                         Normal *Ns) const {
        return Sample(u1, u2, Ns);
    }
    virtual float Pdf(const Point &p, const Vector &wi) const;

    // Shape Public Data
    const Transform *ObjectToWorld, *WorldToObject;
    const bool ReverseOrientation, TransformSwapsHandedness;
    const uint32_t shapeId;
    static uint32_t nextshapeId;
};
*/

struct LocalGeometry {
    LocalGeometry() {}
    LocalGeometry(Shape &_shape, Point _p, Vector _n) {
        shape = &_shape;
        p = _p;
        n = _n;
    }

    Shape *shape;
    Point p;  // Point on the shape.
    Vector n; // Normal outward from the surface element.
};

#include "shapes/sphere.hpp"
#include "shapes/triangle_mesh.hpp"
#include "shapes/quadric.hpp"
#include "shapes/plane.hpp"


#endif // SHAPE_H
