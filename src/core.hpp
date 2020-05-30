#ifndef CORE_H
#define CORE_H
// Every header file in the project should include this.

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <functional>
using std::min;
using std::max;
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4x4 mat4x4;

// Typedefs
typedef vec3 RGB;
typedef vec4 RGBA;
typedef vec3 Vector;

// Global forward declarations.
// These classes form the core of the renderer, so should be available (incomplete) types for all modules,
// as it is expected these classes implementations will be linked.
//
// mathematics/geometry
struct Point; 
struct Ray; 
struct BoundingBox; 
// mathematics/numerics
struct Vec;
struct Mat;
// mathematics/transform
struct Transform;
// shapes
class LocalGeometry;
class Shape;
// primitives
class Primitive;
class GeometricPrimitive;
class Aggregate;
// scene
class Scene;
// imaging/framebuffer
class FrameBuffer;
// imaging/camera
class Camera;
// renderer
struct RenderingState;
class Renderer;

// Utilities.
#define frand() ((1.0 / (RAND_MAX + 1.0)) * rand())
#define frand_interval(A,B) (( A ) + (( B ) - ( A ))*frand())

#endif // CORE_H
