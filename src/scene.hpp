#ifndef SCENE_H
#define SCENE_H
#include <vector>
#include "primitives.hpp"
#include "illumination.hpp"

class Scene {
private:
    // Primitive root;
public:
    std::vector<Primitive *> primitives;
    std::vector<Light *> lights;

    Scene() {
        primitives = std::vector<Primitive *>(0);
        lights = std::vector<Light *>(0);
    }
    inline void add_primitive(Primitive *prim) {
        primitives.push_back(prim);
    }
    inline void add_light(Light *light) {
        lights.push_back(light);
    }

    bool hit_by_ray(Ray ray);
};


#endif // SCENE_H
