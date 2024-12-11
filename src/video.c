/* video -- window and screen handling.
 * Copyright (C) 2010-2021 Simen Heggestøyl <simenheg@runbox.com>
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

SDL_Window *window = NULL;

SDL_Renderer *renderer = NULL;

SDL_Surface *screen = NULL;

SDL_Texture *screen_t = NULL;

SDL_Surface *gameScreen = NULL;

unsigned int WINDOW_W = DEFAULT_WINDOW_W;

unsigned int WINDOW_H = DEFAULT_WINDOW_H;

SDL_Rect gameScreenOnWindowRect = { 0 };

bool screenFreeze = false;

/**
 * Initialize the application window.
 *
 * @return 1 if the initialization was successful, 0 if not.
 */
int initWindow(void)
{
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        screen_t = NULL; /* Destroying the renderer will destroy the texture */
    }
    int width = WINDOW_W;
    int height = WINDOW_H;
    if (fullscreen) {
        if (width * DEFAULT_WINDOW_H > height * DEFAULT_WINDOW_W) {
            height = width * DEFAULT_WINDOW_H / DEFAULT_WINDOW_W;
        } else {
            width = height * DEFAULT_WINDOW_W / DEFAULT_WINDOW_H;
        }
    }
    window = SDL_CreateWindow("Zatacka X",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              width, height,
                              (fullscreen ?
                               SDL_WINDOW_FULLSCREEN :
                               SDL_WINDOW_RESIZABLE));

    SDL_GetWindowSize(window, &width, &height);
    gameScreenOnWindowRect.w = WINDOW_W;
    gameScreenOnWindowRect.h = WINDOW_H;
    gameScreenOnWindowRect.y = (height - WINDOW_H) / 2;
    gameScreenOnWindowRect.x = (width - WINDOW_W) / 2;

    renderer = SDL_CreateRenderer(window, -1, 0);

    return window && renderer;
}

/**
 * Initialize the main screen surfaces.
 *
 * @return 1 if the initialization was successful, 0 if not.
 */
int initScreen(void)
{
    SDL_FreeSurface(screen);
    screen = SDL_CreateRGBSurface(0, WINDOW_W, WINDOW_H, 32,
                                  0x00ff0000,
                                  0x0000ff00,
                                  0x000000ff,
                                  0xff000000);

    gameScreenOnWindowRect.w = WINDOW_W;
    gameScreenOnWindowRect.h = WINDOW_H;

    if (screen_t) {
        SDL_DestroyTexture(screen_t);
    }
    screen_t = SDL_CreateTexture(renderer,
                                 SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 WINDOW_W,
                                 WINDOW_H);

    SDL_FreeSurface(gameScreen);
    gameScreen = SDL_CreateRGBSurface(0, screen->w, screen->h, 32,
                                      0x00ff0000,
                                      0x0000ff00,
                                      0x000000ff,
                                      0xff000000);
    /* SDL_SetColorKey(gameScreen, SDL_SRCCOLORKEY, 0); */

    initParticleScreen(WINDOW_W, WINDOW_H);

    return screen && screen_t && gameScreen && particleScreen;
}
