#ifndef CORE_H
#define CORE_H

#include "illumination.hpp"
#include "mathematics.hpp"
#include "primitives.hpp"
#include "imaging.hpp"
#include "scene.hpp"
#include "renderer.hpp"

// Library includes.
#include <glm/glm.hpp>
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4x4 mat4x4;

#define frand() ((1.0 / (RAND_MAX + 1.0)) * rand())
#define frand_interval(A,B) (( A ) + (( B ) - ( A ))*frand())

#endif // CORE_H
