/* zatackax -- main game module.
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

#include "zatackax.h"

static const unsigned int FPS_CAP = 100;

struct menu menuMain = {
    3,
    0,
};

struct menu menuSettings = {
    11,
    0,
};

struct menu menuPlayer = {
    MAX_PLAYERS + 1,
    0,
};

struct menu menuPConf = {
    7,
    0,
};

static struct scene mainMenu = {
    logicMainMenu,
    displayMainMenu,
    NULL
};

static struct scene wepMenu = {
    logicWepMenu,
    displayWepMenu,
    &mainMenu
};

static struct scene gameStart = {
    logicGameStart,
    displayGameStart,
    &mainMenu
};

static struct scene game = {
    logicGame,
    displayGame,
    &mainMenu
};

static struct scene settingsMenu = {
    logicSettingsMenu,
    displaySettingsMenu,
    &mainMenu
};

static struct scene playerMenu = {
    logicPlayerMenu,
    displayPlayerMenu,
    &settingsMenu
};

static struct scene pConfMenu = {
    logicPConfMenu,
    displayPConfMenu,
    &playerMenu
};

struct scene *curScene = NULL;

struct player players[MAX_PLAYERS];
unsigned char nPlayers = DEFAULT_N_PLAYERS;

/**
 * Stage 1 of player initialization.
 * Assigns keys and colors.
 */
void initPlayers1(void)
{
    for (int i = 0; i < MAX_PLAYERS; ++i)
        resetPlayer(i);
}

/**
 * Stage 2 of player initialization.
 * Called at a later point than stage 1. Assigns arrow sprites and resets
 * scores.
 */
void initPlayers2(void)
{
    int i;
    struct player *p = &players[0];
    SDL_Surface **s = parrows;

    for (i = 0; i < nPlayers; ++i, ++p, ++s) {

        p->active = i + 1;
        p->score = 0;

        /* Assign arrows */
        SDL_BlitSurface(images[IMG_ARROWS], NULL, *s, NULL);
        SDL_LockSurface(*s);
        colorFill(colors[p->color], *s);
        SDL_UnlockSurface(*s);
        p->arrow = *s;
    }

    for (; i < MAX_PLAYERS; ++i, ++p) {
        p->active = 0;
    }
}

/**
 * Reset a player struct.
 *
 * @param player ID of the player to be reset.
 */
void resetPlayer(int player)
{
    char *defaultNames[] = {"Red", "Blue", "Green", "Yellow",
                            "Pink", "Cyan", "White", "Orange"};

    struct player *p = &(players[player]);
    p->color = player;
    p->speed = 1.0;
    p->invertedKeys = false;
    p->weapon = false;
    p->ai = false;
    strncpy(p->name, defaultNames[player], PLAYER_NAME_LEN - 1);

    switch (player) {
    case 0:
        p->lkey = SDLK_LEFT; p->rkey = SDLK_RIGHT;
        p->wkey = SDLK_UP;
        break;
    case 1:
        p->lkey = 'z'; p->rkey = 'c'; p->wkey = 'x';
        break;
    case 2:
        p->lkey = 'v'; p->rkey = 'n'; p->wkey = 'b';
        break;
    case 3:
        p->lkey = ','; p->rkey = '-'; p->wkey = '.';
        break;
    case 4:
        p->lkey = 'q'; p->rkey = 'e'; p->wkey = 'w';
        break;
    case 5:
        p->lkey = 'r'; p->rkey = 'y'; p->wkey = 't';
        break;
    case 6:
        p->lkey = 'i'; p->rkey = 'p'; p->wkey = 'o';
        break;
    case 7:
        p->lkey = SDLK_F1; p->rkey = SDLK_F3; p->wkey = SDLK_F2;
        break;
    default:
        break;
    }
}

/**
 * Sets all selected weapons back to first.
 */
void deselectWeapons(void)
{
    for (int i = 0; i < nPlayers; ++i)
        (&players[i])->weapon = 0;
}

/**
 * Extract and return an array containing the current player colors.
 *
 * @return Current player colors.
 */
SDL_Color *extractPColors(void)
{
    SDL_Color *pcolors = malloc(sizeof(SDL_Color) * N_COLORS);
    for (int i = 0; i < N_COLORS; ++i)
        pcolors[i] = colors[players[i].color];
    return pcolors;
}

/**
 * Kills target player.
 *
 * @param victim ID of the player that was victim.
 * @param killer ID of the player that was crashed into.
 */
void killPlayer(unsigned char victim, unsigned char killer)
{
    playSound(SOUND_CRASH, sound);

    if (broadcasts) {
        char msg[BROADC_BUF];

        if (killer == 0)
            snprintf(msg, BROADC_BUF, "%d; hit the wall", victim);
        else if (killer == victim)
            snprintf(msg, BROADC_BUF, "%d; committed suicide", victim);
        else
            snprintf(msg, BROADC_BUF, "%d; crashed into ;%d", victim, killer);

        /* Send forward those colors */
        newBroadcast(msg);
    }

    struct player *p = &players[victim - 1];
    --alivecount;
    p->alive = false;

    for (int i = 0; i < MAX_PLAYERS; ++i) {
        struct player *pt = &players[i];
        if (pt->active) {
            if (pt->alive) {
                pt->score++;
                if (pt->score == scorecap) {
                    playerWon(i);
                    screenFreeze = true;
                }
            }
        } else {
            break;
        }
    }

    refreshGameScreen(); /* Update scores */
}

/**
 * Add a new broadcast message to the queue.
 *
 * @param msg Message to broadcast
 */
void newBroadcast(char *msg)
{
    SDL_Color *pcolors = extractPColors();
    colorAddBroadcast(msg, pcolors);
    free(pcolors);
}

/**
 * Shows which player won the game.
 * TODO: Handle ties.
 *
 * @param id ID of the player that won.
 */
void playerWon(unsigned char id)
{
    if (broadcasts) {
        char msg[BROADC_BUF];
        snprintf(msg, BROADC_BUF, "%d; won the game! (press RETURN to play "
                 "again, or ESC to exit)", id + 1);
        newBroadcast(msg);
    }

    winnerDeclared = true;

    if (olvl >= O_VERBOSE)
        printf(" -- Player %d won! --\n", id + 1);
}

/**
 * Provides a random position/direction vector.
 *
 * @return The velocity vector of the newly spawned player.
 */
struct vel spawn(void)
{
    double rnd;
    struct vel initPos;

    rnd = (double)rand() / RAND_MAX;
    initPos.x = SPAWN_SPACE_MIN
        + (rnd * (WINDOW_W - (2 * SPAWN_SPACE_MIN)));
    rnd = (double)rand() / RAND_MAX;
    initPos.y = SPAWN_SPACE_MIN
        + (rnd * (WINDOW_H - (2 * SPAWN_SPACE_MIN)));
    rnd = (double)rand() / RAND_MAX;
    initPos.holecount = (rnd * (HOLE_FREQ - HOLE_FIRST_DELAY));
    rnd = (double)rand() / RAND_MAX;
    initPos.dir = rnd * (2 * M_PI);
    if (initPos.dir < 0)
        initPos.dir *= -1;

    return initPos;
}

/**
 * Spawns a player randomly on the map.
 *
 * @param p The player which is to be spawned.
 */
void respawn(struct player *p)
{
    struct vel initPos;
    int posOK = 1;
    int n = TRY_SPAWN_THIS_HARD;

    while (posOK && n != 0) {
        posOK = p->active - 1;
        initPos = spawn();
        p->posx = initPos.x;
        p->posy = initPos.y;
        for (int i = 0; i < p->active - 1; ++i) {
            struct player *comp = &players[i];
            /* Securing spawning space between zatas. This may cause
             * trouble at small maps (will never get a fit).
             * ZATA_SPAWN_SPACING cuts off the waiting. */
            if (fabs(p->posx - comp->posx) > ZATA_SPAWN_SPACING
                && fabs(p->posy - comp->posy) > ZATA_SPAWN_SPACING) {
                --posOK;
            }
        }
        --n;
    }

    p->alive = true;
    p->dir = initPos.dir;

    p->initposx = p->posx;
    p->initposy = p->posy;
    p->initdir = p->dir;

    p->prevx = p->posx;
    p->prevy = p->posy;

    p->holecount = initPos.holecount;
}

/**
 * Spawns a player for duel mode.
 *
 * @param p The player which is to be spawned.
 */
