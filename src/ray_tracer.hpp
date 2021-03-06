#ifndef RAY_TRACER_H
#define RAY_TRACER_H
// Files should include this if they want access to everything in the core of the ray tracer.
// This will probably be scene-describing source files and main programs such as viewers.
//
// No OpenGL stuff is here. Programs using OpenGL should include gl.hpp as well.

#include "core.hpp"

#include "mathematics.hpp"
#include "illumination.hpp"
#include "imaging.hpp"
#include "primitives.hpp"
#include "shapes.hpp"
#include "scene.hpp"
#include "renderer.hpp"
#include "models.hpp"

// Include object libraries.
#include "aggregates.hpp"
#include "shapes_library.hpp"

#include "interaction.hpp"
#include <thread>
#include "multithreading.hpp"

#endif // RAY_TRACER_H
