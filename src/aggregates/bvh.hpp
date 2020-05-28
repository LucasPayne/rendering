#ifndef PRIMITIVE_AGGREGATE_BVH_H
#define PRIMITIVE_AGGREGATE_BVH_H
#include "primitives.hpp"

class BVH : public Aggregate {
public:
    BVH(const vector<Primitive *> &primitives);
    
    // Aggregate-Primitive interface implementations.
    BoundingBox world_bound() const;
    bool intersect(Ray &ray, LocalGeometry *info) const;
    bool does_intersect(Ray &ray) const;


private:
    
};

#endif // PRIMITIVE_AGGREGATE_BVH_H