void drespawn(struct player *p)
{
    p->alive = true;
    p->posx = (WINDOW_W / 2) + (p->active == 1 ? -120 : 120);
    p->posy = WINDOW_H / 2;
    p->dir = p->active == 1 ? 0 : M_PI;

    p->initposx = p->posx;
    p->initposy = p->posy;
    p->initdir = p->dir;

    p->prevx = p->posx;
    p->prevy = p->posy;
    p->holecount = (rand() % (HOLE_FREQ - HOLE_FIRST_DELAY));
}

/**
 * Sets a player's color index up or down.
 *
 * @param pedit ID of the player which color is to be changed.
 * @param up 1 if the color should be set one higher, 0 if one lower.
 */
void setColor(unsigned char pedit, bool up)
{
    if (up) {
        if ((&players[pedit])->color == N_COLORS - 1)
            (&players[pedit])->color = 0;
        else
            ++((&players[pedit])->color);
    } else {
        if ((&players[pedit])->color == 0)
            (&players[pedit])->color = N_COLORS - 1;
        else
            --((&players[pedit])->color);
    }
}

/**
 * Capture and set the next key pressed as a player's directional or
 * weapon key.
 *
 * @param pedit ID of the player to edit.
 * @param key l - left key, r - right key, w - weapon key
 */
void setNextKey(unsigned char pedit, unsigned char key)
{
    for (;;) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN
                    || event.type == SDL_MOUSEBUTTONDOWN
                    || event.type == SDL_JOYBUTTONDOWN
                    || event.type == SDL_JOYAXISMOTION) {

                int k;
                char *keyname;

                if (event.type == SDL_JOYBUTTONDOWN) {
                    Uint8 joyIndex = event.jbutton.which + 1;
                    k = event.jbutton.button | joyIndex << 9;
                    keyname = buttonName(k);
                }
                else if (event.type == SDL_JOYAXISMOTION) {
                    Uint8 joyIndex = event.jaxis.which + 1;
                    k = axisNumber(event.jaxis);
                    k = (k << 4) | (joyIndex << 9);
                    keyname = buttonName(k);
                }
                else {
                    if (event.type == SDL_KEYDOWN) {
                        k = event.key.keysym.sym;
                    }
                    else {
                        k = event.button.button;
                    }

                    keyname = buttonName(k);
                }

                if (keyname[0] == '\0') {
                    if (olvl >= O_DEBUG)
                        fprintf(stderr, "Unknown key\n");
                } else {
                    if (olvl >= O_DEBUG)
                        fprintf(stderr, "Set new key: %s\n", keyname);
                }

                switch (key) {
                case 'l':
                    (&players[pedit])->lkey = k;
                    break;
                case 'r':
                    (&players[pedit])->rkey = k;
                    break;
                case 'w':
                    (&players[pedit])->wkey = k;
                default:
                    break;
                }

                free(keyname);
                return;
            }
        }
    }
}

/**
 * Catches and sets the next string entered as the given players name.
 *
 * @param pedit ID of the player to rename.
 */
void setNextName(unsigned char pedit)
{
    struct player *p = &players[pedit];
    bool keyDown[322];

    memset(keyDown, '\0', 322);
    memset(p->name, '\0', PLAYER_NAME_LEN);
    displayPConfMenu();
    int chars = 0;

    for (;;) {

        SDL_PollEvent(&event);

        if (chars >= PLAYER_NAME_LEN - 1) {
            playSound(SOUND_BEEP, sound);
            return;
        }

        if (event.type == SDL_KEYDOWN) {

            int k = event.key.keysym.sym;

            if (!keyDown[k]) {
                keyDown[k] = 1;

                if (k >= SDLK_EXCLAIM && k <= SDLK_z) {
                    if (event.key.keysym.mod & KMOD_LSHIFT ||
                        event.key.keysym.mod & KMOD_RSHIFT)
                        snprintf(p->name + chars, PLAYER_NAME_LEN, "%c", k - 32);
                    else
                        snprintf(p->name + chars, PLAYER_NAME_LEN, "%c", k);
                    ++chars;
                } else if (k == SDLK_BACKSPACE && chars > 0) {
                    --chars;
                    snprintf(p->name + chars, PLAYER_NAME_LEN, "%c", '\0');
                }

                displayPConfMenu();
            }

            if (chars > 0 &&
                (k == SDLK_ESCAPE || k == SDLK_RETURN ||
                 k == SDLK_DOWN   || k == SDLK_UP)) {

                playSound(SOUND_BEEP, sound);

                if (k == SDLK_DOWN)
                    menuPConf.choice++;
                else if (k == SDLK_UP)
                    menuPConf.choice = menuPConf.choices - 1;
                return;
            }
        } else if (event.type == SDL_KEYUP)
            keyDown[event.key.keysym.sym] = 0;
    }
}

/**
 * Initializes the hitmap.
 *
 * @param w Width of the map.
 * @param h Height of the map.
 */
void initHitMap(unsigned int w, unsigned int h)
{
    size_t len = sizeof(bool) * w * h;
    hitmap = calloc(len, sizeof(bool));
    recents = NULL;
}

/**
 * Adds a new piece to a queue, later to be added to the hitmap.
 *
 * Also draws the corresponding pixel onto the screen, and detects when
 * players collide. (This function should be considered split up.)
 * updateHitMap() actually puts the pieces into the map.
 *
 * @param x x coordinate of the piece.
 * @param y y coordinate of the piece.
 * @param player ID of the player who owns the piece.
 * @param modifier Used to create special offset values on the piece, for
 * example for hole detection.
 * @see updateHitMap
 */
void addToHitMap(unsigned int x, unsigned int y, unsigned char player,
                 unsigned char modifier)
{
    if (olvl >= O_DEBUG)
        fprintf(stderr, "Added to hitmap: %d, %d...\n", x, y);

    SDL_LockSurface(gameScreen);

    for (int i = -TOLERANCE; i <= TOLERANCE; ++i) {
        for (int j = -TOLERANCE; j <= TOLERANCE; ++j) {

            if (abs(i) + abs(j) > TOLERANCE + 1)
                continue;

            int xpx = x + i;
            int ypx = y + j;

            if (xpx >= 0 && xpx < (int)WINDOW_W && ypx >= 0
                && ypx < (int)WINDOW_H) {

                unsigned char *hit =
                    &hitmap[sizeof(bool) * ((WINDOW_W * ypx) + xpx)];
                struct player *p = &players[player - 1];

                putPixel(xpx, ypx, colors[p->color], gameScreen->pixels);

                if (*hit == 0) {
                    struct recentMapPiece *new
                        = malloc(sizeof(struct recentMapPiece));
                    *hit = player + MAX_PLAYERS + modifier;
                    new->count = COUNTDOWN_TOLERANCE;
                    new->x = xpx;
                    new->y = ypx;
                    new->next = recents;
                    recents = new;
                } else if (*hit != player + MAX_PLAYERS &&
                           *hit != player + MAX_PLAYERS * 2) {
                    if (p->inv_self && player == *hit)
                        break;
                    if (player == *hit) {
                        if (olvl >= O_VERBOSE)
                            printf("Player %d committed suicide!\n", player);
                        killPlayer(player, *hit);
                    } else if (!p->inv_others) {
                        int killer = *hit;
                        while (killer > MAX_PLAYERS) {
                            killer -= MAX_PLAYERS;
                        }
                        if (olvl >= O_VERBOSE)
                            printf("Player %d crashed into Player %d!\n",
                                   player, killer);
                        killPlayer(player, killer);
                    }
                    if (olvl >= O_DEBUG)
                        fprintf(stderr, "Player %d crashed at: (%d, %d)\n",
                                player, xpx, ypx);
                    return;
                }
            } else {
                if (olvl >= O_VERBOSE)
                    printf("Player %d hit the wall!\n", player);
                if (olvl >= O_DEBUG)
                    fprintf(stderr, "Player %d walled at: (%d, %d)\n",
                            player, xpx, ypx);
                killPlayer(player, 0);
                return;
            }
        }
    }

    SDL_UnlockSurface(gameScreen);
}

/**
 * Update the hitmap.
 * Actually puts the pieces from the addToHitMap()-queue into the map.
 * Also handles the creation of holes.
 *
 * @param delta Time since last update.
 * @see addToHitMap
 */
