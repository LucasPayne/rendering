#include "mathematics.hpp"
#include "primitives.hpp"
#include "light.hpp"
#include "scene.hpp"

bool VisibilityTester::unoccluded(Scene *scene) const {
    return !scene->hit_by_ray(ray);
}
