CC = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall
LFLAGS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer
CDEBUG = -DDEBUG

OBJECTS = src/zatackax.o src/sound.o src/error.o src/ai.o

zatackax : $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

debug : CFLAGS += $(CDEBUG)
debug : zatackax

src/zatackax.o : src/zatackax.h src/sound.h src/error.h src/ai.h src/common.h
src/error.o : src/error.h src/common.h
src/sound.o : src/sound.h src/error.h src/common.h
src/ai.o : src/ai.h src/common.h

clean :
	rm -f zatackax $(OBJECTS)
