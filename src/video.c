/* video -- window and screen handling.
 * Copyright (C) 2010-2020 The Zatacka X development team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "video.h"

SDL_Surface *screen = NULL;

unsigned int WINDOW_W = DEFAULT_WINDOW_W;

unsigned int WINDOW_H = DEFAULT_WINDOW_H;

bool screenFreeze = false;

/**
 * Initialize the application window and screen surface.
 *
 * @return 1 if the initialization was successful, 0 if not.
 */
int initScreen(void)
{
    SDL_FreeSurface(screen);
    screen = SDL_SetVideoMode(WINDOW_W, WINDOW_H, SCREEN_BPP,
                              SDL_SWSURFACE | (fullscreen
                                               ? SDL_FULLSCREEN
                                               : SDL_RESIZABLE));
    return screen == NULL ? 0 : 1;
}
