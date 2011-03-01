#ifndef WEAPON_H
#define WEAPON_H

#include "player.h"

#define N_WEAPONS               8
#define N_ILLEGAL_2P_WEPS       1
#define WEP_SPACEMOD            50
#define PARROWSELECT_MOD_Y      20
#define PARROWSELECT_MOD_X      14
#define WEP_NONACTIVE           -999
#define MIN_TELEPORT_SPACE      5.5 /* If 5.5 results in random suicides,
                                       tune this up slightly */

#define DURATION_LIGHTNINGSPEED 1200
#define DURATION_FROSTWAVE      1500
#define DURATION_CONFUSION      1100

struct weapon {
    int (*func)(struct player*, bool);
    int charges;
    char *name;
    char *desc1;
    char *desc2;
    int duration;
};

int wepLightningspeed(struct player *p, bool on);
int wepFrostwave(struct player *p, bool on);
int wepSharpturn(struct player *p, bool on);
int wepConfusion(struct player *p, bool on);
int wepTimestep(struct player *p, bool on);
int wepMole(struct player *p, bool on);
int wepWarp(struct player *p, bool on);
int wepSwitch(struct player *p, bool on);

static struct weapon wep_list[N_WEAPONS] = {
    {wepLightningspeed, 1, "Lightning-speed", "Gain lightning-speed.", ""},
    {wepFrostwave, 1, "Frost wave", "Slow down your", "opponents."},
    {wepConfusion, 1, "Confusion", "Inverted controls for", "others."},
    {wepSharpturn, 3, "Sharp turn", "Perform an ultra sharp", "turn."},
    {wepTimestep, 1, "Time step", "Jump through time and", "space."},
    {wepMole, 1, "Mole", "Dig your way out.", ""},
    {wepWarp, 3, "Warp", "Warp to a random spot", "on the map."},
    {wepSwitch, 1, "Switch-aroo", "SWITCH-AROOOO!", ""}
};

#endif
