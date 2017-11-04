/* broadcast -- creating broadcast messages.
 * Copyright (C) 2010-2017 The Zatacka X development team
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

#include "broadcast.h"

/**
 * Pushes the broadcasts one step upwards the list, making room for a new one.
 */
void pushBroadcasts(void)
{
    int i;

    SDL_FreeSurface(broadcast[BROADC_LIMIT - 1]);
    for (i = BROADC_LIMIT - 1; i > 0; --i) {
        broadcast[i] = broadcast[i - 1];
        if (broadcast[i] != NULL) {
            int alpha = 255 - i * (255.0 / BROADC_LIMIT);
            SDL_SetAlpha(broadcast[i], SDL_SRCALPHA, alpha);
        }
    }
}

/**
 * Empties the broadcast message queue.
 */
void cleanBroadcasts(void)
{
    int i;
    for (i = 0; i < BROADC_LIMIT; ++i) {
        SDL_FreeSurface(broadcast[i]);
    }
    memset(broadcast, '\0', BROADC_LIMIT * sizeof(SDL_Surface*));
}

/**
 * Constructs a broadcast message. It may consist of up to three "parts".
 * When numbers 1 to MAX_PLAYERS are used, the number is exchanged for a
 * "PlayerN" string, and colored according the current player color.
 *
 * @param msg Message string to be displayed.
 * @param pcolors An array of the current player colors.
 * @return The constructed broadcast message.
 */
SDL_Surface *makeBroadcast(char *msg, SDL_Color pcolors[MAX_PLAYERS])
{
    extern struct player players[MAX_PLAYERS];
    SDL_Surface *broadcast;
    unsigned int i;
    char *strings[BROADC_PARTS];
    SDL_Surface *parts[BROADC_PARTS];
    unsigned int width = 0;

    /* Split message string */
    strings[0] = strtok(msg, ";");
    for (i = 1; (strings[i] = strtok(NULL, ";")) != NULL; ++i) {}

    for (i = 0; i < BROADC_PARTS && strings[i] != NULL; ++i) {

        char pstring[PLAYER_NAME_LEN];
        if (strlen(strings[i]) == 1) {
            int pnum = atoi(strings[i]);
            snprintf(pstring, PLAYER_NAME_LEN, "%s", (&players[pnum - 1])->name);
            parts[i] = TTF_RenderUTF8_Shaded(font_broadc, pstring,
                                             pcolors[pnum - 1], cMenuBG);
        } else
            parts[i] = TTF_RenderUTF8_Shaded(font_broadc, strings[i],
                                             cBroadcast, cMenuBG);

        width += parts[i]->w;
    }

    broadcast = SDL_CreateRGBSurface(SDL_HWSURFACE, width, parts[0]->h,
                                     SCREEN_BPP, 0, 0, 0, 0);

    unsigned int nparts = i;
    int hoffset = 0;
    for (i = 0; i < nparts; ++i) {
        SDL_Rect offset = {hoffset, 0, 0, 0};
        SDL_BlitSurface(parts[i], NULL, broadcast, &offset);
        hoffset += parts[i]->w;
        SDL_FreeSurface(parts[i]);
    }

    return broadcast;
}

/**
 * Adds color to - and broadcasts a message.
 *
 * @param msg Message to broadcast.
 * @param pcolors Colors of the players.
 */
void colorAddBroadcast(char *msg, SDL_Color pcolors[MAX_PLAYERS])
{
    pushBroadcasts();
    broadcast[0] = makeBroadcast(msg, pcolors);
}
