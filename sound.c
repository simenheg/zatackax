#include "sound.h"

static Mix_Chunk *beep = NULL;
static Mix_Chunk *peeb = NULL;
static Mix_Chunk *sounds[2];

/**
 * Initializes the mixer and loads sound files.
 *
 * @return -1 on failiure, 0 else.
 */
int initSound(void)
{
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT,
                2, 512) == -1 )
    {
        return -1;
    }

    if ((beep = Mix_LoadWAV("data/sound/beep.wav")) == NULL) {
        fileNotFound("data/sound/beep.wav");
        return -1;
    }
    sounds[0] = beep;

    if ((peeb = Mix_LoadWAV("data/sound/peeb.wav")) == NULL) {
        fileNotFound("data/sound/peeb.wav");
        return -1;
    }
    sounds[1] = peeb;

    return 0;
}

/**
 * Plays a sound.
 *
 * @param sound One of the sounds stated in enum sounds (see sound.h).
 * @param play 1 if the sound is to be played, 0 else.
 */
void playSound(unsigned int sound, int play) 
{
    if (play) Mix_PlayChannel(-1, sounds[sound], 0);
}
