#ifndef SOUND_H
#define SOUND_H

#include <SDL/SDL_mixer.h>
#include "error.h"

#define N_SOUNDS    4

enum sounds {
    SOUND_BEEP,
    SOUND_PEEB,
    SOUND_SPEED,
    SOUND_STEP
};

int initSound(void);
void playSound(unsigned int sound, int play);

#endif
