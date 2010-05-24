CC = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall
LFLAGS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer
CDEBUG = -DDEBUG
SOURCES = src/*.c
HEADERS = src/*.h

zatackax : $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(LFLAGS) $(SOURCES) -o $@

debug : CFLAGS += $(CDEBUG)
debug : zatackax

clean :
	rm -f zatackax *.o
