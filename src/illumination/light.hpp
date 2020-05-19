#ifndef ILLUMINATION_LIGHT_H
#define ILLUMINATION_LIGHT_H
#include "illumination/color.hpp"
#include "primitives.hpp"

class Scene;

// A VisibilityTester is a closure encapsulating what should be done to test light occlusion, if that turns out to be needed.
// This is given when radiance incoming to a point is calculated, and encapsulates the test needed to determine if the light actually
// did get there, giving that radiance.
// -
// Idea from pbr p608.
// There is different logic for visibility tests to be done for point and directional lights.
// Point light occlusion should be done in a segment. The ray min_t and max_t values are used here.
struct VisibilityTester {
    // The data is just a ray. The VisibilityTester is basically a wrapper around a ray, with some extra methods.
    Ray ray;

    // Set occlusion test to be over a segment, e.g. for point lights.
    void SetSegment(const Point &p1, const Point &p2) {
        float length = glm::length(p2 - p1);
        ray = Ray(p1, (p2 - p1) / length);
        const float error_shift = 1e-5;
        ray.min_t = error_shift;
        ray.max_t = length;
    }
    // Set occlusion test to be over a ray, e.g. for directional lights.
    void SetRay(const Point &origin, const Vector &direction) {
        const float error_shift = 1e-5;
        ray = Ray(origin, direction);
        ray.min_t = error_shift;
    }
    bool unoccluded(Scene &scene) const;
};

/*
pbrt2 source
struct VisibilityTester {
    // VisibilityTester Public Methods
    void SetSegment(const Point &p1, float eps1,
                    const Point &p2, float eps2, float time) {
        float dist = Distance(p1, p2);
        r = Ray(p1, (p2-p1) / dist, eps1, dist * (1.f - eps2), time);
        Assert(!r.HasNaNs());
    }
    void SetRay(const Point &p, float eps, const Vector &w, float time) {
        r = Ray(p, w, eps, INFINITY, time);
        Assert(!r.HasNaNs());
    }
    bool Unoccluded(const Scene *scene) const;
    Spectrum Transmittance(const Scene *scene, const Renderer *renderer,
        const Sample *sample, RNG &rng, MemoryArena &arena) const;
    Ray r;
};
*/

class Light {
private:
public:
    Transform light_to_world, world_to_light;
    Light(const Transform &l2w) {
        light_to_world = l2w;
        world_to_light = l2w.inverse();
    };
    // Create a ray from an intersection point directed toward the light source.
    // This assumes that these are not area lights. Or, maybe, area lights could give a random light ray from its projected surface.
    virtual Ray light_ray(const LocalGeometry &geom) = 0;

    virtual RGB radiance(const Point &p, Vector *light_vector, VisibilityTester *visibility_tester) = 0;
    // notes on radiance:
    //--------------------------------------------------------------------------------
    // Measure the radiance at a point with this light as a source. This will probably done at a surface, but
    // is in general the radiance (so no cosine falloff).
    // - Is this actually radiance? Is radiance associated to a surface, or rather a point in space?
    // --- a surface.

    // wikipedia:
    // In radiometry, radiance is the radiant flux emitted, reflected, transmitted or
    // received by a given surface, per unit solid angle per unit projected
    // area. Spectral radiance is the radiance of a surface per unit frequency
    // or wavelength, depending on whether the spectrum is taken as a function
    // of frequency or of wavelength. These are directional quantities

    // Radiance: Flux density per unit area, per unit solid angle.
    // Radiant flux: also known as power, is the total amount of energy passing through
    //    a surface or region of space per unit time. (J/s :: watts)
    //        pbr p282
    // So what is a measure of the ~differential radiant flux~ at a point in space?

    // It might make more sense that this quantity (for point lights, intensity/r^2) be called flux.
    // Could this be thought of as the radiance on a surface whose normal is the direction the light came from?
    //--------------------------------------------------------------------------------

};

#endif // ILLUMINATION_LIGHT_H