void updateHitMap(Uint32 delta)
{
    if (olvl >= O_DEBUG)
        fprintf(stderr, "Updating hitmap...\n");

    struct recentMapPiece *cur;
    struct recentMapPiece *prev = recents;

    if (prev == NULL || prev->next == NULL)
        return;

    cur = prev->next;
    prev->count -= delta;

    SDL_LockSurface(gameScreen);

    while (cur != NULL) {
        cur->count -= delta;
        unsigned char *at =
            &hitmap[sizeof(bool) * ((WINDOW_W * cur->y) + cur->x)];
        if (holes && cur->count <= HOLE_DELAY && *at > MAX_PLAYERS * 2) {
            *at = 0;
            putPixel(cur->x, cur->y, cMenuBG, gameScreen->pixels);
            prev->next = cur->next;
            free(cur);
            cur = prev->next;
        } else if (cur->count <= 0) {
            *at -= MAX_PLAYERS;
            prev->next = cur->next;
            free(cur);
            cur = prev->next;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }

    SDL_UnlockSurface(gameScreen);
}

/**
 * Empties the hitmap.
 */
void cleanHitMap(void)
{
    if (olvl >= O_DEBUG)
        fprintf(stderr, "Cleaning hitmap...\n");

    struct recentMapPiece *cur = recents;
    struct recentMapPiece *tmp;

    if (cur == NULL)
        return;

    while (cur->next != NULL) {
        tmp = cur->next;
        free(cur);
        cur = tmp;
    }
    free(cur);

    recents = NULL;
}

/**
 * Logic of the main gameplay.
 *
 * @return 1 if something has changed, and needs to be redrawn, 0 else.
 */
int logicGame(void)
{
    Uint32 timenow = SDL_GetTicks();
    Uint32 delta = timenow - prevtime;
    prevtime = timenow;

    if (olvl >= O_DEBUG) {
        fprintf(stderr, "delta: %d    prevtime: %d\n", delta, prevtime);
    }

    if (delta > 35) {
        delta = 35;
    }
    else if (delta == 0) {
        return 0;
    }

    for (int i = 0; i < MAX_PLAYERS; ++i) {
        if (winnerDeclared)
            return 0;

        if (players[i].active) {

            struct player *p = &players[i];

            if (weapons) {
                if (p->wep_time > 0) {
                    p->wep_time -= delta;
                    if (p->wep_time <= 0) {
                        wep_list[p->weapon].func(p, 0);
                        p->wep_time = WEP_NONACTIVE;
                    }
                }

                if (p->inv_self > 0) {
                    p->inv_self -= delta;
                    if (p->inv_self < 0)
                        p->inv_self = 0;
                }

                if (p->inv_others > 0) {
                    p->inv_others -= delta;
                    if (p->inv_others < 0)
                        p->inv_others = 0;
                }
            }

            if (alivecount <= 1) {
                clearSurface(gameScreen);
                newRound();
                return 0;
            } else if (p->alive) {

                unsigned int curx, cury;

                if (weapons && p->wep_time == WEP_NONACTIVE
                        && p->wep_count > 0) {
                    if (p->ai) {
                        if (rand() / (double)RAND_MAX < AI_WEP_PROB) {
                            p->wep_time = wep_list[p->weapon].func(p, 1);
                            p->wep_count -= 1;
                            refreshGameScreen();
                        }
                    } else if (buttonDown(p->wkey)) {
                        p->wep_time = wep_list[p->weapon].func(p, 1);
                        p->wep_count -= 1;
                        clearButton(p->wkey);
                        refreshGameScreen();
                    }
                }

                if (p->ai) {

                    /* quick and dirty ai-debugging. This should never
                     * be turned on */
                    /*
                      refreshGameScreen();
                      int checkx = p->posx + 40*cos(p->dir + M_PI_2);
                      int checky = p->posy + 40*sin(p->dir + M_PI_2);
                      SDL_Rect a = {checkx, checky, 4, 4};
                      SDL_FillRect(screen, &a, SDL_MapRGB(screen->format,
                      0x55, 0x77, 0x99));
                      checkx = p->posx + 40*cos(p->dir - M_PI_2);
                      checky = p->posy + 40*sin(p->dir - M_PI_2);
                      SDL_Rect b = {checkx, checky, 4, 4};
                      SDL_FillRect(screen, &b, SDL_MapRGB(screen->format,
                      0x55, 0x77, 0x99));
                      checkx = p->posx + 85 * cos(p->dir);
                      checky = p->posy + 85 * sin(p->dir);
                      SDL_Rect d = {checkx, checky, 4, 4};
                      SDL_FillRect(screen, &d, SDL_MapRGB(screen->format,
                      0x55, 0x77, 0x99));
                      checkx = p->posx + 80 * cos(p->dir);
                      checky = p->posy + 80 * sin(p->dir);
                      SDL_Rect e = {checkx, checky, 4, 4};
                      SDL_FillRect(screen, &e, SDL_MapRGB(screen->format,
                      0xFF, 0x30, 0x30));
                      checkx = p->posx + 60*cos(p->dir + M_PI_4);
                      checky = p->posy + 60*sin(p->dir + M_PI_4);
                      SDL_Rect f = {checkx, checky, 4, 4};
                      SDL_FillRect(screen, &f, SDL_MapRGB(screen->format,
                      0x66, 0x66, 0x66));
                      checkx = p->posx + 60*cos(p->dir - M_PI_4);
                      checky = p->posy + 60*sin(p->dir - M_PI_4);
                      SDL_Rect g = {checkx, checky, 4, 4};
                      SDL_FillRect(screen, &g, SDL_MapRGB(screen->format,
                      0x66, 0x66, 0x66));
                    */

                    char c = pollAi(p->posx, p->posy, p->dir, p->active,
                                    hitmap, WINDOW_W, WINDOW_H);
                    if (activeTron) {
                        if (c == 'l')
                            p->dir -= M_PI_2;
                        else if (c == 'r')
                            p->dir += M_PI_2;
                    } else {
                        if (c == 'l')
                            p->dir -= 0.0022 * delta * p->speed;
                        else if (c == 'r')
                            p->dir += 0.0022 * delta * p->speed;
                    }
                } else {
                    if (activeTron) {
                        /* Screw smooth angles, TRON MODE from here on! */
                        if (keyDown[p->lkey]) {
                            keyDown[p->lkey] = 0;
                            p->dir -= M_PI_2;
                        } else if (keyDown[p->rkey]) {
                            keyDown[p->rkey] = 0;
                            p->dir += M_PI_2;
                        }
                    } else {
                        if (buttonDown(p->lkey)) {
                            p->dir -= 0.0022 * delta * p->speed;
                        }
                        else if (buttonDown(p->rkey)) {
                            p->dir += 0.0022 * delta * p->speed;
                        }
                    }
                }

                p->posx += ZATA_SPEED * cos(p->dir) * delta * p->speed;
                p->posy += ZATA_SPEED * sin(p->dir) * delta * p->speed;

                curx = (unsigned int)p->posx;
                cury = (unsigned int)p->posy;

                if (holes) {
                    p->holecount += delta;
                    if (p->holecount > HOLE_FREQ)
                        p->holecount = 0;
                }

                if (p->prevx != curx || p->prevy != cury) {
                    if (olvl >= O_DEBUG)
                        fprintf(stderr, "Adding to hitmap: %d, %d...\n",
                                p->prevx, p->prevy);
                    if (holes && p->holecount > HOLE_SIZE)
                        addToHitMap(p->prevx, p->prevy, p->active,
                                    MAX_PLAYERS);
                    else
                        addToHitMap(p->prevx, p->prevy, p->active, 0);
                    p->prevx = curx;
                    p->prevy = cury;
                }
            }
        } else {
            break;
        }
    }
    updateHitMap(delta);
    SDL_UnlockSurface(gameScreen);
    return 1;
}

/**
 * Display a game round.
 */
void displayGame(void)
{
    clearSurface(screen);

    SDL_BlitSurface(gameScreen, NULL, screen, NULL);

    if (SDL_Flip(screen) == -1) {
        exit(1);
    }
}

/**
 * Logic of the startup phase of the game.
 *
 * @return 1 if something has changed, and needs to be redrawn, 0 else.
 */
int logicGameStart(void)
{
    Uint32 timenow = SDL_GetTicks();
    Uint32 delta = timenow - prevtime;
    prevtime = timenow;

    if (olvl >= O_DEBUG)
        fprintf(stderr, "delta: %d    prevtime: %d\n", delta, prevtime);

    if (delta > 13)
        delta = 13;

    if (countdown > 0)
        countdown -= delta;
    else
        curScene = &game;

    return 1;
}

/**
 * Display blinking arrows at the beginning of a game.
 */
void displayGameStart(void)
{
    clearSurface(screen);
    clearSurface(gameScreen);

    drawExtras();
    SDL_BlitSurface(gameScreen, NULL, screen, NULL);

    if ((countdown % (START_ROUND_WAIT / 4)) > (START_ROUND_WAIT / 8)) {
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            if (players[i].active) {
                struct player *p = &players[i];
                SDL_Rect offset = {(int)p->posx - 8, (int)p->posy - 8,
                                   0, 0};
                int diri = (int)((p->dir) * (32.0 / (2.0 * M_PI)));
                SDL_BlitSurface(p->arrow, &arrowClip[diri], screen,
                                &offset);
            } else {
                break;
            }
        }
    }

    if (SDL_Flip(screen) == -1)
        exit(1);
}

