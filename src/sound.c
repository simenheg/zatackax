/* sound -- sound handling.
 * Copyright (C) 2010-2011 The Zatacka X development team
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

static Mix_Music *bgm = NULL;
static Mix_Chunk *seBep = NULL;
static Mix_Chunk *seBeep = NULL;
static Mix_Chunk *seBeeep = NULL;
static Mix_Chunk *seBeeeep = NULL;
static Mix_Chunk *seBeeeeep = NULL;
static Mix_Chunk *seBeeeeeep = NULL;
static Mix_Chunk *seBeeeeeeep = NULL;
static Mix_Chunk *seBeeeeeeeep = NULL;
static Mix_Chunk *seRoundBegin = NULL;
static Mix_Chunk *seCrash = NULL;
static Mix_Chunk *seSpeed = NULL;
static Mix_Chunk *seFreeze = NULL;
static Mix_Chunk *seConfusion = NULL;
static Mix_Chunk *seSharpturn = NULL;
static Mix_Chunk *seTimestep = NULL;
static Mix_Chunk *seMole = NULL;
static Mix_Chunk *seWarp = NULL;
static Mix_Chunk *seGhost = NULL;
static Mix_Chunk *seSwitch = NULL;
static Mix_Chunk *sounds[N_SOUNDS];

/**
 * Loads a sound effect into appropriate sound array index.
 *
 * @param sound Pointer to where to put the loaded sound.
 * @param soundstr Name of the sound (.ogg added).
 * @param index Index in the sound array (0 --> N_SOUNDS).
 * @return -1 on failiure, 0 else.
 */
int loadSound(Mix_Chunk *sound, char *soundstr, int index)
{
    char *soundpath = malloc(STRBUF);
    snprintf(soundpath, STRBUF, "data/sound/%s.ogg", soundstr);
    if ((sound = Mix_LoadWAV(soundpath)) == NULL) {
        fileNotFound(soundpath);
        free(soundpath);
        return -1;
    } else {
        sounds[index] = sound;
        free(soundpath);
        return 0;
    }
}

/**
 * Initializes the mixer and loads sound files.
 *
 * @return -1 on failiure, 0 else.
 */
int initSound(void)
{
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY * 2, MIX_DEFAULT_FORMAT,
                      2, 512) == -1 )
    {
        return -1;
    }

    if ((bgm = Mix_LoadMUS("data/sound/Padanaya_Blokov.ogg")) == NULL) {
        fileNotFound("data/sound/Padanaya_Blokov.ogg");
        return -1;
    }

    if (loadSound(seBep, "bep", SOUND_BEP) == -1)
        return -1;
    if (loadSound(seBeep, "beep", SOUND_BEEP) == -1)
        return -1;
    if (loadSound(seBeeep, "beeep", SOUND_BEEEP) == -1)
        return -1;
    if (loadSound(seBeeeep, "beeeep", SOUND_BEEEEP) == -1)
        return -1;
    if (loadSound(seBeeeeep, "beeeeep", SOUND_BEEEEEP) == -1)
        return -1;
    if (loadSound(seBeeeeeep, "beeeeeep", SOUND_BEEEEEEP) == -1)
        return -1;
    if (loadSound(seBeeeeeeep, "beeeeeeep", SOUND_BEEEEEEEP) == -1)
        return -1;
    if (loadSound(seBeeeeeeeep, "beeeeeeeep", SOUND_BEEEEEEEEP) == -1)
        return -1;
    if (loadSound(seRoundBegin, "round_begin", SOUND_ROUND_BEGIN) == -1)
        return -1;
    if (loadSound(seCrash, "crash", SOUND_CRASH) == -1)
        return -1;
    if (loadSound(seSpeed, "speed", SOUND_SPEED) == -1)
        return -1;
    if (loadSound(seFreeze, "freeze", SOUND_FREEZE) == -1)
        return -1;
    if (loadSound(seConfusion, "confusion", SOUND_CONFUSION) == -1)
        return -1;
    if (loadSound(seSharpturn, "sharpturn", SOUND_SHARPTURN) == -1)
        return -1;
    if (loadSound(seTimestep, "timestep", SOUND_TIMESTEP) == -1)
        return -1;
    if (loadSound(seMole, "mole", SOUND_MOLE) == -1)
        return -1;
    if (loadSound(seWarp, "warp", SOUND_WARP) == -1)
        return -1;
    if (loadSound(seGhost, "ghost", SOUND_GHOST) == -1)
        return -1;
    if (loadSound(seSwitch, "switch", SOUND_SWITCH) == -1)
        return -1;

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
