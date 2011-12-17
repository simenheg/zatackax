# Makefile for Zatacka X.
# Copyright (C) 2010-2011 The Zatacka X development team

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

CFLAGS = $(shell pkg-config sdl --cflags) -std=c99
LDFLAGS = $(shell pkg-config sdl --libs) -lm -lSDL_image -lSDL_ttf -lSDL_mixer -fno-common
CDEBUG = -DDEBUG
CVERBOSE = -DVERBOSE
CSILENT = -DSILENT
CDEVEL = -g3 -pg -pedantic -Wall
CRELEASE = -O2

OBJECTS = src/zatackax.o src/sound.o src/error.o src/ai.o src/broadcast.o src/stats.o

release : CFLAGS += $(CRELEASE)
release : zatackax

windows : $(OBJECTS)
windows : CC = i486-mingw32-gcc
windows : CFLAGS = $(shell /usr/i486-mingw32/bin/sdl-config --cflags) -std=c99 -O2
windows : LDFLAGS = $(shell /usr/i486-mingw32/bin/sdl-config --libs) -lm -lSDL_image -lSDL_ttf -lSDL_mixer
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
src/stats.o : src/common.h

.PHONY: clean
clean :
	rm -f zatackax .zatackax $(OBJECTS)