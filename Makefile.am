## Process this file with automake to produce Makefile.in

## Copyright (C) 2010-2019 The Zatacka X development team

## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.

## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.

## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

bin_PROGRAMS = zatackax
zatackax_SOURCES = src/ai.c src/ai.h src/broadcast.c src/broadcast.h src/common.h src/config.c src/config.h src/data.c src/data.h src/error.c src/error.h src/font.c src/font.h src/gfx.c src/gfx.h src/input.c src/input.h src/player.h src/sound.c src/sound.h src/weapon.h src/zatackax.c src/zatackax.h
AM_CPPFLAGS = -DDATADIR=\"$(pkgdatadir)\"
EXTRA_DIST = data AUTHORS.md NEWS.md README.md

devel : CFLAGS += -O0 -g3 -pg -pedantic -Wall -Wextra
devel : AM_CPPFLAGS = -DDATADIR=\"data\"
devel : zatackax

debug : CFLAGS += -DDEBUG
debug : devel

verbose : CFLAGS += -DVERBOSE
verbose : devel

silent : CFLAGS += -DSILENT
silent : devel

install-data-hook :
	mkdir -p $(pkgdatadir);
	cp -r $(srcdir)/data/* $(pkgdatadir);

uninstall-hook :
	rm -rf $(pkgdatadir);
