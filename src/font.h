#ifndef FONT_H
#define FONT_H

#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#include "data.h"
#include "error.h"

typedef enum font {
    FONT_MENU = 0,
    FONT_MENU_BOLD,
    FONT_BROADCAST,
    FONT_BROADCAST_BOLD,
    FONT_SCORE,
    N_FONTS
} Font;

typedef enum font_size {
    FONT_SIZE_BROADCAST = 13,
    FONT_SIZE_MENU      = 22,
    FONT_SIZE_SCORE     = 22
} FontSize;

extern TTF_Font *fonts[N_FONTS];

bool loadFont(Font f, const char *filename, FontSize size);
bool loadFonts(void);

#endif
