#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include "mathematics.hpp"
#include <vector>

class Primitive {
public:    
    Primitive() {}

    virtual BoundingBox world_bound() const = 0;

    virtual bool can_intersect() const = 0;
    virtual bool intersect(Ray &ray, LocalGeometry *info) = 0;
    // does_intersect() is just a predicate, so can probably be made more efficient than intersect().
    virtual bool does_intersect(Ray &ray) = 0;
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


class GeometricPrimitive : Primitive {
public:
    virtual bool can_intersect() const;
private:
};

bool GeometricPrimitive::can_intersect() const {
    return true;
}

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

class Aggregate : public Primitive {

};

// Routine to intersect with a vector of primitives. This acts as a basic list, whose members are exhaustively searched.
// -
// This should be a type of primitive instead of requiring a separate routine.
bool intersect_primitive_vector(Ray &ray, std::vector<Primitive *> &primitives, LocalGeometry *geom);

#endif // PRIMITIVES_H
