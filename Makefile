CC = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall -Wextra -lGLU -lglut -lSDL -lSDL_image -lSDL_ttf

objects = zatackax.o

zatackax : $(objects)
	$(CC) $(CFLAGS) $^ -o $@

zatackax.o : zatackax.h

clean :
	rm -f zatackax $(objects)
