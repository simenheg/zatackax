#ifndef BROADCAST_H
#define BROADCAST_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "common.h"

#define BROADC_BUF        64  /* Maximum broadcast name length */
#define BROADC_FONT_SIZE  13  /* Broadcast font size */
#define BROADC_LIMIT      6   /* Maximum number of broadcast messages */
#define BROADC_PARTS      3   /* Maximum parts of a broadcast */
#define PLAYER_NAME_LEN   8   /* Length of player name (PlayerN), plus one */

/* Broadcast colors */
static const SDL_Color cBroadcast = {0xFF, 0xFF, 0xFF, 0};
static const SDL_Color cMenuBG    = {0x00, 0x00, 0x00, 0};

/* Broadcast fonts */
extern TTF_Font *font_broadc;
extern TTF_Font *font_broadcb;

SDL_Surface *makeBroadcast(char *msg, SDL_Color pcolors[8]);

#endif
