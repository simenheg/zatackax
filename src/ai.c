/* ai -- artificial intelligence experimentation.
 * Copyright (C) 2010-2018 The Zatacka X development team
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

#include "ai.h"

#define COWARDNESS_FRONT 85
#define COWARDNESS_SIDES 40
#define COWARDNESS_DIAGS 60

/**
 * NOTE: This is only a quick doodle, and is not to be considered final!
 *
 * Polls the AI to decide what action to do.
 *
 * @param posx Current x position.
 * @param posy Current y position.
 * @param dir Current direction.
 * @param id ID of the player to decide actions for.
 * @param hitmap A pointer to the whole level hitmap.
 * @param w Width of the playfield.
 * @param h Height of the playfield.
 * @return 'l' to turn left, 'r' to turn right, '0' else.
 */
char pollAi(double posx, double posy, double dir, unsigned char id,
            unsigned char *hitmap, unsigned int w, unsigned int h)
{
    int i;
    for (i = 0; i < COWARDNESS_FRONT; ++i) {
        int checkx, checky, result;
        if (i < COWARDNESS_DIAGS) {
            /* left diagonal */
            checkx = posx + i*cos(dir - M_PI_4);
            checky = posy + i*sin(dir - M_PI_4);
            if (checkx < 0 || checky < 0 || (unsigned)checkx >= w ||
                    (unsigned)checky >= h)
                return 'r';
            result = hitmap[w * checky + checkx];
            if (result != id + 8 && result != id + 16 && result != 0)
                return 'r';
            /* right diagonal */
            checkx = posx + i*cos(dir + M_PI_4);
            checky = posy + i*sin(dir + M_PI_4);
            if (checkx < 0 || checky < 0 || (unsigned)checkx >= w ||
                    (unsigned)checky >= h)
                return 'r';
            result = hitmap[w * checky + checkx];
            if (result != id + 8 && result != id + 16 && result != 0)
                return 'l';
        }
        if (i < COWARDNESS_SIDES) {
            /* right side */
            checkx = posx + i*cos(dir + M_PI_2);
            checky = posy + i*sin(dir + M_PI_2);
            if (checkx < 0 || checky < 0 || (unsigned)checkx >= w ||
                    (unsigned)checky >= h)
                return 'r';
            result = hitmap[w * checky + checkx];
            if (result != id + 8 && result != id + 16 && result != 0)
                return 'l';
            /* left side */
            checkx = posx + i*cos(dir - M_PI_2);
            checky = posy + i*sin(dir - M_PI_2);
            if (checkx < 0 || checky < 0 || (unsigned)checkx >= w ||
                    (unsigned)checky >= h)
                return 'r';
            result = hitmap[w * checky + checkx];
            if (result != id + 8 && result != id + 16 && result != 0)
                return 'r';
        }
        /* front */
        checkx = posx + i * cos(dir);
        checky = posy + i * sin(dir);
        if (checkx < 0 || checky < 0 || (unsigned)checkx >= w ||
                (unsigned)checky >= h)
            return 'r';
        result = hitmap[w * checky + checkx];
        if (result != id + 8 && result != id + 16 && result != 0) {
            if (result == id)
                return 'l';
            else
                return 'r';
        }
    }

    return '0';
}
