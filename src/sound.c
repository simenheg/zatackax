/* sound -- sound handling.
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

#include "sound.h"

Mix_Music *bgm = NULL;
Mix_Chunk *sounds[N_SOUNDS];

/**
 * Initialize the mixer. Return -1 on failure, 0 else.
 */
int initSound(void)
{
    return Mix_OpenAudio(MIX_DEFAULT_FREQUENCY * 2, MIX_DEFAULT_FORMAT, 2,
                         512);
}

/**
 * Load a sound file `filename` and identify it as `s`. Return true on
 * success, otherwise false.
 */
bool loadSound(Sound s, const char *filename)
{
    char *file = dataFile("sound", filename);
    Mix_Chunk *sound = Mix_LoadWAV(file);

    if (sound == NULL) {
        fileNotFound(file);
        free(file);
        return false;
    }

    sounds[s] = sound;
    free(file);
    return true;
}

/**
 * Load the background music from `filename`. Return true on success,
 * otherwise false.
 */
bool loadBGM(const char *filename)
{
    char *file = dataFile("sound", filename);
    bgm = Mix_LoadMUS(file);

    if (bgm == NULL) {
        fileNotFound(file);
        free(file);
        return false;
    }

    free(file);
    return true;
}

/**
 * Load every needed sound file. Return true if every file loaded
 * successfully, otherwise false.
 */
bool loadSounds(void)
{
    return loadBGM("theme1.ogg")
        && loadSound(SOUND_BEP, "bep.ogg")
        && loadSound(SOUND_BEEP, "beep.ogg")
        && loadSound(SOUND_BEEEP, "beeep.ogg")
        && loadSound(SOUND_BEEEEP, "beeeep.ogg")
        && loadSound(SOUND_BEEEEEP, "beeeeep.ogg")
        && loadSound(SOUND_BEEEEEEP, "beeeeeep.ogg")
        && loadSound(SOUND_BEEEEEEEP, "beeeeeeep.ogg")
        && loadSound(SOUND_BEEEEEEEEP, "beeeeeeeep.ogg")
        && loadSound(SOUND_ROUND_BEGIN, "round_begin.ogg")
        && loadSound(SOUND_CRASH, "crash.ogg")
        && loadSound(SOUND_SPEED, "speed.ogg")
        && loadSound(SOUND_FREEZE, "freeze.ogg")
        && loadSound(SOUND_CONFUSION, "confusion.ogg")
        && loadSound(SOUND_SHARPTURN, "sharpturn.ogg")
        && loadSound(SOUND_TIMESTEP, "timestep.ogg")
        && loadSound(SOUND_MOLE, "mole.ogg")
        && loadSound(SOUND_WARP, "warp.ogg")
        && loadSound(SOUND_GHOST, "ghost.ogg")
        && loadSound(SOUND_TRON, "tronmode.ogg")
        && loadSound(SOUND_CHILIRUN, "chilirun.ogg")
        && loadSound(SOUND_SWITCH, "switch.ogg");
}

/**
 * Plays a sound.
 *
 * @param sound One of the sounds stated in enum sounds (see sound.h).
 * @param play 1 if the sound is to be played, 0 else.
 */
void playSound(unsigned int sound, int play)
{
    if (play)
        Mix_PlayChannel(-1, sounds[sound], 0);
}

/**
 * Plays the background music.
 */
void playBGM(void)
{
    if (!Mix_PlayingMusic()) {
        if ((Mix_PlayMusic(bgm, -1) == -1)) {
            if (olvl >= O_NORMAL)
                fprintf(stderr, "Couldn't play music: %s\n", Mix_GetError());
        }
    }
}

/**
 * Stops the background music.
 */
void stopBGM(void)
{
    Mix_HaltMusic();
}
