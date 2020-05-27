#ifndef PRIMITIVES_AGGREGATES_PRIMITIVELIST_H
#define PRIMITIVES_AGGREGATES_PRIMITIVELIST_H
#include "primitives.hpp"
#include <vector>

/*--------------------------------------------------------------------------------
    A PrimitiveList just holds a list, and passes calls down to all primitives
    in the list.
    
    This is the root primitive held by the scene. By default, adding to the scene
    adds to this root list.
--------------------------------------------------------------------------------*/
class PrimitiveList : public Aggregate {
public:
    PrimitiveList() {
        primitives = std::vector<Primitive *>(0);
        world_bound = BoundingBox();
    }
    PrimitiveList(std::vector<Primitive *> _primitives) {
        primitives = _primitives;
        world_bound = BoundingBox();
    }
    Primitive *add(Primitive *primitive);
    int length() const { return primitives.size(); }

    // Aggregate-Primitive interface implementations.
    BoundingBox world_bound();
    virtual bool intersect(Ray &ray, LocalGeometry *info);
    virtual bool does_intersect(Ray &ray);
private:
    std::vector<Primitive *> primitives;
};

#endif // PRIMITIVES_AGGREGATES_PRIMITIVELIST_H
