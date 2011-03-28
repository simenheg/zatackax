#ifndef SOUND_H
#define SOUND_H

#include <SDL/SDL_mixer.h>
#include "common.h"
#include "error.h"

#define N_SOUNDS 12

enum sounds {
    SOUND_BEEP,
    SOUND_PEEB,
    SOUND_CRASH,
    SOUND_SPEED,
    SOUND_FREEZE,
    SOUND_CONFUSION,
    SOUND_SHARPTURN,
    SOUND_TIMESTEP,
    SOUND_MOLE,
    SOUND_WARP,
    SOUND_GHOST,
    SOUND_SWITCH
};

int loadSound(Mix_Chunk *sound, char *soundstr, int index);
int initSound(void);
void playSound(unsigned int sound, int play);
void playBGM(void);
void stopBGM(void);

#endif
