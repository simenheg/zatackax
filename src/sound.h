#ifndef SOUND_H
#define SOUND_H

#include <SDL/SDL_mixer.h>
#include "error.h"

enum sounds {
    SOUND_BEEP,
    SOUND_PEEB
};

int initSound(void);
void playSound(unsigned int sound, int play);

#endif