/**
 * Redraws the whole game screen.
 * Should only be called as a last resort when the whole screen needs to
 * be redrawn.
 */
void refreshGameScreen(void)
{
    SDL_UnlockSurface(gameScreen);
    clearSurface(gameScreen);

    drawExtras();

    SDL_LockSurface(gameScreen);

    unsigned char *target = gameScreen->pixels;

    for (unsigned int yy = 0; yy < WINDOW_H; ++yy) {
        for (unsigned int xx = 0; xx < WINDOW_W; ++xx, target += 4) {
            char charat = hitmap[sizeof(bool)
                                 * ((WINDOW_W * yy) + xx)];
            if (charat != 0) {
                struct player *p;
                if (charat > MAX_PLAYERS) {
                    if (charat > MAX_PLAYERS * 2) {
                        p = &players[charat - (MAX_PLAYERS * 2) - 1];
                    } else {
                        p = &players[charat - MAX_PLAYERS - 1];
                    }
                } else {
                    p = &players[charat - 1];
                }
                target[0] = (&colors[p->color])->b;
                target[1] = (&colors[p->color])->g;
                target[2] = (&colors[p->color])->r;
            }
        }
    }

    SDL_UnlockSurface(gameScreen);
}

/**
 * Draws the game scores onto the screen.
 */
void drawExtras(void)
{
    char score_str[SCORE_BUF];
    SDL_Surface *scoreText;

    for (int i = 0; i < nPlayers; ++i) {
        struct player *p = &players[i];

        SDL_Rect offset = {7, SCORE_SPACING * i, 0, 0};

        snprintf(score_str, SCORE_BUF, "%d", p->score);
        scoreText = TTF_RenderUTF8_Shaded(fonts[FONT_SCORE], score_str,
                                          colors[p->color], cMenuBG);
        SDL_BlitSurface(scoreText, NULL, gameScreen, &offset);
        SDL_FreeSurface(scoreText);

        if (weapons && p->wep_count > 0) {
            offset.y += 2;
            offset.x += WEP_SMALL_INIT_OFFSET;

            for (int j = 0; j < p->wep_count; ++j) {
                offset.x += WEP_SMALL_PADDING;
                SDL_BlitSurface(smallWepIcons[p->weapon], NULL, gameScreen,
                                &offset);
            }
        }
    }

    if (broadcasts) {
        for (int i = 0; i < BROADC_LIMIT; ++i) {
            if (broadcast[i] != NULL) {
                SDL_Rect offset = {WINDOW_W - broadcast[i]->w - 4,
                                   WINDOW_H - (broadcast[i]->h * (i + 1)),
                                   0, 0};
                SDL_BlitSurface(broadcast[i], NULL, gameScreen, &offset);
            }
        }
    }

    if (border) {
        Uint32 white = SDL_MapRGB(gameScreen->format, 0xFF, 0xFF, 0xFF);
        SDL_Rect leftBorder = {0, 0, 1, WINDOW_H};
        SDL_Rect rightBorder = {WINDOW_W - 1, 0, WINDOW_W, WINDOW_H};
        SDL_Rect topBorder = {0, 0, WINDOW_W, 1};
        SDL_Rect bottomBorder = {0, WINDOW_H - 1, WINDOW_W, WINDOW_H};
        SDL_FillRect(gameScreen, &leftBorder, white);
        SDL_FillRect(gameScreen, &rightBorder, white);
        SDL_FillRect(gameScreen, &topBorder, white);
        SDL_FillRect(gameScreen, &bottomBorder, white);
    }

    /* Active weapons */
    if (activeFreeze || activeConfusion || activeTron || activeChilirun) {
        SDL_Rect offset = {WINDOW_W - wepIcons[0]->w, 0, 0, 0};
        if (activeFreeze)
            SDL_BlitSurface(wepIcons[WEP_FROSTWAVE + 1], NULL, gameScreen, &offset);
        if (activeConfusion)
            SDL_BlitSurface(wepIcons[WEP_CONFUSION + 1], NULL, gameScreen, &offset);
        if (activeTron)
            SDL_BlitSurface(wepIcons[WEP_TRON + 1], NULL, gameScreen, &offset);
        if (activeChilirun)
            SDL_BlitSurface(wepIcons[WEP_CHILI_RUN + 1], NULL, gameScreen, &offset);
    }
}

/**
 * Starts a new round.
 */
void newRound(void)
{
    cleanHitMap();

    alivecount = nPlayers;
    countdown = START_ROUND_WAIT;
    memset(hitmap, '\0', sizeof(bool) * WINDOW_W * WINDOW_H);

    curScene = &gameStart;

    if (olvl >= O_VERBOSE)
        printf(" -- New round! --  ( Score: ");

    if (weapons)
        resetWeapons();
    winnerDeclared = false;

    for (int i = 0; i < MAX_PLAYERS; ++i) {
        struct player *p = &players[i];
        if (p->active) {
            if (olvl >= O_VERBOSE)
                printf("%d ", p->score);
            if (duelmode)
                drespawn(p);
            else
                respawn(p);
        } else {
            if (olvl >= O_VERBOSE)
                printf(")\n");
            return;
        }

        p->inv_self = 0;
        p->inv_others = 0;
    }

    if (olvl >= O_VERBOSE)
        printf(")\n");
}

/**
 * Ends the current round.
 */
void endRound(void)
{
    cleanHitMap();
    if (broadcasts)
        cleanBroadcasts();
    if (weapons)
        resetWeapons();
}

/**
 * Lets the AI pick weapons at random.
 */
void assignAiWeapons(void)
{
    for (int i = 0; i < nPlayers; ++i) {
        struct player *p = &players[i];
        if (p->ai) {
            double rnd = (double)rand() / RAND_MAX;
            p->weapon = (int)(rnd * N_WEAPONS);
        }
    }
}

/**
 * Negates all active weapon effects.
 */
void resetWeapons(void)
{
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        struct player *p = &players[i];
        if (p->active) {

            p->wep_count = wep_list[p->weapon].charges;

            if (p->wep_time != WEP_NONACTIVE) {
                wep_list[p->weapon].func(p, 0);
                p->wep_time = WEP_NONACTIVE;
            }
        }
    }
}

/**
 * Initializes the main menu.
 */
void initMainMenu(void)
{
    colorBalls();
}

/**
 * Logic for the main menu.
 *
 * @return 1 if something has changed, and needs to be redrawn, 0 else.
 */
int logicMainMenu(void)
{
    if (enterButtonDown()) {
        switch (menuMain.choice) {
        case 0:
            playSound(SOUND_BEEP, sound);
            initPlayers2();
            newRound();
            if (weapons) {
                assignAiWeapons();
                curScene = &wepMenu;
            } else {
                curScene = &gameStart;
            }
            break;
        case 1:
            playSound(SOUND_BEEP, sound);
            menuSettings.choice = 0;
            curScene = &settingsMenu;
            break;
        case 2:
            exitGame(0);
            break;
        default:
            break;
        }
        clearEnterButtons();
        return 1;
    } else if (menuButtonQuery(KEY_LEFT)) {
        if (menuMain.choice == 0 && nPlayers > MIN_PLAYERS) {
            --nPlayers;
            playSound(nPlayers - 1, sound);
            deselectWeapons();
        }
        return 1;
    } else if (menuButtonQuery(KEY_RIGHT)) {
        if (!duelmode && menuMain.choice == 0 && nPlayers < MAX_PLAYERS) {
            ++nPlayers;
            playSound(nPlayers - 1, sound);
        }
        return 1;
    }
    return handleMenu(&menuMain);
}

