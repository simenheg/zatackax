#ifndef WEAPON_H
#define WEAPON_H

#include "player.h"

#define N_WEAPONS               8
#define N_ILLEGAL_2P_WEPS       1
#define WEP_SPACEMOD            50
#define PARROWSELECT_MOD_Y      20
#define PARROWSELECT_MOD_X      14
#define WEP_NONACTIVE           -999
#define MIN_TELEPORT_SPACE      5.4 /* If 5.4 results in random suicides,
                                       tune this up slightly */

struct weapon {
    int (*func)(struct player*, bool);
    int charges;
};

int wepSpeedup(struct player *p, bool on);
int wepFrostwave(struct player *p, bool on);
int wepSharpturn(struct player *p, bool on);
int wepConfusion(struct player *p, bool on);
int wepTimestep(struct player *p, bool on);
int wepMole(struct player *p, bool on);
int wepWarp(struct player *p, bool on);
int wepSwitch(struct player *p, bool on);

static struct weapon wep_list[N_WEAPONS] = {{wepSpeedup, 1},
                                            {wepFrostwave, 1},
                                            {wepConfusion, 1},
                                            {wepSharpturn, 3},
                                            {wepTimestep, 1},
                                            {wepMole, 1},
                                            {wepWarp, 2},
                                            {wepSwitch, 1}};

#endif
