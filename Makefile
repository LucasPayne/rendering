
CC=g++ -g -Isrc -Ilibraries
CFLAGS=-lm -lglfw3 -lGL -lX11 -ldl -lpthread -lrt

build/rays.o: src/main.cpp build/libraries/glad.o build/core.o
	$(CC) -c src/main.cpp -o build/main.o $(CFLAGS)
	ld -relocatable -o $@ build/main.o build/libraries/glad.o build/core.o

clean:
	rm build/**/*.o

build/libraries/glad.o: libraries/glad/glad.c
	$(CC) -c $< -o $@ $(CFLAGS)

build/core.o: build/mathematics.o build/primitives.o build/illumination.o build/imaging.o build/scene.o build/renderer.o
	ld -relocatable -o $@ $^

build/mathematics.o: build/mathematics/geometry.o build/mathematics/transform.o
	ld -relocatable -o $@ $^
build/mathematics/geometry.o: src/mathematics/geometry.cpp src/mathematics/geometry.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
build/mathematics/transform.o: src/mathematics/transform.cpp src/mathematics/transform.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

build/primitives.o: build/primitives/sphere.o
	ld -relocatable -o $@ $^
build/primitives/sphere.o: src/primitives/sphere.cpp src/primitives/sphere.hpp src/primitives.hpp
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

build/scene.o: src/scene.hpp src/scene/scene.cpp
	$(CC) -c src/scene/scene.cpp -o $@ $(CFLAGS)

build/renderer.o: src/renderer.hpp src/renderer/renderer.cpp
	$(CC) -c src/renderer/renderer.cpp -o $@ $(CFLAGS)
