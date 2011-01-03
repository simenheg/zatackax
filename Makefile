CC = cc
CFLAGS = $(shell sdl-config --cflags) -g3 -std=c99 -pedantic -Wall
LDFLAGS = $(shell sdl-config --libs) -lSDL_image -lSDL_ttf -lSDL_mixer
CDEBUG = -DDEBUG
CVERBOSE = -DVERBOSE
CSILENT = -DSILENT

OBJECTS = src/zatackax.o src/sound.o src/error.o src/ai.o src/broadcast.o

zatackax : $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

debug : CFLAGS += $(CDEBUG)
debug : zatackax

verbose : CFLAGS += $(CVERBOSE)
verbose : zatackax

silent : CFLAGS += $(CSILENT)
silent : zatackax

src/zatackax.o : src/zatackax.h src/sound.h src/error.h src/ai.h src/broadcast.h src/common.h
src/error.o : src/error.h src/common.h
src/sound.o : src/sound.h src/error.h src/common.h
src/ai.o : src/ai.h src/common.h
src/broadcast.o : src/broadcast.h src/common.h

clean :
	rm -f src/zatackax $(OBJECTS)