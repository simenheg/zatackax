#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "common.h"
#include "input.h"

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
    button lkey;
    button rkey;
    button wkey;
    int holecount;
};

extern struct player players[MAX_PLAYERS];

#endif
