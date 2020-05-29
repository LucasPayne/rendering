#--------------------------------------------------------------------------------
#
# note:
#     Possibly dependencies, such as for headers, are missing.
#     It would be good to learn to use cmake.
#
#--------------------------------------------------------------------------------

CC=g++ -g -Isrc -Ilibraries
CFLAGS=-lm -lglfw3 -lGL -lX11 -ldl -lpthread -lrt

build/rays.o: src/main.cpp build/libraries/glad.o build/core.o build/gl_core.o
	$(CC) -c src/main.cpp -o build/main.o $(CFLAGS)
	ld -relocatable -o $@ build/main.o build/libraries/glad.o build/core.o build/gl_core.o

clean:
	find build/ -type f -name '*.o' -delete

build/libraries/glad.o: libraries/glad/glad.c
	$(CC) -c $< -o $@ $(CFLAGS)

build/core.o: build/mathematics.o build/primitives.o build/illumination.o build/imaging.o build/scene.o build/renderer.o build/interaction.o build/shapes.o build/aggregates.o build/multithreading.o
	ld -relocatable -o $@ $^

build/mathematics.o: build/mathematics/geometry.o build/mathematics/transform.o build/mathematics/numerics.o
	ld -relocatable -o $@ $^
build/mathematics/geometry.o: src/mathematics/geometry.cpp src/mathematics/geometry.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
build/mathematics/transform.o: src/mathematics/transform.cpp src/mathematics/transform.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
build/mathematics/numerics.o: src/mathematics/numerics.cpp src/mathematics/numerics.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

build/primitives.o: build/primitives/primitives.o
	ld -relocatable -o $@ $^
build/primitives/primitives.o: src/primitives/primitives.cpp src/primitives.hpp src/mathematics.hpp
	$(CC) -c $< -o $@ $(CFLAGS)


build/aggregates.o: build/aggregates/primitive_list.o build/aggregates/bvh.o
	ld -relocatable -o $@ $^
build/aggregates/primitive_list.o: src/aggregates/primitive_list.cpp src/aggregates/primitive_list.hpp src/primitives.hpp src/aggregates.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
build/aggregates/bvh.o: src/aggregates/bvh.cpp src/aggregates/bvh.hpp src/primitives.hpp src/aggregates.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

build/shapes.o: build/shapes/shapes.o build/shapes/sphere.o build/shapes/plane.o
	ld -relocatable -o $@ $^
build/shapes/shapes.o: src/shapes/shapes.cpp src/shapes.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
build/shapes/sphere.o: src/shapes/sphere.cpp src/shapes/sphere.hpp src/shapes.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
build/shapes/plane.o: src/shapes/plane.cpp src/shapes/plane.hpp src/shapes.hpp src/mathematics.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
# build/shapes/triangle_mesh.o: src/shapes/triangle_mesh.cpp src/shapes/triangle_mesh.hpp src/shapes.hpp src/models.hpp
# 	$(CC) -c $< -o $@ $(CFLAGS)
# build/shapes/quadric.o: src/shapes/quadric.cpp src/shapes/quadric.hpp src/shapes.hpp src/mathematics.hpp
# 	$(CC) -c $< -o $@ $(CFLAGS)

build/imaging.o: build/imaging/camera.o build/imaging/framebuffer.o
	ld -relocatable -o $@ $^
build/imaging/camera.o: src/imaging/camera.cpp src/imaging/camera.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
build/imaging/framebuffer.o: src/imaging/framebuffer.cpp src/imaging/framebuffer.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

build/illumination.o: build/illumination/light.o build/illumination/point_light.o
	ld -relocatable -o $@ $^
build/illumination/light.o: src/illumination/light.cpp src/illumination/light.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
build/illumination/point_light.o: src/illumination/point_light.cpp src/illumination/point_light.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

build/scene.o: src/scene/scene.cpp src/scene.hpp src/mathematics.hpp src/primitives.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

build/renderer.o: src/renderer/renderer.cpp src/renderer.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

# build/models.o: src/models/models.cpp src/models.hpp src/primitives.hpp src/illumination/color.hpp src/mathematics.hpp
# 	$(CC) -c $< -o $@ $(CFLAGS)

build/gl_core.o: build/gl.o build/gl/gl_texture.o build/gl/gl_shader_program.o build/gl/gl_input.o
	ld -relocatable -o $@ $^
build/gl.o: src/gl/gl.cpp src/gl.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
build/gl/gl_texture.o: src/gl/gl_texture.cpp src/gl/gl_texture.hpp src/gl.hpp src/imaging/framebuffer.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
build/gl/gl_shader_program.o: src/gl/gl_shader_program.cpp src/gl/gl_shader_program.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
build/gl/gl_input.o: src/gl/gl_input.cpp src/gl/gl_input.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

build/interaction.o: build/interaction/player.o
	ld -relocatable -o $@ $^
build/interaction/player.o: src/interaction/player.cpp src/interaction/player.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

build/multithreading.o: build/multithreading/multithreading.o
	ld -relocatable -o $@ $^
build/multithreading/multithreading.o: src/multithreading/multithreading.cpp src/multithreading.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
