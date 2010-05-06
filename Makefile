CC = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall
LFLAGS = -lSDL -lSDL_image -lSDL_ttf
CDEBUG = -DDEBUG
SOURCES = *.c
HEADERS = *.h

zatackax : $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(LFLAGS) $(SOURCES) -o $@

debug : CFLAGS += $(CDEBUG)
debug : zatackax

zatackax.o : zatackax.h

clean :
	rm -f zatackax zatackax.o
