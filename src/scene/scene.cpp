#include <vector>
#include "scene.hpp"
#include "mathematics.hpp"
#include "illumination.hpp"
#include "primitives.hpp"
#include "aggregates.hpp"

// Just pass down these routines to whatever aggregate primitive holds the primitives.
BoundingBox Scene::world_bound() const
{
    return primitives.world_bound();
}
bool Scene::intersect(Ray &ray, LocalGeometry *geom) const
{
    return primitives.intersect(ray, geom);
}
bool Scene::does_intersect(Ray &ray) const
{
    return primitives.does_intersect(ray);
}

void Scene::add_primitive(Primitive *prim)
{
    primitives.add(prim);
}
void Scene::add_light(Light *light)
{
    lights.push_back(light);
}