/**
 * Displays the main menu screen.
 */
void displayMainMenu(void)
{
    char *c[menuMain.choices];
    c[0] = "START GAME";
    c[1] = "SETTINGS";
    c[2] = "EXIT";

    SDL_Surface *logo = images[IMG_LOGO];
    int ymod = logo->h / 2;
    displayMenu(c, &menuMain, ymod);
    SDL_Rect offset = {WINDOW_W / 2 - logo->w / 2, WINDOW_H / 2 - logo->h,
                       0, 0};
    SDL_BlitSurface(logo, NULL, screen, &offset);

    /* This could/should be made smoother... */
    for (int i = 0; i < nPlayers; ++i) {
        offset.x = (WINDOW_W / 2)            /* window offset */
            - 60                             /* temp. offset */
            + (i - nPlayers) * BALL_SPACING; /* player modifier */

        offset.y = (WINDOW_H / 2)            /* window offset */
            - BALL_Y_MOD + ymod;             /* temp. offset */

        SDL_BlitSurface(pballs[i], NULL, screen, &offset);
    }

    for (int i = nPlayers; i < MAX_PLAYERS; ++i) {
        offset.x = (WINDOW_W / 2)            /* window offset */
            + 68                             /* temp. offset */
            + (i - nPlayers) * BALL_SPACING; /* player modifier */

        offset.y = (WINDOW_H / 2)            /* window offset */
            - BALL_Y_MOD + ymod;             /* temp. offset */

        SDL_BlitSurface(pballs[MAX_PLAYERS], NULL, screen, &offset);
    }

    if (SDL_Flip(screen) == -1)
        exit(1);
}

/**
 * Logic of the weapon selection menu.
 *
 * @return 1 if something has changed, and needs to be redrawn, 0 else.
 */
int logicWepMenu(void)
{
    int i;
    struct player *p;

    if (enterButtonDown()) {
        resetWeapons();
        playSound(SOUND_ROUND_BEGIN, sound);
        curScene = &gameStart;
    }

    for (p = &players[0], i = 0; i < nPlayers; ++i, ++p) {
        if (buttonDown(p->lkey)) {
            if (p->weapon == 0) {
                p->weapon = N_WEAPONS - 1;
            }
            else {
                p->weapon--;
            }
            clearButton(p->lkey);
            return 1;
        }
        else if (buttonDown(p->rkey)) {
            if (p->weapon == N_WEAPONS - 1) {
                p->weapon = 0;
            }
            else {
                p->weapon++;
            }
            clearButton(p->rkey);
            return 1;
        }
    }
    return 0;
}

/**
 * Displays the weapon selection menu.
 */
void displayWepMenu(void)
{
    struct player *p;
    int mid = -N_WEAPONS/2;
    clearSurface(screen);

    char wep_str[MENU_BUF];
    SDL_Surface *wep_text;

    /* Draw weapon description strings */
    for (int i = 0; i < nPlayers; ++i) {
        p = &players[i];
        struct weapon w = wep_list[p->weapon];

        SDL_Rect offset = {(i % 4) * (screen->w / 4) + 10,
                           i > 3 ? screen->h - 118 : 10, 0, 0};

        snprintf(wep_str, PLAYER_NAME_LEN, "%s", p->name);
        wep_text = TTF_RenderUTF8_Shaded(fonts[FONT_SCORE], wep_str,
                                         colors[p->color], cMenuBG);
        SDL_BlitSurface(wep_text, NULL, screen, &offset);
        SDL_FreeSurface(wep_text);

        offset.y += 30;
        snprintf(wep_str, MENU_BUF, "%s", w.name);
        wep_text = TTF_RenderUTF8_Shaded(fonts[FONT_SCORE], wep_str,
                                         colors[p->color], cMenuBG);
        SDL_BlitSurface(wep_text, NULL, screen, &offset);
        SDL_FreeSurface(wep_text);

        offset.y += 30;
        snprintf(wep_str, MENU_BUF, "%s", w.desc1);
        wep_text = TTF_RenderUTF8_Shaded(fonts[FONT_BROADCAST_BOLD], wep_str,
                                         colors[p->color], cMenuBG);
        SDL_BlitSurface(wep_text, NULL, screen, &offset);
        SDL_FreeSurface(wep_text);

        if (w.desc2[0] != '\0') {
            offset.y += FONT_SIZE_BROADCAST + 2;
            snprintf(wep_str, MENU_BUF, "%s", w.desc2);
            wep_text = TTF_RenderUTF8_Shaded(fonts[FONT_BROADCAST_BOLD],
                                             wep_str, colors[p->color],
                                             cMenuBG);
            SDL_BlitSurface(wep_text, NULL, screen, &offset);
            SDL_FreeSurface(wep_text);
        }

        if (i > 3)
            offset.y = screen->h - 10 - FONT_SIZE_BROADCAST;
        else
            offset.y = 105;
        snprintf(wep_str, MENU_BUF, "Charges: %d", w.charges);
        wep_text = TTF_RenderUTF8_Shaded(fonts[FONT_BROADCAST_BOLD], wep_str,
                                         colors[p->color], cMenuBG);
        SDL_BlitSurface(wep_text, NULL, screen, &offset);
        SDL_FreeSurface(wep_text);
    }

    /* Draw weapon icons */
    for (int i = 0; i < N_WEAPONS; ++i, ++mid) {
        SDL_Rect offset = {
            (WINDOW_W / 2)          /* offset */
            - (wepIcons[0]->w / 2)  /* centralize */
            + (mid * ((wepIcons[0]->w / 2) + WEP_SPACEMOD))
            - ((N_WEAPONS % 2) - 1)
            * (WEP_SPACEMOD / 1.2)
            ,
            (WINDOW_H / 2)          /* offset */
            - (wepIcons[0]->h / 2)  /* centralize */
            , 0, 0
        };

        SDL_BlitSurface(wepIcons[0], NULL, screen, &offset);
        SDL_BlitSurface(wepIcons[i + 1], NULL, screen, &offset);

        /* Draw weapon selection arrows */
        offset.y -= PARROWSELECT_MOD_Y;
        for (int j = 0; j < nPlayers; ++j) {
            if (j == MAX_PLAYERS / 2) {
                offset.y += wepIcons[0]->h + PARROWSELECT_MOD_Y * 1.2;
                offset.x -= PARROWSELECT_MOD_X * (MAX_PLAYERS / 2);
            }
            p = &players[j];
            if (j != 0) offset.x += PARROWSELECT_MOD_X;
            if (p->weapon == i) {
                int diri;
                if (j >= MAX_PLAYERS / 2) {
                    diri = (int)(3*M_PI_2 * (32 / (2*M_PI)));
                } else {
                    diri = (int)(M_PI_2 * (32 / (2*M_PI)));
                }
                SDL_BlitSurface(p->arrow, &arrowClip[diri], screen,
                                &offset);
            }
        }
    }

    if (SDL_Flip(screen) == -1)
        exit(1);
}

/**
 * Logic of the settings menu.
 *
 * @return 1 if something has changed, and needs to be redrawn, 0 else.
 */
