#ifndef PRIMITIVES_AGGREGATES_PRIMITIVELIST_H
#define PRIMITIVES_AGGREGATES_PRIMITIVELIST_H
#include "primitives.hpp"

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
        m_world_bound = BoundingBox();
    }
    PrimitiveList(std::vector<Primitive *> _primitives) {
        primitives = _primitives;
        m_world_bound = BoundingBox();
    }
    Primitive *add(Primitive *primitive);
    int length() const { return primitives.size(); }

    // Aggregate-Primitive interface implementations.
    BoundingBox world_bound() const;
    bool intersect(Ray &ray, Intersection *inter);
    bool does_intersect(Ray &ray) const;
    // bool can_intersect() const { return true; }
private:
    std::vector<Primitive *> primitives;
    BoundingBox m_world_bound;
};

#endif // PRIMITIVES_AGGREGATES_PRIMITIVELIST_H
