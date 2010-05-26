/* sound -- sound handling.
 * Copyright (C) 2010 The Zatacka X development team
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

#include "sound.h"

static Mix_Chunk *beep = NULL;
static Mix_Chunk *peeb = NULL;
static Mix_Chunk *seSpeed = NULL;
static Mix_Chunk *seStep = NULL;
static Mix_Chunk *sounds[N_SOUNDS];

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

    if ((seSpeed = Mix_LoadWAV("data/sound/lightningspeed.wav")) == NULL) {
        fileNotFound("data/sound/lightningspeed.wav");
        return -1;
    }
    sounds[2] = seSpeed;

    if ((seStep = Mix_LoadWAV("data/sound/timestep.wav")) == NULL) {
        fileNotFound("data/sound/timestep.wav");
        return -1;
    }
    sounds[3] = seStep;

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
