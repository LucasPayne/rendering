#ifndef SCENE_H
#define SCENE_H
#include <vector>
#include "primitives.hpp"
#include "illumination.hpp"

class Scene {
private:
public:
    PrimitiveList primitives; // The root primitive.
    std::vector<Light *> lights;

    Scene() {
        primitives = Primitive
        primitives = std::vector<Primitive *>(0);
        lights = std::vector<Light *>(0);
    }
    inline void add_primitive(Primitive *prim) {
        primitives.add(prim);
    }
    inline void add_light(Light *light) {
        lights.push_back(light);
    }
};


#endif // SCENE_H
