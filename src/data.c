/* data -- handling of data files.
 * Copyright (C) 2010-2020 Simen Heggestøyl <simenheg@runbox.com>
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

#include "data.h"

/**
 * Return the full file name of the data file named `name`, found in
 * the data directory named `dir`.
 */
char *dataFile(const char *dir, const char *name)
{
    int strlen = snprintf(NULL, 0, "%s/%s/%s", DATADIR, dir, name);
    char *file = calloc(strlen + 1, sizeof(char));
    snprintf(file, strlen + 1, "%s/%s/%s", DATADIR, dir, name);
    return file;
}
