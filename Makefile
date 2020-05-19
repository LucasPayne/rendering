#--------------------------------------------------------------------------------
#
# note:
#     Possibly dependencies, such as for headers, are missing.
#     It would be good to learn to use cmake.
#
#--------------------------------------------------------------------------------

CC=g++ -g -Isrc -Ilibraries
CFLAGS=-lm -lglfw3 -lGL -lX11 -ldl -lpthread -lrt

build/rays.o: src/main.cpp build/libraries/glad.o build/core.o
	$(CC) -c src/main.cpp -o build/main.o $(CFLAGS)
	ld -relocatable -o $@ build/main.o build/libraries/glad.o build/core.o

clean:
	rm build/**/*.o

build/libraries/glad.o: libraries/glad/glad.c
	$(CC) -c $< -o $@ $(CFLAGS)

build/core.o: build/mathematics.o build/primitives.o build/illumination.o build/imaging.o build/scene.o build/renderer.o build/models.o
	ld -relocatable -o $@ $^

build/mathematics.o: build/mathematics/geometry.o build/mathematics/transform.o
	ld -relocatable -o $@ $^
build/mathematics/geometry.o: src/mathematics/geometry.cpp src/mathematics/geometry.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
build/mathematics/transform.o: src/mathematics/transform.cpp src/mathematics/transform.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

build/primitives.o: build/primitives/primitives.o build/primitives/sphere.o build/primitives/triangle_mesh.o
	ld -relocatable -o $@ $^
build/primitives/primitives.o: src/primitives/primitives.cpp src/primitives.hpp src/mathematics.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
build/primitives/sphere.o: src/primitives/sphere.cpp src/primitives/sphere.hpp src/primitives.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
build/primitives/triangle_mesh.o: src/primitives/triangle_mesh.cpp src/primitives/triangle_mesh.hpp src/primitives.hpp src/models.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

build/imaging.o: build/imaging/camera.o build/imaging/framebuffer.o
	ld -relocatable -o $@ $^
build/imaging/camera.o: src/imaging/camera.cpp src/imaging/camera.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
build/imaging/framebuffer.o: src/imaging/framebuffer.cpp src/imaging/framebuffer.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

build/illumination.o: build/illumination/color.o build/illumination/light.o build/illumination/point_light.o
	ld -relocatable -o $@ $^
build/illumination/color.o: src/illumination/color.cpp src/illumination/color.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
build/illumination/light.o: src/illumination/light.cpp src/illumination/light.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
build/illumination/point_light.o: src/illumination/point_light.cpp src/illumination/point_light.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

build/scene.o: src/scene/scene.cpp src/scene.hpp src/mathematics.hpp src/primitives.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

build/renderer.o: src/renderer/renderer.cpp src/renderer.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

build/models.o: src/models/models.cpp src/models.hpp src/primitives.hpp src/illumination/color.hpp src/mathematics.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