int logicSettingsMenu(void)
{
    if (enterButtonDown()) {
        switch (menuSettings.choice) {
        case 0: /* Toggle fullscreen */
            playSound(SOUND_BEEP, sound);
            fullscreen ^= 1;
            initScreen();
            initHitMap(WINDOW_W, WINDOW_H);
            break;
        case 1: /* Toggle sound */
            sound ^= 1;
            playSound(SOUND_BEEP, sound);
            break;
        case 2: /* Toggle music */
            music ^= 1;
            if (music)
                playBGM();
            else
                stopBGM();
            break;
        case 3: /* Toggle weapons */
            playSound(SOUND_BEEP, sound);
            weapons ^= 1;
            break;
        case 4: /* Toggle holes */
            playSound(SOUND_BEEP, sound);
            holes ^= 1;
            break;
        case 5: /* Toggle broadcasts */
            playSound(SOUND_BEEP, sound);
            broadcasts ^= 1;
            break;
        case 6: /* Toggle border visibility */
            playSound(SOUND_BEEP, sound);
            border ^= 1;
            break;
        case 7: /* Toggle duel mode */
            playSound(SOUND_BEEP, sound);
            duelmode ^= 1;
            if (duelmode)
                nPlayers = 2;
            break;
        case 8: /* Score cap */
            playSound(SOUND_BEEP, sound);
            if (scorecap < SCORE_CAP_MAX - 10)
                scorecap += 10;
            break;
        case 9:
            playSound(SOUND_BEEP, sound);
            menuPlayer.choice = 0;
            curScene = &playerMenu;
            break;
        case 10: /* Back */
            playSound(SOUND_BEP, sound);
            clearEnterButtons();
            initMainMenu();
            curScene = curScene->parentScene;
            break;
        default:
            break;
        }
        clearEnterButtons();
        return 1;
    }
    else if (menuButtonQuery(KEY_LEFT)) {
        if (menuSettings.choice == 8 && scorecap > 0) {
            playSound(SOUND_BEP, sound);
            --scorecap;
        }
        return 1;
    }
    else if (menuButtonQuery(KEY_RIGHT)) {
        if (menuSettings.choice == 8 && scorecap < SCORE_CAP_MAX) {
            playSound(SOUND_BEEP, sound);
            ++scorecap;
        }
        return 1;
    }
    else if (keyDown[SDLK_BACKSPACE]) {
        keyDown[SDLK_BACKSPACE] = 0;
        if (menuSettings.choice == 8) {
            playSound(SOUND_BEP, sound);
            scorecap = 0;
        }
        return 1;
    }

    /* Special case for the score setting */
    if (menuSettings.choice == 8) {
        int num = -1;
        if      (keyDown[SDLK_0]) num = 0;
        else if (keyDown[SDLK_1]) num = 1;
        else if (keyDown[SDLK_2]) num = 2;
        else if (keyDown[SDLK_3]) num = 3;
        else if (keyDown[SDLK_4]) num = 4;
        else if (keyDown[SDLK_5]) num = 5;
        else if (keyDown[SDLK_6]) num = 6;
        else if (keyDown[SDLK_7]) num = 7;
        else if (keyDown[SDLK_8]) num = 8;
        else if (keyDown[SDLK_9]) num = 9;

        if (num != -1) {
            /* Multiply the scorecap by 10 and add the number */
            scorecap = scorecap * 10 + num;
            playSound(SOUND_BEEP, sound);

            keyDown[SDLK_0] = keyDown[SDLK_1] = keyDown[SDLK_2] = keyDown[SDLK_3] =
                keyDown[SDLK_4] = keyDown[SDLK_5] = keyDown[SDLK_6] =
                keyDown[SDLK_7] = keyDown[SDLK_8] = keyDown[SDLK_9] = 0;

            return 1;
        }
    }

    return handleMenu(&menuSettings);
}

/**
 * Display the settings menu screen.
 */
void displaySettingsMenu(void)
{
    char *c[menuSettings.choices];
    char tmpCap[SCORE_BUF];

    char s1[MENU_BUF] = "FULLSCREEN ";
    char s2[MENU_BUF] = "SOUND EFFECTS ";
    char s3[MENU_BUF] = "MUSIC ";
    char s4[MENU_BUF] = "WEAPONS ";
    char s5[MENU_BUF] = "HOLES ";
    char s6[MENU_BUF] = "BROADCASTS ";
    char s7[MENU_BUF] = "SHOW BORDER ";
    char s8[MENU_BUF] = "DUEL MODE ";
    char s9[MENU_BUF] = "SCORE CAP: ";
    strncat(s1, fullscreen ON_OFF, MENU_BUF - strlen(s1) - 1);
    strncat(s2, sound ON_OFF, MENU_BUF - strlen(s2) - 1);
    strncat(s3, music ON_OFF, MENU_BUF - strlen(s3) - 1);
    strncat(s4, weapons ON_OFF, MENU_BUF - strlen(s4) - 1);
    strncat(s5, holes ON_OFF, MENU_BUF - strlen(s5) - 1);
    strncat(s6, broadcasts ON_OFF, MENU_BUF - strlen(s6) - 1);
    strncat(s7, border ON_OFF, MENU_BUF - strlen(s7) - 1);
    strncat(s8, duelmode ON_OFF, MENU_BUF - strlen(s8) - 1);
    if (scorecap == 0) {
        strncat(s9, "∞", MENU_BUF - strlen(s9) - 1);
    } else {
        snprintf(tmpCap, SCORE_BUF, "%d", scorecap);
        strncat(s9, tmpCap, MENU_BUF - strlen(s9) - 1);
    }
    c[0] = s1;
    c[1] = s2;
    c[2] = s3;
    c[3] = s4;
    c[4] = s5;
    c[5] = s6;
    c[6] = s7;
    c[7] = s8;
    c[8] = s9;
    c[9] = "PLAYER CONFIG";
    c[10] = "BACK";

    displayMenu(c, &menuSettings, 0);

    if (SDL_Flip(screen) == -1)
        exit(1);
}

/**
 * Logic for the player menu.
 *
 * @return 1 if something has changed, and needs to be redrawn, 0 else.
 */
int logicPlayerMenu(void)
{
    if (enterButtonDown()) {
        if (menuPlayer.choice == menuPlayer.choices - 1) {
            playSound(SOUND_BEP, sound);
            curScene = curScene->parentScene;
        } else {
            playSound(SOUND_BEEP, sound);
            editPlayer = menuPlayer.choice;
            menuPConf.choice = 0;
            curScene = &pConfMenu;
        }
        clearEnterButtons();
        return 1;
    }
    return handleMenu(&menuPlayer);
}

/**
 * Display the player menu screen.
 */
void displayPlayerMenu(void)
{
    char *c[menuPlayer.choices];
    c[0] = "PLAYER 1 CONFIG";
    c[1] = "PLAYER 2 CONFIG";
    c[2] = "PLAYER 3 CONFIG";
    c[3] = "PLAYER 4 CONFIG";
    c[4] = "PLAYER 5 CONFIG";
    c[5] = "PLAYER 6 CONFIG";
    c[6] = "PLAYER 7 CONFIG";
    c[7] = "PLAYER 8 CONFIG";
    c[8] = "BACK";

    displayMenu(c, &menuPlayer, 0);

    if (SDL_Flip(screen) == -1)
        exit(1);
}

/**
 * Logic for the player configuration.
 *
 * @return 1 if something has changed, and needs to be redrawn, 0 else.
 */
int logicPConfMenu(void)
{
    if (enterButtonDown()) {
        switch (menuPConf.choice) {
        case 0:
            // Disable name input with joystick for the time being.
            if (keyDown[SDLK_SPACE] || keyDown[SDLK_RETURN]) {
                playSound(SOUND_BEEP, sound);
                setNextName(editPlayer);
            }
            break;
        case 1: /* Toggle AI */
            playSound(SOUND_BEEP, sound);
            (&players[editPlayer])->ai ^= 1;
            break;
        case 2:
            playSound(SOUND_BEEP, sound);
            (&players[editPlayer])->lkey = SDLK_CLEAR;
            displayPConfMenu(); /* Update menu before catching key */
            setNextKey(editPlayer, 'l');
            break;
        case 3:
            playSound(SOUND_BEEP, sound);
            (&players[editPlayer])->wkey = SDLK_CLEAR;
            displayPConfMenu(); /* Update menu before catching key */
            setNextKey(editPlayer, 'w');
            break;
        case 4:
            playSound(SOUND_BEEP, sound);
            (&players[editPlayer])->rkey = SDLK_CLEAR;
            displayPConfMenu(); /* Update menu before catching key */
            setNextKey(editPlayer, 'r');
            break;
        case 5: /* Reset player settings */
            playSound(SOUND_BEP, sound);
            resetPlayer(editPlayer);
            break;
        case 6:
            playSound(SOUND_BEP, sound);
            curScene = curScene->parentScene;
            break;
        default:
            break;
        }
        clearEnterButtons();
        return 1;
    } else if (menuPConf.choice == 0) {
        if (menuButtonQuery(KEY_LEFT)) {
            playSound(SOUND_BEEP, sound);
            setColor(editPlayer, 0);
            return 1;
        } else if (menuButtonQuery(KEY_RIGHT)) {
            playSound(SOUND_BEEP, sound);
            setColor(editPlayer, 1);
            return 1;
        } else if (keyDown[SDLK_BACKSPACE]) {
            keyDown[SDLK_BACKSPACE] = 0;
            playSound(SOUND_BEEP, sound);
            setNextName(editPlayer);
            return 1;
        }
    }
    return handleMenu(&menuPConf);
}

