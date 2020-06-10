#ifndef VIDEO_H
#define VIDEO_H

#include <stdbool.h>

#include <SDL/SDL.h>

#include "common.h"
#include "config.h"

/*
 * The main framebuffer surface.
 */
extern SDL_Surface *screen;

/*
 * Surface holding graphics that don't need to change every frame
 * during a game round (i.e. everything except particles).
 */
extern SDL_Surface *gameScreen;

/*
 * Current window width and height.
 */
extern unsigned int WINDOW_W;
extern unsigned int WINDOW_H;

/*
 * True to temporarily freeze what's currently being displayed.
 */
extern bool screenFreeze;

int initScreen(void);

#endif
