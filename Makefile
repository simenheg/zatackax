CC = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall -Wextra -lSDL -lSDL_image -lSDL_ttf
CDEBUG = -DDEBUG

objects = zatackax.o

zatackax : $(objects)
	$(CC) $(CFLAGS) $^ -o $@

debug : CFLAGS += $(CDEBUG)
debug : zatackax

zatackax.o : zatackax.h

clean :
	rm -f zatackax $(objects)