/**
 * Display the player configuration screen.
 */
void displayPConfMenu(void)
{
    char *c[menuPConf.choices];

    char s1[MENU_BUF] = "";
    char s2[MENU_BUF] = "AI ";
    char s3[MENU_BUF] = "LEFT KEY: ";
    char s4[MENU_BUF] = "WEAPON KEY: ";
    char s5[MENU_BUF] = "RIGHT KEY: ";

    char tmpKey[BUTTON_NAME_MAX_LEN];
    struct player *p = (&players[editPlayer]);
    char *lkey = buttonName(p->lkey);
    char *rkey = buttonName(p->rkey);
    char *wkey = buttonName(p->wkey);
    snprintf(s1, MENU_BUF, "%s",
             p->name[0] == '\0' ? "< enter name >" : p->name);

    strncat(s2, p->ai ON_OFF, MENU_BUF - strlen(s2) - 1);

    snprintf(tmpKey, BUTTON_NAME_MAX_LEN*sizeof(char), "%s", lkey);
    free(lkey);
    strncat(s3, tmpKey, MENU_BUF - strlen(s3) - 1);

    snprintf(tmpKey, BUTTON_NAME_MAX_LEN*sizeof(char), "%s", wkey);
    free(wkey);
    strncat(s4, tmpKey, MENU_BUF - strlen(s4) - 1);

    snprintf(tmpKey, BUTTON_NAME_MAX_LEN*sizeof(char), "%s", rkey);
    free(rkey);
    strncat(s5, tmpKey, MENU_BUF - strlen(s5) - 1);

    c[0] = s1;
    c[1] = s2;
    c[2] = s3;
    c[3] = s4;
    c[4] = s5;
    c[5] = "RESET";
    c[6] = "BACK";

    displayMenu(c, &menuPConf, 0);

    if (SDL_Flip(screen) == -1)
        exit(1);
}

/**
 * Handles a general menu.
 *
 * @param m The menu to do handling of.
 */
int handleMenu(struct menu *m)
{
    if (menuButtonQuery(KEY_DOWN)) {
        playSound(SOUND_BEEP, sound);
        ++(m->choice);
        return 1;
    }
    else if (menuButtonQuery(KEY_UP)) {
        playSound(SOUND_BEEP, sound);
        --(m->choice);
        return 1;
    }

    if (m->choice >= m->choices) {
        m->choice = 0;
        return 1;
    }
    else if (m->choice < 0) {
        m->choice = m->choices - 1;
        return 1;
    }

    return 0;
}

/**
 * Displays a general menu.
 * The menu structure is separated from the strings, so that arbitrary
 * strings can be used.
 *
 * @param c All the strings to be shown in the menu.
 * @param m The menu to be shown.
 * @param ymod Amount of y-coordinate shifting.
 */
void displayMenu(char *c[], struct menu *m, int ymod)
{
    int mid = -(m->choices / 2);
    clearSurface(screen);

    for (int i = 0; i < m->choices; ++i, ++mid) {

        if (i == m->choice) {
            msg = TTF_RenderUTF8_Shaded(fonts[FONT_MENU_BOLD], c[i],
                                        i == 0 && curScene == &pConfMenu
                                        ? colors[(&players[editPlayer])->color]
                                        : cMenuTextH, cMenuBG);
        } else {
            msg = TTF_RenderUTF8_Solid(fonts[FONT_MENU], c[i],
                                       i == 0 && curScene == &pConfMenu
                                       ? colors[(&players[editPlayer])->color]
                                       : cMenuText);
        }

        SDL_Rect offset = {
            (WINDOW_W / 2)                      /* offset */
            - (msg->w / 2)                      /* centralize */
            ,
            (WINDOW_H / 2)                      /* offset */
            + (mid * ((msg->h / 2) + SPACEMOD)) /* spacing */
            - ((m->choices % 2) - 1)            /* halfspace */
            * (SPACEMOD / 2)
            - (msg->h / 2)                      /* centralize */
            + ymod                              /* eventual offset */
            , 0, 0
        };

        SDL_BlitSurface(msg, NULL, screen, &offset);
        SDL_FreeSurface(msg);
    }
}

/**
 * Puts a pixel on a surface.
 *
 * @param x x coordinate of the pixel destination.
 * @param y y coordinate of the pixel destination.
 * @param c Desired color of the pixel.
 * @param target Points to which SDL_Surface the pixel should be put.
 */
void putPixel(int x, int y, SDL_Color c, unsigned char *target)
{
    target += 4 * ((WINDOW_W * y) + x);
    target[0] = c.b;
    target[1] = c.g;
    target[2] = c.r;
}

/**
 * Fills a surface with a color.
 * All white pixels in the surface will be replaced with the specified
 * color.
 *
 * @param c Color to fill the surface with.
 * @param sprite Surface to be filled.
 */
void colorFill(SDL_Color c, SDL_Surface *sprite)
{
    unsigned char *target = sprite->pixels;

    for (int yy = 0; yy < sprite->h; ++yy) {
        for (int xx = 0; xx < sprite->w; ++xx, target += 4) {
            target[0] *= c.b / 255.0;
            target[1] *= c.g / 255.0;
            target[2] *= c.r / 255.0;
        }
    }
}

/**
 * Initializes the starter colors.
 */
void initColors(void)
{
    SDL_Color *c = &colors[0];
    c->r = 0xFF; c->g = 0x00; c->b = 0x00; ++c; /* Red */
    c->r = 0x00; c->g = 0x6F; c->b = 0xFF; ++c; /* Blue */
    c->r = 0x00; c->g = 0xFF; c->b = 0x00; ++c; /* Green */
    c->r = 0xFF; c->g = 0xFF; c->b = 0x00; ++c; /* Yellow */
    c->r = 0xFF; c->g = 0x00; c->b = 0xFF; ++c; /* Pink */
    c->r = 0x00; c->g = 0xFF; c->b = 0xFF; ++c; /* Cyan */
    c->r = 0xFF; c->g = 0xFF; c->b = 0xFF; ++c; /* White */
    c->r = 0xFF; c->g = 0x80; c->b = 0x00; ++c; /* Orange */
}

/**
 * Colors the balls for the main menu.
 */
void colorBalls(void)
{
    SDL_Color inactive = {127, 127, 127, 0};
    SDL_Surface **s = pballs;
    struct player *p = &players[0];
    for (int i = 0; i < MAX_PLAYERS; ++i, ++p, ++s) {
        SDL_BlitSurface(images[IMG_BALL], NULL, *s, NULL);
        SDL_LockSurface(*s);
        colorFill(colors[p->color], *s);
        SDL_UnlockSurface(*s);
    }

    SDL_BlitSurface(images[IMG_BALL], NULL, *s, NULL);
    SDL_LockSurface(*s);
    colorFill(inactive, *s);
    SDL_UnlockSurface(*s);
}

/**
 * Initializes SDL.
 *
 * @return 1 if the initialization was successful, 0 if not.
 */
int init(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        if (olvl >= O_NORMAL)
            fprintf(stderr, "ERROR: Could not initialize SDL.\n");
        return 0;
    }

    if (atexit(SDL_Quit) != 0)
        return 0;

    openJoysticks();
    atexit(closeJoysticks);

    if (initSound() == -1) {
        if (olvl >= O_NORMAL)
            fprintf(stderr, "ERROR: Could not initialize sound.\n");
        return 0;
    }

    if (TTF_Init() == -1) {
        if (olvl >= O_NORMAL)
            fprintf(stderr, "ERROR: Could not initialize fonts.\n");
        return 0;
    }
    atexit(TTF_Quit);

    cleanBroadcasts();
    srand(SDL_GetTicks());

    SDL_ShowCursor(SDL_DISABLE);
    SDL_WM_SetCaption("Zatacka X", "Zatacka X");
    SDL_Surface *icon = loadIcon("icon.bmp");
    SDL_WM_SetIcon(icon, NULL);
    SDL_FreeSurface(icon);

    return 1;
}

/**
 * Make miscellaneous graphics preparations, such as sprite clipping
 * and duplication.
 */
