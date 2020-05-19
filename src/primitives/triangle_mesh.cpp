#include "primitives.hpp"
#include "models.hpp"
#include <string>
#include <iostream>
#include <glm/glm.hpp>

bool TriangleMesh::intersect(Ray &world_ray, LocalGeometry *geom)
{
    Ray object_ray = world_to_object(world_ray);
    Point &o = object_ray.o;
    Vector &d = object_ray.d;

    int triangle_index = -1;
    Vector triangle_n;
    for (int i = 0; i < model.num_triangles; i++) {
        Point &a = model.vertices[model.triangles[3*i + 0]];
        Point &b = model.vertices[model.triangles[3*i + 1]];
        Point &c = model.vertices[model.triangles[3*i + 2]];
        
        Vector n = glm::cross(c - a, b - a);
        float denom = glm::dot(d, n);
        const float epsilon = 1e-5;
        if (denom < 0 && denom > -epsilon) {
            denom = -epsilon;
        } else if (denom >= 0 && denom < epsilon) {
            denom = epsilon;
        }
        float t = glm::dot(a - o, n) / denom;
        if (t < world_ray.min_t || t > world_ray.max_t) continue;

        Point p = o + d * t;
        if (glm::dot(p - a, glm::cross(b - a, glm::cross(c - a, b - a))) < 0)
            continue;
        if (glm::dot(p - b, glm::cross(c - b, glm::cross(a - b, c - b))) < 0)
            continue;
        if (glm::dot(p - c, glm::cross(a - c, glm::cross(b - c, a - c))) < 0)
            continue;
        

        triangle_index = i;
        triangle_n = n;
        world_ray.max_t = t;
    }
    if (triangle_index < 0) return false;

    geom->primitive = this;
    geom->p = world_ray.o + world_ray.d * world_ray.max_t;
    geom->n = triangle_n;
    
    return true;
}
