#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "common.h"

#define PLAYER_NAME_LEN 12 + 1

struct player {
    char name[PLAYER_NAME_LEN];
    unsigned char active;  /* 0 if activated, ID else */
    int weapon;            /* Weapon ID */
    int wep_time;          /* Remaining weapon time, -999 if unused */
    int wep_count;         /* Remaining weapon uses */
    int inv_self;          /* Invincible to oneself when not 0 */
    int inv_others;        /* Invincible to others when not 0 */
    bool alive;
    bool invertedKeys;
    bool ai;               /* Player is AI-controlled */
    SDL_Surface *arrow;
    double posx, initposx;
    double posy, initposy;
    double dir, initdir;
    double speed;
    unsigned int color;
    unsigned int prevx;
    unsigned int prevy;
    unsigned int score;
    unsigned int lkey;
    unsigned int rkey;
    unsigned int wkey;
    int holecount;
};

struct player players[MAX_PLAYERS];
static const char *defaultNames[] = {"Red", "Blue", "Green", "Yellow",
                                     "Pink", "Cyan", "White", "Orange"};

#endif
