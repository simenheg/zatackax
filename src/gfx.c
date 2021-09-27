/* gfx -- image file loading.
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

#include "gfx.h"

SDL_Color colors[N_COLORS];

SDL_Surface *images[N_IMAGES];

/**
 * Fill a surface with black pixels.
 *
 * Return 0 on success, -1 on error.
 */
int clearSurface(SDL_Surface *s)
{
    Uint32 color = SDL_MapRGB(s->format, 0x00, 0x00, 0x00);
    return SDL_FillRect(s, &s->clip_rect, color);
}

/**
 * Load an icon file named `filename`. Return the loaded image, or
 * NULL if it wasn't loaded.
 */
SDL_Surface *loadIcon(const char *filename)
{
    char *file = dataFile("gfx", filename);
    SDL_Surface *icon = SDL_LoadBMP(file);

    if (icon == NULL) {
        fileNotFound(file);
    }

    free(file);
    return icon;
}

/**
 * Load an image file named `filename` and identify it as `i`. Return
 * true on success, otherwise false.
 */
bool loadImage(Image i, const char *filename)
{
    char *file = dataFile("gfx", filename);
    SDL_Surface *loadedImage = IMG_Load(file);

    if (loadedImage == NULL) {
        fileNotFound(file);
        free(file);
        return false;
    }

    images[i] = SDL_DisplayFormatAlpha(loadedImage);
    if (olvl >= O_DEBUG) {
        printf("Loaded: %s\t(w:%d h:%d bpp:%d)\n", filename, images[i]->w,
               images[i]->h, images[i]->format->BitsPerPixel);
    }
    SDL_FreeSurface(loadedImage);

    free(file);
    return true;
}

/**
 * Load every needed image file. Return true if every file loaded
 * successfully, otherwise false.
 */
bool loadImages(void)
{
    return loadImage(IMG_ARROWS, "arrowsheet.png")
        && loadImage(IMG_BALL, "ball.png")
        && loadImage(IMG_LOGO, "logo.png")
        && loadImage(IMG_WIS_CHILIRUN, "wis_chilirun.png")
        && loadImage(IMG_WIS_CONFUSION, "wis_confusion.png")
        && loadImage(IMG_WIS_FROST, "wis_frostwave.png")
        && loadImage(IMG_WIS_GHOST, "wis_ghost.png")
        && loadImage(IMG_WIS_MOLE, "wis_mole.png")
        && loadImage(IMG_WIS_ROCKETLAUNCHER, "wis_rocketlauncher.png")
        && loadImage(IMG_WIS_SHARPTURN, "wis_sharpturn.png")
        && loadImage(IMG_WIS_SPEED, "wis_lightningspeed.png")
        && loadImage(IMG_WIS_TIMESTEP, "wis_timestep.png")
        && loadImage(IMG_WIS_TRON, "wis_tron.png")
        && loadImage(IMG_WIS_WARP, "wis_warp.png")
        && loadImage(IMG_WI_BG, "wi_bg.png")
        && loadImage(IMG_WI_CHILIRUN, "wi_chilirun.png")
        && loadImage(IMG_WI_CONFUSION, "wi_confusion.png")
        && loadImage(IMG_WI_FROST, "wi_frostwave.png")
        && loadImage(IMG_WI_GHOST, "wi_ghost.png")
        && loadImage(IMG_WI_MOLE, "wi_mole.png")
        && loadImage(IMG_WI_ROCKETLAUNCHER, "wi_rocketlauncher.png")
        && loadImage(IMG_WI_SHARPTURN, "wi_sharpturn.png")
        && loadImage(IMG_WI_SPEED, "wi_lightningspeed.png")
        && loadImage(IMG_WI_TIMESTEP, "wi_timestep.png")
        && loadImage(IMG_WI_TRON, "wi_tron.png")
        && loadImage(IMG_WI_WARP, "wi_warp.png");
}

/**
 * Initialize the player colors.
 */
void initColors(void)
{
    SDL_Color *c = &colors[0];
    c->r = 0xFF; c->g = 0x00; c->b = 0x00; ++c; /* Red */
    c->r = 0x00; c->g = 0x6F; c->b = 0xFF; ++c; /* Blue */
    c->r = 0x00; c->g = 0xFF; c->b = 0x00; ++c; /* Green */
    c->r = 0xFF; c->g = 0xFF; c->b = 0x00; ++c; /* Yellow */
    c->r = 0xFF; c->g = 0x00; c->b = 0xFF; ++c; /* Pink */
    c->r = 0x00; c->g = 0xFF; c->b = 0xFF; ++c; /* Cyan */
    c->r = 0xFF; c->g = 0xFF; c->b = 0xFF; ++c; /* White */
    c->r = 0xFF; c->g = 0x80; c->b = 0x00; ++c; /* Orange */
}