void initGraphics(void)
{
    SDL_Surface **p;

    /* Clip arrow sprite sheet */
    SDL_Surface *arrows = images[IMG_ARROWS];
    for (int y = 0, i = 0; y < arrows->h; y += 16) {
        for (int x = 0; x < arrows->w; x += 16, ++i) {
            arrowClip[i].x = x;
            arrowClip[i].y = y;
            arrowClip[i].w = 16;
            arrowClip[i].h = 16;
        }
    }

    /* Make arrow copies */
    parrows = malloc(MAX_PLAYERS * sizeof(SDL_Surface));
    p = parrows;
    for (int i = 0; i < MAX_PLAYERS; ++i, ++p) {
        *p = SDL_CreateRGBSurface(arrows->flags, arrows->w,
                                  arrows->h, arrows->format->BitsPerPixel,
                                  arrows->format->Rmask,
                                  arrows->format->Gmask,
                                  arrows->format->Bmask, 0);
    }

    /* Make ball copies */
    pballs = malloc((MAX_PLAYERS + 1) * sizeof(SDL_Surface));
    p = pballs;
    SDL_Surface *ball = images[IMG_BALL];
    for (int i = 0; i < MAX_PLAYERS + 1; ++i, ++p) {
        *p = SDL_CreateRGBSurface(ball->flags, ball->w,
                                  ball->h, ball->format->BitsPerPixel,
                                  ball->format->Rmask, ball->format->Gmask,
                                  ball->format->Bmask, 0);
    }

    /* Initialize weapon pointer array */
    wepIcons[0] = images[IMG_WI_BG];
    wepIcons[WEP_LIGHTNINGSPEED + 1] = images[IMG_WI_SPEED];
    wepIcons[WEP_FROSTWAVE + 1] = images[IMG_WI_FROST];
    wepIcons[WEP_CONFUSION + 1] = images[IMG_WI_CONFUSION];
    wepIcons[WEP_SHARPTURN + 1] = images[IMG_WI_SHARPTURN];
    wepIcons[WEP_TIMESTEP + 1] = images[IMG_WI_TIMESTEP];
    wepIcons[WEP_MOLE + 1] = images[IMG_WI_MOLE];
    wepIcons[WEP_WARP + 1] = images[IMG_WI_WARP];
    wepIcons[WEP_GHOST + 1] = images[IMG_WI_GHOST];
    wepIcons[WEP_TRON + 1] = images[IMG_WI_TRON];
    wepIcons[WEP_CHILI_RUN + 1] = images[IMG_WI_CHILIRUN];

    smallWepIcons[WEP_LIGHTNINGSPEED] = images[IMG_WIS_SPEED];
    smallWepIcons[WEP_FROSTWAVE] = images[IMG_WIS_FROST];
    smallWepIcons[WEP_CONFUSION] = images[IMG_WIS_CONFUSION];
    smallWepIcons[WEP_SHARPTURN] = images[IMG_WIS_SHARPTURN];
    smallWepIcons[WEP_TIMESTEP] = images[IMG_WIS_TIMESTEP];
    smallWepIcons[WEP_MOLE] = images[IMG_WIS_MOLE];
    smallWepIcons[WEP_WARP] = images[IMG_WIS_WARP];
    smallWepIcons[WEP_GHOST] = images[IMG_WIS_GHOST];
    smallWepIcons[WEP_TRON] = images[IMG_WIS_TRON];
    smallWepIcons[WEP_CHILI_RUN] = images[IMG_WIS_CHILIRUN];
}

/**
 * Exits the game.
 *
 * @param status Exit status.
 */
void exitGame(int status)
{
    free(hitmap);
    free(parrows);
    free(pballs);

    screen = SDL_SetVideoMode(WINDOW_W, WINDOW_H, SCREEN_BPP,
                              SDL_SWSURFACE);
    saveSettings();

    exit(status);
}

int main(void)
{
    static Uint32 prevtime = 0;

    initPlayers1();
    restoreSettings();

    if (!init())
        return 1;

    if (!initScreen())
        return 1;

    initHitMap(WINDOW_W, WINDOW_H);

    if (!loadImages()) {
        if (olvl >= O_NORMAL) {
            fprintf(stderr, "ERROR: Failed to load image files.\n");
        }
        return 1;
    }

    if (!loadSounds()) {
        if (olvl >= O_NORMAL) {
            fprintf(stderr, "ERROR: Failed to load sound files.\n");
        }
        return 1;
    }

    if (!loadFonts()) {
        if (olvl >= O_NORMAL) {
            fprintf(stderr, "ERROR: Failed to load font files.\n");
        }
        return 1;
    }

    initGraphics();
    initColors();
    initMainMenu();

    curScene = &mainMenu;
    curScene->displayFunc();

    if (music)
        playBGM();

    for (;;) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN
                    || event.type == SDL_MOUSEBUTTONDOWN
                    || event.type == SDL_JOYBUTTONDOWN
                    || event.type == SDL_JOYAXISMOTION) {

                int k = -1;
                if (event.type == SDL_KEYDOWN) {
                    k = event.key.keysym.sym;
                }
                else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    k = event.button.button;
                }
                else if (event.type == SDL_JOYBUTTONDOWN) {
                    k = event.button.button;
                }
                else if (event.type == SDL_JOYAXISMOTION) {
                    k = axisNumber(event.jaxis) << 4;
                }

                if (screenFreeze && k == SDLK_RETURN) {
                    screenFreeze = false;
                    endRound();
                    curScene = &mainMenu;
                    curScene->displayFunc();
                }

                if ((event.type == SDL_KEYDOWN && k == SDLK_ESCAPE)) {
                    screenFreeze = false;
                    if (curScene == &game || curScene == &gameStart)
                        endRound();
                    else if (curScene == &settingsMenu)
                        initMainMenu();
                    else if (curScene->parentScene == NULL)
                        exitGame(0);
                    playSound(SOUND_BEP, sound);
                    curScene = curScene->parentScene;
                    curScene->displayFunc();
                }
                else if (event.type == SDL_JOYBUTTONDOWN) {
                    if (olvl >= O_DEBUG) {
                        fprintf(stderr, "Pressed joy button: %d\n", k);
                    }
                    joyButtonDown[event.button.which][k] = true;
                }
                else if (event.type == SDL_JOYAXISMOTION) {
                    Uint8 jid = event.jaxis.which;

                    if (event.jaxis.value) {
                        if (olvl >= O_DEBUG) {
                            fprintf(stderr, "Joy axis changed: %d\n", k);
                        }
                        joyButtonDown[jid][k] = true;
                    }
                    else {
                        if (event.jaxis.axis == JOY_DIR_UP
                                || event.jaxis.axis == JOY_DIR_DOWN) {
                            joyButtonDown[jid][JOY_DIR_UP << 4] = false;
                            joyButtonDown[jid][JOY_DIR_DOWN << 4] = false;
                        }
                        else {
                            joyButtonDown[jid][JOY_DIR_RIGHT << 4] = false;
                            joyButtonDown[jid][JOY_DIR_LEFT << 4] = false;
                        }
                    }
                }
                else {
                    if (olvl >= O_DEBUG) {
                        fprintf(stderr, "Pressed: %c (%d)\n", k, k);
                    }
                    keyDown[k] = true;
                }
            }
            else if (event.type == SDL_KEYUP) {
                keyDown[event.key.keysym.sym] = false;
            }
            else if (event.type == SDL_MOUSEBUTTONUP) {
                keyDown[event.button.button] = false;
            }
            else if (event.type == SDL_JOYBUTTONUP) {
                joyButtonDown[event.button.which][event.button.button] = false;
            }
            else if (event.type == SDL_QUIT) {
                exitGame(0);
            }
            else if (event.type == SDL_VIDEORESIZE) {
                WINDOW_W = event.resize.w;
                WINDOW_H = event.resize.h;

                if (!initScreen()) {
                    return 1;
                }

                initHitMap(WINDOW_W, WINDOW_H);
                refreshGameScreen();
            }
        }
        if (!screenFreeze) {
            Uint32 timenow = SDL_GetTicks();
            Uint32 delta = timenow - prevtime;
            prevtime = timenow;

            if (delta < 1000/FPS_CAP) {
                SDL_Delay(1000/FPS_CAP - delta);
            }

            if (curScene->logicFunc()) {
                curScene->displayFunc();
            }
        }
    }

    return 0;
}
