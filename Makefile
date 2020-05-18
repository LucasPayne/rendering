
CC=g++ -g -Iinclude -Ilibraries
CFLAGS=-lm -lglfw3 -lGL -lX11 -ldl -lpthread -lrt

rays: main.cpp build/glad.o
	$(CC) main.cpp build/glad.o -o rays $(CFLAGS)
rays_run: rays
	./rays

build/glad.o: src/glad/glad.c
	$(CC) -c $< -o $@ $(CFLAGS)
