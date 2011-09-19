CC = cc
CFLAGS = $(shell sdl-config --cflags) -std=c99
LDFLAGS = $(shell sdl-config --libs) -lSDL_image -lSDL_ttf -lSDL_mixer -fno-common
CDEBUG = -DDEBUG
CVERBOSE = -DVERBOSE
CSILENT = -DSILENT
CDEVEL = -g3 -pg -pedantic -Wall
CRELEASE = -O2

OBJECTS = src/zatackax.o src/sound.o src/error.o src/ai.o src/broadcast.o

release : CFLAGS += $(CRELEASE)
release : zatackax

windows : $(OBJECTS)
windows : CC = i486-mingw32-gcc
windows : CFLAGS = $(shell /usr/i486-mingw32/bin/sdl-config --cflags) -std=c99 -O2
windows : LDFLAGS = $(shell /usr/i486-mingw32/bin/sdl-config --libs) -lSDL_image -lSDL_ttf -lSDL_mixer
windows : zatackax

zatackax : $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

devel : CFLAGS += $(CDEVEL)
devel : zatackax

debug : CFLAGS += $(CDEBUG)
debug : devel

verbose : CFLAGS += $(CVERBOSE)
verbose : devel

silent : CFLAGS += $(CSILENT)
silent : devel

src/zatackax.o : src/zatackax.h src/sound.h src/error.h src/ai.h src/broadcast.h src/common.h src/player.h src/weapon.h
src/error.o : src/error.h src/common.h
src/sound.o : src/sound.h src/error.h src/common.h
src/ai.o : src/ai.h src/common.h
src/broadcast.o : src/broadcast.h src/player.h src/common.h

clean :
	rm -f zatackax $(OBJECTS)
