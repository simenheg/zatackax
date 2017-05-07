#ifndef SOUND_H
#define SOUND_H

#include <SDL/SDL_mixer.h>
#include "common.h"
#include "error.h"

enum sounds {
    SOUND_BEP = 0,
    SOUND_BEEP,
    SOUND_BEEEP,
    SOUND_BEEEEP,
    SOUND_BEEEEEP,
    SOUND_BEEEEEEP,
    SOUND_BEEEEEEEP,
    SOUND_BEEEEEEEEP,
    SOUND_ROUND_BEGIN,
    SOUND_CRASH,
    SOUND_SPEED,
    SOUND_FREEZE,
    SOUND_CONFUSION,
    SOUND_SHARPTURN,
    SOUND_TIMESTEP,
    SOUND_MOLE,
    SOUND_WARP,
    SOUND_GHOST,
    SOUND_TRON,
    SOUND_CHILIRUN,
    SOUND_SWITCH,
    N_SOUNDS
};

int loadSound(Mix_Chunk *sound, char *soundstr, int index);
int initSound(void);
void playSound(unsigned int sound, int play);
void playBGM(void);
void stopBGM(void);

#endif
