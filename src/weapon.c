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
    {wepLightningspeed, addParticlesLightningspeed, 1, "Lightning-speed",
     "Gain lightning-speed.", ""},
    {wepFrostwave, addParticlesFrostwave, 1, "Frost wave",
     "Slow down your", "opponents."},
    {wepConfusion, addParticlesConfusion, 1, "Confusion",
     "Inverted controls for", "others."},
    {wepSharpturn, addParticlesSharpturn, 3, "Sharp turn",
     "Perform an ultra sharp", "turn."},
    {wepTimestep, addParticlesVoid, 1, "Time step",
     "Jump through time and", "space."},
    {wepMole, addParticlesVoid, 2, "Mole",
     "Dig your way out.", ""},
    {wepWarp, addParticlesLightningspeed, 4, "Warp",
     "Warp to a random spot", "on the map."},
    {wepGhost, addParticlesGhost, 1, "Ghost walk",
     "Transform into ghost", "form."},
    {wepTron, addParticlesTron, 1, "Tron-mode",
     "No more smooth turns.", ""},
    {wepChilirun, addParticlesChilirun, 1, "Chili run",
     "Hot hot hot!", ""}
};

void addParticlesVoid(struct player *p __attribute__((unused)),
                      Uint32 delta __attribute__((unused)))
{}

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

void addParticlesLightningspeed(struct player *p, Uint32 delta)
{
    addParticles(delta * 2.5,
                 p->posx, p->posy,
                 0, 0.4,
                 p->dir + M_PI - 0.12, p->dir + M_PI + 0.12,
                 0.5, 1, 0.0005,
                 235, 253,
                 224, 247,
                 102, 185);
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

void addParticlesFrostwave(struct player *p, Uint32 delta)
{
    for (int i = 0; i < nPlayers; ++i) {
        struct player *target = &players[i];
        if (target != p && target->alive) {
            addParticles(delta / 2.0,
                         target->posx, target->posy,
                         0, 0.04,
                         0, 2*M_PI,
                         1, 1, 0.0005,
                         28, 28,
                         212, 212,
                         228, 228);
        }
    }
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

void addParticlesConfusion(struct player *p, Uint32 delta)
{
    for (int i = 0; i < nPlayers; ++i) {
        struct player *target = &players[i];
        if (target != p && target->alive) {
            addParticleSpinner(delta, target, 0.03, 20,
                               0.08, 0.1,
                               138, 255,
                               0, 0,
                               255, 255);
        }
    }
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

void addParticlesSharpturn(struct player *p, Uint32 delta)
{
    addParticles(delta / 2.0,
                 p->posx, p->posy,
                 0.1, 0.5,
                 p->dir + M_PI - 0.05, p->dir + M_PI + 0.05,
                 1, 1, 0.001,
                 0, 218,
                 183, 239,
                 22, 221);
}

/**
 * Weapon: Time step.
 */
int wepTimestep(struct player *p, bool on)
{
    if (on) {
        if (particleEffects) {
            addParticles(50,
                         p->posx, p->posy,
                         0.05, 0.2,
                         0, 2*M_PI,
                         0.5, 1, 0.0005,
                         255, 255,
                         255, 255,
                         255, 255);
        }

        playSound(SOUND_TIMESTEP, sound);
        p->posx += 90 * cos(p->dir);
        p->posy += 90 * sin(p->dir);

        if (particleEffects) {
            addParticles(50,
                         p->posx, p->posy,
                         0.05, 0.2,
                         0, 2*M_PI,
                         0.5, 1, 0.0005,
                         255, 255,
                         255, 255,
                         255, 255);
        }
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

        if (particleEffects) {
            SDL_Color color = colors[p->color];
            addParticles(100,
                         p->posx, p->posy,
                         0, 0.1,
                         p->dir - 0.2, p->dir + 0.2,
                         0.5, 1, 0.0005,
                         color.r, color.r,
                         color.b, color.b,
                         color.g, color.g);
        }

        double oldposx = p->posx;
        double oldposy = p->posy;
        double olddir = p->dir;

        p->posx = p->initposx;
        p->posy = p->initposy;
        p->dir = p->initdir + M_PI;

        p->initposx = oldposx;
        p->initposy = oldposy;
        p->initdir = olddir + M_PI;

        p->inv_self = DURATION_MOLE;
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

        if (particleEffects) {
            addParticles(1000,
                         p->posx, p->posy,
                         0, 0.5,
                         0, 2*M_PI,
                         0.9, 1, 0.0005,
                         54, 183,
                         3, 211,
                         97, 246);
        }
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

void addParticlesGhost(struct player *p, Uint32 delta)
{
    addParticles(delta * 8,
                 p->posx, p->posy,
                 0, 0.2,
                 0, 2*M_PI,
                 0.8, 1, 0.0009,
                 91, 192,
                 15, 119,
                 100, 199);
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

void addParticlesTron(struct player *p __attribute__((unused)), Uint32 delta)
{
    for (int i = 0; i < nPlayers; ++i) {
        struct player *target = &players[i];
        if (target->alive) {
            addParticles(delta,
                         target->posx, target->posy,
                         0, 0.1,
                         target->dir + M_PI, target->dir + M_PI,
                         1, 1, 0.0005,
                         195, 195,
                         194, 194,
                         255, 255);
            addParticles(delta,
                         target->posx, target->posy,
                         0, 0.1,
                         target->dir, target->dir,
                         1, 1, 0.0005,
                         195, 195,
                         194, 194,
                         255, 255);
        }
    }
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

void addParticlesChilirun(struct player *p, Uint32 delta)
{
    for (int i = 0; i < nPlayers; ++i) {
        struct player *target = &players[i];
        if (target != p && target->alive) {
            addParticles(delta * 10,
                         target->posx, target->posy,
                         0, 0.5,
                         0, 2*M_PI,
                         0.1, 1, 0.001,
                         223, 255,
                         31, 220,
                         26, 220);
        }
    }
}
