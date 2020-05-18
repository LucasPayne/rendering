
CC=g++ -g -Isrc -Ilibraries
CFLAGS=-lm -lglfw3 -lGL -lX11 -ldl -lpthread -lrt

rays: main.cpp build/glad.o build/core.o src/core.hpp
	$(CC) main.cpp build/glad.o build/core.o -o rays $(CFLAGS)
rays_run: rays
	./rays

build/glad.o: src/glad/glad.c
	$(CC) -c $< -o $@ $(CFLAGS)

build/core.o: build/core/geometry.o build/core/transform.o
	ld -relocatable -o $@ $^
build/core/geometry.o: src/core/geometry.cpp src/core/geometry.hpp
	mkdir -p build/core
	$(CC) -c $< -o $@ $(CFLAGS)
build/core/transform.o: src/core/transform.cpp src/core/transform.hpp
	mkdir -p build/core
	$(CC) -c $< -o $@ $(CFLAGS)
