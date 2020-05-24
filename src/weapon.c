/* weapon -- weapon definitions.
 * Copyright (C) 2010-2020 The Zatacka X development team
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

#include "weapon.h"

bool activeFreeze = false;
bool activeConfusion = false;
bool activeTron = false;
bool activeChilirun = false;

struct weapon wep_list[N_WEAPONS] = {
    {wepLightningspeed, 1, "Lightning-speed", "Gain lightning-speed.", ""},
    {wepFrostwave, 1, "Frost wave", "Slow down your", "opponents."},
    {wepConfusion, 1, "Confusion", "Inverted controls for", "others."},
    {wepSharpturn, 3, "Sharp turn", "Perform an ultra sharp", "turn."},
    {wepTimestep, 1, "Time step", "Jump through time and", "space."},
    {wepMole, 2, "Mole", "Dig your way out.", ""},
    {wepWarp, 4, "Warp", "Warp to a random spot", "on the map."},
    {wepGhost, 1, "Ghost walk", "Transform into ghost", "form."},
    {wepTron, 1, "Tron-mode", "No more smooth turns.", ""},
    {wepChilirun, 1, "Chili run", "Hot hot hot!", ""}
};

/**
 * Weapon: Lightning-speed.
 */
int wepLightningspeed(struct player *p, bool on)
{
    if (on) {
        playSound(SOUND_SPEED, sound);
        p->speed = 2.0;
    } else {
        p->speed = 1.0;
    }
    return DURATION_LIGHTNINGSPEED;
}

/**
 * Weapon: Frost wave.
 */
int wepFrostwave(struct player *p, bool on)
{
    activeFreeze = on;

    if (on) {
        playSound(SOUND_FREEZE, sound);
    } else {
        refreshGameScreen();
    }

    for (int i = 0; i < nPlayers; ++i) {
        struct player *target = &players[i];
        if (target != p) {
            target->speed = on ? 0.3 : 1.0;
        }
    }
    return DURATION_FROSTWAVE;
}

/**
 * Weapon: Confusion.
 */
int wepConfusion(struct player *p, bool on)
{
    activeConfusion = on;

    if (on) {
        playSound(SOUND_CONFUSION, sound);
    } else {
        refreshGameScreen();
    }

    for (int i = 0; i < nPlayers; ++i) {
        struct player *target = &players[i];
        if (target != p) {
            unsigned int tmpkey = target->lkey;
            target->invertedKeys = on;
            target->lkey = target->rkey;
            target->rkey = tmpkey;
        }
    }

    return DURATION_CONFUSION;
}

/**
 * Weapon: Sharp turn.
 */
int wepSharpturn(struct player *p, bool on)
{
    if (on) {
        playSound(SOUND_SHARPTURN, sound);

        if (keyDown[p->rkey]) {
            p->dir += M_PI_2;
        } else {
            p->dir -= M_PI_2;
        }
    }

    p->inv_self = INV_TIME;

    return DURATION_SHARPTURN;
}

/**
 * Weapon: Time step.
 */
int wepTimestep(struct player *p, bool on)
{
    if (on) {
        playSound(SOUND_TIMESTEP, sound);
        p->posx += 90 * cos(p->dir);
        p->posy += 90 * sin(p->dir);
    }
    return WEP_NONACTIVE;
}

/**
 * Weapon: Mole.
 */
int wepMole(struct player *p, bool on)
{
    if (on) {
        playSound(SOUND_MOLE, sound);

        double oldposx = p->posx;
        double oldposy = p->posy;
        double olddir = p->dir;

        p->posx = p->initposx;
        p->posy = p->initposy;
        p->dir = p->initdir + M_PI;

        p->initposx = oldposx;
        p->initposy = oldposy;
        p->initdir = olddir + M_PI;

        p->inv_self= DURATION_MOLE;
        p->inv_others = DURATION_MOLE;
    }

    return WEP_NONACTIVE;
}

/**
 * Weapon: Warp.
 */
int wepWarp(struct player *p, bool on)
{
    if (on) {
        playSound(SOUND_WARP, sound);

        double rnd = (double)rand()/RAND_MAX;
        p->posx = SPAWN_SPACE_MIN
            + (rnd * (WINDOW_W - 2*SPAWN_SPACE_MIN));
        rnd = (double)rand()/RAND_MAX;
        p->posy = SPAWN_SPACE_MIN
            + (rnd * (WINDOW_H - 2*SPAWN_SPACE_MIN));
    }

    return WEP_NONACTIVE;
}

/**
 * Weapon: Ghost walk.
 */
int wepGhost(struct player *p, bool on)
{
    if (on) {
        playSound(SOUND_GHOST, sound);
        p->inv_self = DURATION_GHOSTWALK;
        p->inv_others = DURATION_GHOSTWALK;
    }

    return 0;
}

/**
 * Weapon: Tron-mode.
 */
int wepTron(struct player *p __attribute__ ((unused)), bool on)
{
    activeTron = on;

    if (on) {
        playSound(SOUND_TRON, sound);
    } else {
        refreshGameScreen();
    }

    return DURATION_TRON;
}


/**
 * Weapon: Chili run.
 */
int wepChilirun(struct player *p, bool on)
{
    activeChilirun = on;

    if (on) {
        playSound(SOUND_CHILIRUN, sound);
    } else {
        refreshGameScreen();
    }

    for (int i = 0; i < nPlayers; ++i) {
        struct player *target = &players[i];
        if (target != p) {
            target->speed = on ? 2 : 1;
        }
    }

    return DURATION_CHILIRUN;
}
