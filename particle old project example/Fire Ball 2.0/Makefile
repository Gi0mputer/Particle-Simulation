# Makefile for compiling and running the C files
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O3 -I/usr/include/SDL2
LIBS = -L/usr/lib -lSDL2 -lm -lSDL2_image
TARGET = FireBall

SRCS = main.c render.c dinam.c image.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
