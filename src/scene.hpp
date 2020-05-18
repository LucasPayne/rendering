#ifndef SCENE_H
#define SCENE_H
#include <vector>
#include "primitives.hpp"

class Scene {
private:
    // Primitive root;
public:

    std::vector<Primitive *> primitives;

    Scene() {}
    Scene(int num_primitives) {
        primitives = std::vector<Primitive *>(num_primitives);
    }
    void add_primitive(Primitive *prim) {
        primitives.push_back(prim);
    }
};

#endif // SCENE_H
