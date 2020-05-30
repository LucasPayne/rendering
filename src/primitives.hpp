#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include "core.hpp"
#include "shapes.hpp"
// #include "materials.hpp"

/*--------------------------------------------------------------------------------
A Primitive is more general than just an object in the scene.
They are what is hit by rays, but that can include ideas like
    "two icosahedra can be hit by a ray",
and that means the primitive "two icosahedra" can pass intersection routines
down to the two icosahedra it contains, which are themselves primitives.

This idea is used to unify atomic shapes, self-refining shapes and recursively-contained
acceleration structures. Each traced ray just "intersects" a root primitive, the scene.

This is heavily based off of the pbrt v2 implementation of the ideas in James Arvo and David Kirk's
paper, "The Ray Tracing Kernel".
--------------------------------------------------------------------------------*/
class Primitive {
public:    
    Primitive() {}

    // Pure virtual functions, must be implemented by derived classes.
    virtual BoundingBox world_bound() const = 0;

    // These are pure virtual so that derived classes can, for example, redefine can_intersect()
    // to return false, and give an error on these so that further derived classes don't have to implement it themselves.
    virtual bool intersect(Ray &ray, LocalGeometry *info) const = 0;
    virtual bool does_intersect(Ray &ray) const = 0;

    // Overridable functions.
    virtual bool can_intersect() const { return true; }
private:    
};

/*--------------------------------------------------------------------------------
    A GeometricPrimitive encapsulates a single shape. In this way, the shape
    can be associated more things that are needed for it to be rendered, and
    the Shape class can purely contain geometric things.
--------------------------------------------------------------------------------*/
class GeometricPrimitive : public Primitive {
public:
    GeometricPrimitive() {}
    GeometricPrimitive(const Shape *_shape) :
        shape(_shape)
    {}

    // Pass on some routines to the underlying shape.
    virtual bool can_intersect() const { 
        return shape->can_intersect();
    }
    virtual bool intersect(Ray &ray, LocalGeometry *geom) const {
        return shape->intersect(ray, geom);
    }
    virtual bool does_intersect(Ray &ray) const {
        return shape->does_intersect(ray);
    }
    virtual BoundingBox world_bound() const {
        return shape->world_bound();
    };
private:
    const Shape *shape;
    // const Material *material;
};

class Aggregate : public Primitive {
public:
    Aggregate() {}
    // If there is anything that doesn't make sense for aggregates available in the Primitive interface,
    // override it to give an error here.
private:
};

/*
// Primitive Declarations
class Primitive : public ReferenceCounted {
public:
    // Primitive Interface
    Primitive() : primitiveId(nextprimitiveId++) { }
    virtual ~Primitive();
    virtual BBox WorldBound() const = 0;
    virtual bool CanIntersect() const;
    virtual bool Intersect(const Ray &r, Intersection *in) const = 0;
    virtual bool IntersectP(const Ray &r) const = 0;
    virtual void Refine(vector<Reference<Primitive> > &refined) const;
    void FullyRefine(vector<Reference<Primitive> > &refined) const;
    virtual const AreaLight *GetAreaLight() const = 0;
    virtual BSDF *GetBSDF(const DifferentialGeometry &dg,
        const Transform &ObjectToWorld, MemoryArena &arena) const = 0;
    virtual BSSRDF *GetBSSRDF(const DifferentialGeometry &dg,
        const Transform &ObjectToWorld, MemoryArena &arena) const = 0;

    // Primitive Public Data
    const uint32_t primitiveId;
protected:
    // Primitive Protected Data
    static uint32_t nextprimitiveId;
}
*/


/*
// GeometricPrimitive Declarations
class GeometricPrimitive : public Primitive {
public:
    // GeometricPrimitive Public Methods
    bool CanIntersect() const;
    void Refine(vector<Reference<Primitive> > &refined) const;
    virtual BBox WorldBound() const;
    virtual bool Intersect(const Ray &r, Intersection *isect) const;
    virtual bool IntersectP(const Ray &r) const;
    GeometricPrimitive(const Reference<Shape> &s,
                       const Reference<Material> &m, AreaLight *a);
    const AreaLight *GetAreaLight() const;
    BSDF *GetBSDF(const DifferentialGeometry &dg,
                  const Transform &ObjectToWorld, MemoryArena &arena) const;
    BSSRDF *GetBSSRDF(const DifferentialGeometry &dg,
                      const Transform &ObjectToWorld, MemoryArena &arena) const;
private:
    // GeometricPrimitive Private Data
    Reference<Shape> shape;
    Reference<Material> material;
    AreaLight *areaLight;
};
*/

#endif // PRIMITIVES_H
