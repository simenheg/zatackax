/* zatackax -- main game module.
 * Copyright (C) 2010-2017 The Zatacka X development team
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
    9,
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
    displayVoid,
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

/**
 * Stage 1 of player initialization.
 * Assigns keys and colors.
 */
void initPlayers1(void)
{
    int i;
    for (i = 0; i < MAX_PLAYERS; ++i)
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
        SDL_BlitSurface(arrows, NULL, *s, NULL);
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
    snprintf(p->name, PLAYER_NAME_LEN, defaultNames[player]);

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
    int i;
    for (i = 0; i < nPlayers; ++i)
        (&players[i])->weapon = 0;
}

/**
 * Extract and return an array containing the current player colors.
 *
 * @return Current player colors.
 */
SDL_Color *extractPColors(void)
{
    int i;
    SDL_Color *pcolors = malloc(sizeof(SDL_Color) * N_COLORS);
    for (i = 0; i < N_COLORS; ++i)
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
    int i;

    playSound(SOUND_CRASH, sound);
    // registerKill(killer, victim);

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

    for (i = 0; i < MAX_PLAYERS; ++i) {
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
    // printStats(nPlayers);
    // clearStats();

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
        int i;
        posOK = p->active - 1;
        initPos = spawn();
        p->posx = initPos.x;
        p->posy = initPos.y;
        for (i = 0; i < p->active - 1; ++i) {
            struct player *comp = &players[i];
            /* Securing spawning space between zatas. This may cause
             * trouble at small maps (will never get a fit).
             * ZATA_SPAWN_SPACING cuts off the waiting. */
            if (abs(p->posx - comp->posx) > ZATA_SPAWN_SPACING
                && abs(p->posy - comp->posy) > ZATA_SPAWN_SPACING) {
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
 * Catches and sets the next key pressed as a player's directional keys.
 *
 * @param pedit ID of the player to edit.
 * @param key l - left key, r - right key, w - weapon key
 */
void setNextKey(unsigned char pedit, unsigned char key)
{
    for (;;) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN
                || event.type == SDL_MOUSEBUTTONDOWN) {

                int k;
                if (event.type == SDL_KEYDOWN)
                    k = event.key.keysym.sym;
                else
                    k = event.button.button;

                char *keyname = keyName(k);

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
    int i, j;

    if (olvl >= O_DEBUG)
        fprintf(stderr, "Added to hitmap: %d, %d...\n", x, y);

    SDL_LockSurface(screen);

    for (i = -TOLERANCE; i <= TOLERANCE; ++i) {
        for (j = -TOLERANCE; j <= TOLERANCE; ++j) {

            if (abs(i) + abs(j) > TOLERANCE + 1)
                continue;

            int xpx = x + i;
            int ypx = y + j;

            if (xpx >= 0 && xpx < (int)WINDOW_W && ypx >= 0
                && ypx < (int)WINDOW_H) {

                unsigned char *hit =
                    &hitmap[sizeof(bool) * ((WINDOW_W * ypx) + xpx)];
                struct player *p = &players[player - 1];

                putPixel(xpx, ypx, colors[p->color], screen->pixels);

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

    SDL_UnlockSurface(screen);

    if (SDL_Flip(screen) == -1)
        exit(1);
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

    while (cur != NULL) {
        cur->count -= delta;
        unsigned char *at =
            &hitmap[sizeof(bool) * ((WINDOW_W * cur->y) + cur->x)];
        if (holes && cur->count <= HOLE_DELAY && *at > MAX_PLAYERS * 2) {
            *at = 0;
            SDL_LockSurface(screen);
            putPixel(cur->x, cur->y, cMenuBG, screen->pixels);
            SDL_UnlockSurface(screen);
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
    int i;
    Uint32 timenow = SDL_GetTicks();
    Uint32 delta = timenow - prevtime;
    prevtime = timenow;

    if (olvl >= O_DEBUG)
        fprintf(stderr, "delta: %d    prevtime: %d\n", delta, prevtime);

    if (delta > 35)
        delta = 35;

    for (i = 0; i < MAX_PLAYERS; ++i) {

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
                SDL_FreeSurface(screen);
                newRound();
                return 0;
            } else if (p->alive) {

                unsigned int curx, cury;

                if (weapons && p->wep_time == WEP_NONACTIVE
                    && p->wep_count > 0 && !activeDisable) {
                    if (p->ai) {
                        if (rand() / (double)RAND_MAX < AI_WEP_PROB) {
                            p->wep_time = wep_list[p->weapon].func(p, 1);
                            p->wep_count -= 1;
                            refreshGameScreen();
                        }
                    } else if (keyDown[p->wkey]) {
                        p->wep_time = wep_list[p->weapon].func(p, 1);
                        p->wep_count -= 1;
                        keyDown[p->wkey] = 0;
                        refreshGameScreen();
                    }
                }

                if (p->ai) {

                    /* quick and dirty ai-debugging. This should never
                     * be turned on */
                    /*
                      refreshGameScreen();
                      int checkx = p->posx + 40 * cos(p->dir + M_PI/2);
                      int checky = p->posy + 40 * sin(p->dir + M_PI/2);
                      SDL_Rect a = {checkx, checky, 4, 4};
                      SDL_FillRect(screen, &a, SDL_MapRGB(screen->format,
                      0x55, 0x77, 0x99));
                      checkx = p->posx + 40 * cos(p->dir - M_PI/2);
                      checky = p->posy + 40 * sin(p->dir - M_PI/2);
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
                      checkx = p->posx + 60 * cos(p->dir + M_PI/4);
                      checky = p->posy + 60 * sin(p->dir + M_PI/4);
                      SDL_Rect f = {checkx, checky, 4, 4};
                      SDL_FillRect(screen, &f, SDL_MapRGB(screen->format,
                      0x66, 0x66, 0x66));
                      checkx = p->posx + 60 * cos(p->dir - M_PI/4);
                      checky = p->posy + 60 * sin(p->dir - M_PI/4);
                      SDL_Rect g = {checkx, checky, 4, 4};
                      SDL_FillRect(screen, &g, SDL_MapRGB(screen->format,
                      0x66, 0x66, 0x66));
                    */

                    char c = pollAi(p->posx, p->posy, p->dir, p->active,
                                    hitmap, WINDOW_W, WINDOW_H);
                    if (activeTron) {
                        if (c == 'l')
                            p->dir -= M_PI / 2;
                        else if (c == 'r')
                            p->dir += M_PI / 2;
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
                            p->dir -= M_PI / 2;
                        } else if (keyDown[p->rkey]) {
                            keyDown[p->rkey] = 0;
                            p->dir += M_PI / 2;
                        }
                    } else {
                        if (keyDown[p->lkey])
                            p->dir -= 0.0022 * delta * p->speed;
                        else if (keyDown[p->rkey])
                            p->dir += 0.0022 * delta * p->speed;
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
    SDL_UnlockSurface(screen);
    return 1;
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
    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format,
                                                        0x00, 0x00, 0x00));

    drawExtras();

    if ((countdown % (START_ROUND_WAIT / 4)) > (START_ROUND_WAIT / 8)) {
        int i;
        for (i = 0; i < MAX_PLAYERS; ++i) {
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
    unsigned char *target;
    unsigned int xx;
    unsigned int yy;

    SDL_UnlockSurface(screen);
    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format,
                                                        0x00, 0x00, 0x00));

    drawExtras();

    SDL_LockSurface(screen);

    target = screen->pixels;

    for (yy = 0; yy < WINDOW_H; ++yy) {
        for (xx = 0; xx < WINDOW_W; ++xx, target += 4) {
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

    SDL_UnlockSurface(screen);

    if (SDL_Flip(screen) == -1)
        exit(1);
}

/**
 * Draws the game scores onto the screen.
 */
void drawExtras(void)
{
    int i;
    struct player *p;
    char score_str[SCORE_BUF];
    SDL_Surface *scoreText;

    for (i = 0; i < nPlayers; ++i) {
        p = &players[i];

        SDL_Rect offset = {7, SCORE_SPACING * i, 0, 0};

        snprintf(score_str, SCORE_BUF, "%d", p->score);
        scoreText = TTF_RenderUTF8_Shaded(font_score, score_str,
                                          colors[p->color], cMenuBG);
        SDL_BlitSurface(scoreText, NULL, screen, &offset);
        SDL_FreeSurface(scoreText);

        if (weapons && p->wep_count > 0) {
            int i;
            offset.y += 2;
            offset.x += WEP_SMALL_INIT_OFFSET;

            for (i = 0; i < p->wep_count; ++i) {
                offset.x += WEP_SMALL_PADDING;
                SDL_BlitSurface(smallWepIcons[p->weapon], NULL, screen,
                                &offset);
            }
        }
    }


    if (broadcasts) {
        for (i = 0; i < BROADC_LIMIT; ++i) {
            if (broadcast[i] != NULL) {
                SDL_Rect offset = {WINDOW_W - broadcast[i]->w - 4,
                                   WINDOW_H - (broadcast[i]->h * (i + 1)),
                                   0, 0};
                SDL_BlitSurface(broadcast[i], NULL, screen, &offset);
            }
        }
    }

    if (border) {
        Uint32 white = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);
        SDL_Rect leftBorder = {0, 0, 1, WINDOW_H};
        SDL_Rect rightBorder = {WINDOW_W - 1, 0, WINDOW_W, WINDOW_H};
        SDL_Rect topBorder = {0, 0, WINDOW_W, 1};
        SDL_Rect bottomBorder = {0, WINDOW_H - 1, WINDOW_W, WINDOW_H};
        SDL_FillRect(screen, &leftBorder, white);
        SDL_FillRect(screen, &rightBorder, white);
        SDL_FillRect(screen, &topBorder, white);
        SDL_FillRect(screen, &bottomBorder, white);
    }

    /* Active weapons */
    if (activeFreeze || activeConfusion || activeTron ||activeDisable) {
        SDL_Rect offset = {WINDOW_W - wepIcons[0]->w, 0, 0, 0};
        if (activeFreeze)
            SDL_BlitSurface(wepIcons[WEP_FROSTWAVE + 1], NULL, screen, &offset);
        if (activeConfusion)
            SDL_BlitSurface(wepIcons[WEP_CONFUSION + 1], NULL, screen, &offset);
        if (activeTron)
            SDL_BlitSurface(wepIcons[WEP_TRON + 1], NULL, screen, &offset);
        if (activeDisable)
            SDL_BlitSurface(wepIcons[WEP_DISABLE + 1], NULL, screen, &offset);
    }
}

/**
 * Starts a new round.
 */
void newRound(void)
{
    int i;

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

    for (i = 0; i < MAX_PLAYERS; ++i) {
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
 * Returns the number of legal weapons for the current number of players.
 * This is used to filter out weapons that are unusable in 2-player mode.
 *
 * @return Number of legal weapons for the current number of players.
 */
int legalWeps(void)
{
    if (nPlayers == 2)
        return N_WEAPONS - N_ILLEGAL_2P_WEPS;
    return N_WEAPONS;
}

/**
 * Lets the AI pick weapons at random.
 */
void assignAiWeapons(void)
{
    int i;
    for (i = 0; i < nPlayers; ++i) {
        struct player *p = &players[i];
        if (p->ai) {
            double rnd = (double)rand() / RAND_MAX;
            p->weapon = (int)(rnd * legalWeps());
        }
    }
}

/**
 * Weapon: lightning speed
 *
 * @param p Weapon user.
 * @param on 1 to use weapon, 0 to disable weapon.
 */
int wepLightningspeed(struct player *p, bool on)
{
    if (on) {
        playSound(SOUND_SPEED, sound);
        p->speed = 2.0;
    } else {
        p->speed = 1.0;
    }
    return DURATION_LIGHTNINGSPEED;
}

/**
 * Weapon: frostwave
 *
 * @param p Weapon user.
 * @param on 1 to use weapon, 0 to disable weapon.
 */
int wepFrostwave(struct player *p, bool on)
{
    int i;

    activeFreeze = on;

    if (on)
        playSound(SOUND_FREEZE, sound);
    else
        refreshGameScreen();

    for (i = 0; i < nPlayers; ++i) {
        struct player *target = &players[i];
        if (target != p) {
            if (on)
                target->speed = 0.3;
            else
                target->speed = 1.0;
        }
    }
    return DURATION_FROSTWAVE;
}

/**
 * Weapon: sharpturn
 *
 * @param p Weapon user.
 * @param on 1 to use weapon, 0 to disable weapon.
 */
int wepSharpturn(struct player *p, bool on)
{
    if (on) {
        playSound(SOUND_SHARPTURN, sound);

        if (keyDown[p->rkey])
            p->dir += M_PI / 2;
        else
            p->dir -= M_PI / 2;
    }

    p->inv_self = INV_TIME;

    return DURATION_SHARPTURN;
}

/**
 * Weapon: confusion
 *
 * @param p Weapon user.
 * @param on 1 to use weapon, 0 to disable weapon.
 */
int wepConfusion(struct player *p, bool on)
{
    activeConfusion = on;

    if (on)
        playSound(SOUND_CONFUSION, sound);
    else
        refreshGameScreen();

    int i;
    for (i = 0; i < nPlayers; ++i) {
        struct player *target = &players[i];
        if (target != p) {
            unsigned int tmpkey = target->lkey;
            if (on) {
                target->invertedKeys = true;
                target->lkey = target->rkey;
                target->rkey = tmpkey;
            } else {
                target->invertedKeys = false;
                target->lkey = target->rkey;
                target->rkey = tmpkey;
            }
        }
    }
    return DURATION_CONFUSION;
}

/**
 * Weapon: timestep
 *
 * @param p Weapon user.
 * @param on 1 to use weapon, 0 to disable weapon.
 */
int wepTimestep(struct player *p, bool on)
{
    if (on) {
        playSound(SOUND_TIMESTEP, sound);
        p->posx += 90 * cos(p->dir);
        p->posy += 90 * sin(p->dir);
    }
    return WEP_NONACTIVE;
}

/**
 * Weapon: mole
 *
 * @param p Weapon user.
 * @param on 1 to use weapon, 0 to disable weapon.
 */
int wepMole(struct player *p, bool on)
{
    if (on) {
        playSound(SOUND_MOLE, sound);

        double oldposx = p->posx;
        double oldposy = p->posy;
        double olddir = p->dir;

        p->posx = p->initposx;
        p->posy = p->initposy;
        p->dir = p->initdir + M_PI;

        p->initposx = oldposx;
        p->initposy = oldposy;
        p->initdir = olddir + M_PI;

        p->inv_self= DURATION_MOLE;
        p->inv_others = DURATION_MOLE;
    }

    return WEP_NONACTIVE;
}

/**
 * Weapon: warp
 *
 * @param p Weapon user.
 * @param on 1 to use weapon, 0 to disable weapon.
 */
int wepWarp(struct player *p, bool on)
{
    if (on) {
        playSound(SOUND_WARP, sound);

        double rnd;

        rnd = (double)rand() / RAND_MAX;
        p->posx = SPAWN_SPACE_MIN
            + (rnd * (WINDOW_W - (2 * SPAWN_SPACE_MIN)));
        rnd = (double)rand() / RAND_MAX;
        p->posy = SPAWN_SPACE_MIN
            + (rnd * (WINDOW_H - (2 * SPAWN_SPACE_MIN)));
    }

    return WEP_NONACTIVE;
}

/**
 * Weapon: ghost walk
 *
 * @param p Weapon user.
 * @param on 1 to use weapon, 0 to disable weapon.
 */
int wepGhost(struct player *p, bool on)
{
    if (on) {
        playSound(SOUND_GHOST, sound);
        p->inv_self = DURATION_GHOSTWALK;
        p->inv_others = DURATION_GHOSTWALK;
    }

    return 0;
}

/**
 * Weapon: tron mode
 *
 * @param p Weapon user.
 * @param on 1 to use weapon, 0 to disable weapon.
 */
int wepTron(struct player *p __attribute__ ((unused)), bool on)
{
    activeTron = on;

    if (on)
        playSound(SOUND_TRON, sound);
    else
        refreshGameScreen();

    return DURATION_TRON;
}

/**
 * Weapon: disable
 *
 * @param p Weapon user.
 * @param on 1 to use weapon, 0 to disable weapon.
 */
int wepDisable(struct player *p, bool on)
{
    activeDisable = on;

    if (on)
        playSound(SOUND_DISABLE, sound);
    else
        refreshGameScreen();

    p->invertedKeys = false;
    p->speed = 1.0;
    return DURATION_DISABLE;
}

/**
 * Weapon: switch-aroo
 *
 * @param p Weapon user.
 * @param on 1 to use weapon, 0 to disable weapon.
 */
int wepSwitch(struct player *p, bool on)
{
    if (on)
        playSound(SOUND_SWITCH, sound);

    unsigned int i, r;
    bool valid = 0;

    if (alivecount == 2)
        return WEP_NONACTIVE;

    i = rand();
    i %= nPlayers;
    while (!valid) {
        if (i != (unsigned)(p->active - 1) && (&players[i])->alive)
            valid = 1;
        else
            ++i;
        i %= nPlayers;
    }

    valid = 0;
    r = rand();
    r %= nPlayers;
    while (!valid) {
        if ((r != (unsigned)(p->active - 1)) &&
                (r != i) && (&players[r])->alive)
            valid = 1;
        else
            ++r;
        r %= nPlayers;
    }

    struct player *target1 = &players[i];
    struct player *target2 = &players[r];
    double posx = target2->posx;
    double posy = target2->posy;
    double dir = target2->dir;

    target1->inv_others = INV_TIME;
    target2->inv_others = INV_TIME;

    target2->posx = target1->posx;
    target2->posy = target1->posy;
    target2->dir = target1->dir;

    target1->posx = posx;
    target1->posy = posy;
    target1->dir = dir;

    return WEP_NONACTIVE;
}

/**
 * Negates all active weapon effects.
 */
void resetWeapons(void)
{
    int i;
    for (i = 0; i < MAX_PLAYERS; ++i) {
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
 * Query key status (for menu navigation).
 *
 * @param keysym One of enum keySymbols.
 * @return True if any of the bound keys are pressed.
 */
bool qkey(int keysym)
{
    switch (keysym) {
    case KEY_LEFT:
        if (keyDown[SDLK_LEFT]) {
            keyDown[SDLK_LEFT] = 0;
            return 1;
        } else if (keyDown[SDLK_h]) {
            keyDown[SDLK_h] = 0;
            return 1;
        } else if (keyDown[SDLK_b]) {
            keyDown[SDLK_b] = 0;
            return 1;
        }
        break;
    case KEY_RIGHT:
        if (keyDown[SDLK_RIGHT]) {
            keyDown[SDLK_RIGHT] = 0;
            return 1;
        } else if (keyDown[SDLK_l]) {
            keyDown[SDLK_l] = 0;
            return 1;
        } else if (keyDown[SDLK_f]) {
            keyDown[SDLK_f] = 0;
            return 1;
        }
        break;
    case KEY_UP:
        if (keyDown[SDLK_UP]) {
            keyDown[SDLK_UP] = 0;
            return 1;
        } else if (keyDown[SDLK_k]) {
            keyDown[SDLK_k] = 0;
            return 1;
        } else if (keyDown[SDLK_p]) {
            keyDown[SDLK_p] = 0;
            return 1;
        }
        break;
    case KEY_DOWN:
        if (keyDown[SDLK_DOWN]) {
            keyDown[SDLK_DOWN] = 0;
            return 1;
        } else if (keyDown[SDLK_j]) {
            keyDown[SDLK_j] = 0;
            return 1;
        } else if (keyDown[SDLK_n]) {
            keyDown[SDLK_n] = 0;
            return 1;
        }
        break;
    default:
        break;
    }

    return 0;
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
    if (keyDown[SDLK_SPACE] || keyDown[SDLK_RETURN]) {
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
        keyDown[SDLK_SPACE] = keyDown[SDLK_RETURN] = 0;
        return 1;
    } else if (qkey(KEY_LEFT)) {
        if (menuMain.choice == 0 && nPlayers > MIN_PLAYERS) {
            --nPlayers;
            playSound(nPlayers - 1, sound);
            deselectWeapons();
        }
        return 1;
    } else if (qkey(KEY_RIGHT)) {
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
    int i;
    char *c[menuMain.choices];
    c[0] = "START GAME";
    c[1] = "SETTINGS";
    c[2] = "EXIT";

    int ymod = logo->h / 2;
    displayMenu(c, &menuMain, ymod);
    SDL_Rect offset = {WINDOW_W / 2 - logo->w / 2, WINDOW_H / 2 - logo->h,
                       0, 0};
    SDL_BlitSurface(logo, NULL, screen, &offset);

    /* This could/should be made smoother... */
    for (i = 0; i < nPlayers; ++i) {
        offset.x = (WINDOW_W / 2)            /* window offset */
            - 60                             /* temp. offset */
            + (i - nPlayers) * BALL_SPACING; /* player modifier */

        offset.y = (WINDOW_H / 2)            /* window offset */
            - BALL_Y_MOD + ymod;             /* temp. offset */

        SDL_BlitSurface(pballs[i], NULL, screen, &offset);
    }

    for (i = nPlayers; i < MAX_PLAYERS; ++i) {
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

    if (keyDown[SDLK_SPACE] || keyDown[SDLK_RETURN]) {
        resetWeapons();
        playSound(SOUND_ROUND_BEGIN, sound);
        curScene = &gameStart;
    }

    for (p = &players[0], i = 0; i < nPlayers; ++i, ++p) {
        if (keyDown[p->lkey]) {
            if (p->weapon == 0) p->weapon = legalWeps() - 1;
            else p->weapon--;
            keyDown[p->lkey] = 0;
            return 1;
        } else if (keyDown[p->rkey]) {
            if (p->weapon == legalWeps() - 1) p->weapon = 0;
            else p->weapon++;
            keyDown[p->rkey] = 0;
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
    int i, j;
    struct player *p;
    int nweps = legalWeps();
    int mid = -(nweps / 2);
    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format,
                                                        0x00, 0x00, 0x00));

    char wep_str[MENU_BUF];
    SDL_Surface *wep_text;

    /* Draw weapon description strings */
    for (i = 0; i < nPlayers; ++i) {
        p = &players[i];
        struct weapon w = wep_list[p->weapon];

        SDL_Rect offset = {(i % 4) * (screen->w / 4) + 10,
                           i > 3 ? screen->h - 118 : 10, 0, 0};

        snprintf(wep_str, PLAYER_NAME_LEN, "%s", p->name);
        wep_text = TTF_RenderUTF8_Shaded(font_score, wep_str,
                                         colors[p->color], cMenuBG);
        SDL_BlitSurface(wep_text, NULL, screen, &offset);

        offset.y += 30;
        snprintf(wep_str, MENU_BUF, "%s", w.name);
        wep_text = TTF_RenderUTF8_Shaded(font_score, wep_str,
                                         colors[p->color], cMenuBG);
        SDL_BlitSurface(wep_text, NULL, screen, &offset);

        offset.y += 30;
        snprintf(wep_str, MENU_BUF, "%s", w.desc1);
        wep_text = TTF_RenderUTF8_Shaded(font_broadcb, wep_str,
                                         colors[p->color], cMenuBG);
        SDL_BlitSurface(wep_text, NULL, screen, &offset);

        if (w.desc2[0] != '\0') {
            offset.y += BROADC_FONT_SIZE + 2;
            snprintf(wep_str, MENU_BUF, "%s", w.desc2);
            wep_text = TTF_RenderUTF8_Shaded(font_broadcb, wep_str,
                                             colors[p->color], cMenuBG);
            SDL_BlitSurface(wep_text, NULL, screen, &offset);
        }

        if (i > 3)
            offset.y = screen->h - 10 - BROADC_FONT_SIZE;
        else
            offset.y = 105;
        snprintf(wep_str, MENU_BUF, "Charges: %d", w.charges);
        wep_text = TTF_RenderUTF8_Shaded(font_broadcb, wep_str,
                                         colors[p->color], cMenuBG);
        SDL_BlitSurface(wep_text, NULL, screen, &offset);

        SDL_FreeSurface(wep_text);
    }

    /* Draw weapon icons */
    for (i = 0; i < nweps; ++i, ++mid) {
        SDL_Rect offset = {
            (WINDOW_W / 2)          /* offset */
            - (wepIcons[0]->w / 2)  /* centralize */
            + (mid * ((wepIcons[0]->w / 2) + WEP_SPACEMOD))
            - ((nweps % 2) - 1)
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
        for (j = 0; j < nPlayers; ++j) {
            if (j == MAX_PLAYERS / 2) {
                offset.y += wepIcons[0]->h + PARROWSELECT_MOD_Y * 1.2;
                offset.x -= PARROWSELECT_MOD_X * (MAX_PLAYERS / 2);
            }
            p = &players[j];
            if (j != 0) offset.x += PARROWSELECT_MOD_X;
            if (p->weapon == i) {
                int diri;
                if (j >= MAX_PLAYERS / 2) {
                    diri = (int)((3.0 * M_PI / 2.0) * (32.0 / (2.0 * M_PI)));
                } else {
                    diri = (int)((M_PI / 2.0) * (32.0 / (2.0 * M_PI)));
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
    if (keyDown[SDLK_SPACE] || keyDown[SDLK_RETURN]) {
        switch (menuSettings.choice) {
        case 0: /* Toggle fullscreen */
            playSound(SOUND_BEEP, sound);
            fullscreen ^= 1;
            initScreen();
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
            keyDown[SDLK_SPACE] = keyDown[SDLK_RETURN] = 0;
            initMainMenu();
            curScene = curScene->parentScene;
            break;
        default:
            break;
        }
        keyDown[SDLK_SPACE] = keyDown[SDLK_RETURN] = 0;
        return 1;
    } else if (qkey(KEY_LEFT)) {
        if (menuSettings.choice == 8 && scorecap > 0) {
            playSound(SOUND_BEP, sound);
            --scorecap;
        }
        return 1;
    } else if (qkey(KEY_RIGHT)) {
        if (menuSettings.choice == 8 && scorecap < SCORE_CAP_MAX) {
            playSound(SOUND_BEEP, sound);
            ++scorecap;
        }
        return 1;
    } else if (keyDown[SDLK_BACKSPACE]) {
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
    strncat(s1, fullscreen ON_OFF, MENU_BUF - strlen(s1));
    strncat(s2, sound ON_OFF, MENU_BUF - strlen(s2));
    strncat(s3, music ON_OFF, MENU_BUF - strlen(s3));
    strncat(s4, weapons ON_OFF, MENU_BUF - strlen(s4));
    strncat(s5, holes ON_OFF, MENU_BUF - strlen(s5));
    strncat(s6, broadcasts ON_OFF, MENU_BUF - strlen(s6));
    strncat(s7, border ON_OFF, MENU_BUF - strlen(s7));
    strncat(s8, duelmode ON_OFF, MENU_BUF - strlen(s8));
    if (scorecap == 0) {
        strncat(s9, "∞", MENU_BUF);
    } else {
        snprintf(tmpCap, SCORE_BUF, "%d", scorecap);
        strncat(s9, tmpCap, MENU_BUF - SCORE_BUF);
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
    if (keyDown[SDLK_SPACE] || keyDown[SDLK_RETURN]) {
        if (menuPlayer.choice == 8) {
            playSound(SOUND_BEP, sound);
            curScene = curScene->parentScene;
        } else {
            playSound(SOUND_BEEP, sound);
            editPlayer = menuPlayer.choice;
            menuPConf.choice = 0;
            curScene = &pConfMenu;
        }
        keyDown[SDLK_SPACE] = keyDown[SDLK_RETURN] = 0;
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
    if (keyDown[SDLK_SPACE] || keyDown[SDLK_RETURN]) {
        switch (menuPConf.choice) {
        case 0:
            playSound(SOUND_BEEP, sound);
            setNextName(editPlayer);
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
        keyDown[SDLK_SPACE] = keyDown[SDLK_RETURN] = 0;
        return 1;
    } else if (menuPConf.choice == 0) {
        if (qkey(KEY_LEFT)) {
            playSound(SOUND_BEEP, sound);
            setColor(editPlayer, 0);
            return 1;
        } else if (qkey(KEY_RIGHT)) {
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

    char tmpKey[MAX_KEYNAME];
    struct player *p = (&players[editPlayer]);
    char *lkey = keyName(p->lkey);
    char *rkey = keyName(p->rkey);
    char *wkey = keyName(p->wkey);
    snprintf(s1, MENU_BUF, "%s",
             p->name[0] == '\0' ? "< enter name >" : p->name);

    strncat(s2, p->ai ON_OFF, MENU_BUF - strlen(s2));

    snprintf(tmpKey, MAX_KEYNAME * sizeof(char), "%s", lkey);
    free(lkey);
    strncat(s3, tmpKey, MAX_KEYNAME - 1);

    snprintf(tmpKey, MAX_KEYNAME * sizeof(char), "%s", wkey);
    free(wkey);
    strncat(s4, tmpKey, MAX_KEYNAME - 1);

    snprintf(tmpKey, MAX_KEYNAME * sizeof(char), "%s", rkey);
    free(rkey);
    strncat(s5, tmpKey, MAX_KEYNAME - 1);

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
    if (qkey(KEY_DOWN)) {
        playSound(SOUND_BEEP, sound);
        ++(m->choice);
        return 1;
    } else if (qkey(KEY_UP)) {
        playSound(SOUND_BEEP, sound);
        --(m->choice);
        return 1;
    }
    if (m->choice >= m->choices) {
        m->choice = 0;
        return 1;
    } else if (m->choice < 0) {
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
    int i;
    int mid = -(m->choices / 2);

    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format,
                                                        0x00, 0x00, 0x00));

    for (i = 0; i < m->choices; ++i, ++mid) {

        if (i == m->choice) {
            msg = TTF_RenderUTF8_Shaded(font_menub, c[i],
                                        i == 0 && curScene == &pConfMenu
                                        ? colors[(&players[editPlayer])->color]
                                        : cMenuTextH, cMenuBG);
        } else {
            msg = TTF_RenderUTF8_Solid(font_menu, c[i],
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
 * Initializes the application window and screen.
 *
 * @return 1 if the initialization was successful, 0 if not.
 */
int initScreen(void)
{
    SDL_FreeSurface(screen);

    if (fullscreen)
        screen = SDL_SetVideoMode(WINDOW_W, WINDOW_H, SCREEN_BPP,
                                  SDL_SWSURFACE | SDL_FULLSCREEN);
    else
        screen = SDL_SetVideoMode(WINDOW_W, WINDOW_H, SCREEN_BPP,
                                  SDL_SWSURFACE | SDL_RESIZABLE);
    if (screen == NULL)
        return 0;

    initHitMap(WINDOW_W, WINDOW_H);

    return 1;
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
    int xx, yy;
    unsigned char *target = sprite->pixels;

    for (yy = 0; yy < sprite->h; ++yy) {
        for (xx = 0; xx < sprite->w; ++xx, target += 4) {
            target[0] *= c.b / 255.0;
            target[1] *= c.g / 255.0;
            target[2] *= c.r / 255.0;
        }
    }
}

/**
 * Loads an image file.
 *
 * @param filename Name of the file to be loaded.
 * @return The loaded image, NULL if it failed.
 */
SDL_Surface *loadImage(const char filename[])
{
    SDL_Surface *loadedImage = IMG_Load(filename);
    SDL_Surface *optimizedImage = NULL;

    if (loadedImage != NULL)
        optimizedImage = SDL_DisplayFormatAlpha(loadedImage);

    SDL_FreeSurface(loadedImage);

    return optimizedImage;
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
    int i;
    SDL_Color inactive = {127, 127, 127, 0};
    SDL_Surface **s = pballs;
    struct player *p = &players[0];
    for (i = 0; i < MAX_PLAYERS; ++i, ++p, ++s) {
        SDL_BlitSurface(ball, NULL, *s, NULL);
        SDL_LockSurface(*s);
        colorFill(colors[p->color], *s);
        SDL_UnlockSurface(*s);
    }

    SDL_BlitSurface(ball, NULL, *s, NULL);
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
    SDL_WM_SetIcon(SDL_LoadBMP("data/gfx/icon.bmp"), NULL);

    return 1;
}

/**
 * Loads every file to be used in the game.
 * Also prepares them (clipping, duplicating).
 *
 * @return 1 if all files loaded successfully, 0 if not.
 */
int loadFiles(void)
{
    int x, y, i;
    SDL_Surface **p;

    /* Load images and fonts */
    if ((arrows = loadImage("data/gfx/arrowsheet.png")) == NULL) {
        fileNotFound("data/gfx/arrowsheet.png");
        return 0;
    }
    if ((ball = loadImage("data/gfx/ball.png")) == NULL) {
        fileNotFound("data/gfx/ball.png");
        return 0;
    }
    if ((logo = loadImage("data/gfx/logo.png")) == NULL) {
        fileNotFound("data/gfx/logo.png");
        return 0;
    }
    if ((wiBg = loadImage("data/gfx/wi_bg.png")) == NULL) {
        fileNotFound("data/gfx/wi_bg.png");
        return 0;
    }
    if ((wiSpeed = loadImage("data/gfx/wi_lightningspeed.png")) == NULL) {
        fileNotFound("data/gfx/wi_lightningspeed.png");
        return 0;
    }
    if ((wisSpeed = loadImage("data/gfx/wis_lightningspeed.png")) == NULL) {
        fileNotFound("data/gfx/wis_lightningspeed.png");
        return 0;
    }
    if ((wiFrost = loadImage("data/gfx/wi_frostwave.png")) == NULL) {
        fileNotFound("data/gfx/wi_frostwave.png");
        return 0;
    }
    if ((wisFrost = loadImage("data/gfx/wis_frostwave.png")) == NULL) {
        fileNotFound("data/gfx/wis_frostwave.png");
        return 0;
    }
    if ((wiConf = loadImage("data/gfx/wi_confusion.png")) == NULL) {
        fileNotFound("data/gfx/wi_confusion.png");
        return 0;
    }
    if ((wisConf = loadImage("data/gfx/wis_confusion.png")) == NULL) {
        fileNotFound("data/gfx/wis_confusion.png");
        return 0;
    }
    if ((wiTurn = loadImage("data/gfx/wi_sharpturn.png")) == NULL) {
        fileNotFound("data/gfx/wi_sharpturn.png");
        return 0;
    }
    if ((wisTurn = loadImage("data/gfx/wis_sharpturn.png")) == NULL) {
        fileNotFound("data/gfx/wis_sharpturn.png");
        return 0;
    }
    if ((wiStep = loadImage("data/gfx/wi_timestep.png")) == NULL) {
        fileNotFound("data/gfx/wi_timestep.png");
        return 0;
    }
    if ((wisStep = loadImage("data/gfx/wis_timestep.png")) == NULL) {
        fileNotFound("data/gfx/wis_timestep.png");
        return 0;
    }
    if ((wiMole = loadImage("data/gfx/wi_mole.png")) == NULL) {
        fileNotFound("data/gfx/wi_mole.png");
        return 0;
    }
    if ((wisMole = loadImage("data/gfx/wis_mole.png")) == NULL) {
        fileNotFound("data/gfx/wis_mole.png");
        return 0;
    }
    if ((wiWarp = loadImage("data/gfx/wi_warp.png")) == NULL) {
        fileNotFound("data/gfx/wi_warp.png");
        return 0;
    }
    if ((wisWarp = loadImage("data/gfx/wis_warp.png")) == NULL) {
        fileNotFound("data/gfx/wis_warp.png");
        return 0;
    }
    if ((wiGhost = loadImage("data/gfx/wi_ghost.png")) == NULL) {
        fileNotFound("data/gfx/wi_ghost.png");
        return 0;
    }
    if ((wisGhost = loadImage("data/gfx/wis_ghost.png")) == NULL) {
        fileNotFound("data/gfx/wis_ghost.png");
        return 0;
    }
    if ((wiTron = loadImage("data/gfx/wi_tron.png")) == NULL) {
        fileNotFound("data/gfx/wi_tron.png");
        return 0;
    }
    if ((wisTron = loadImage("data/gfx/wis_tron.png")) == NULL) {
        fileNotFound("data/gfx/wis_tron.png");
        return 0;
    }
    if ((wiDisable = loadImage("data/gfx/wi_disable.png")) == NULL) {
        fileNotFound("data/gfx/wi_disable.png");
        return 0;
    }
    if ((wisDisable = loadImage("data/gfx/wis_disable.png")) == NULL) {
        fileNotFound("data/gfx/wis_disable.png");
        return 0;
    }
    if ((wiSwitch = loadImage("data/gfx/wi_switch.png")) == NULL) {
        fileNotFound("data/gfx/wi_switch.png");
        return 0;
    }
    if ((wisSwitch = loadImage("data/gfx/wis_switch.png")) == NULL) {
        fileNotFound("data/gfx/wis_switch.png");
        return 0;
    }
    if ((font_menu = TTF_OpenFont("data/fonts/jura/JuraLight.ttf",
                                  MENU_FONT_SIZE)) == NULL) {
        fileNotFound("data/fonts/jura/JuraLight.ttf");
        return 0;
    }
    if ((font_menub = TTF_OpenFont("data/fonts/jura/JuraMedium.ttf",
                                   MENU_FONT_SIZE)) == NULL) {
        fileNotFound("data/fonts/jura/JuraMedium.ttf");
        return 0;
    }
    if ((font_broadc = TTF_OpenFont("data/fonts/ankacoder/AnkaCoder-r.ttf",
                                    BROADC_FONT_SIZE)) == NULL) {
        fileNotFound("data/fonts/ankacoder/AnkaCoder-r.ttf");
        return 0;
    }
    if ((font_broadcb = TTF_OpenFont("data/fonts/ankacoder/AnkaCoder-b.ttf"
                                     , BROADC_FONT_SIZE)) == NULL) {
        fileNotFound("data/fonts/ankacoder/AnkaCoder-b.ttf");
        return 0;
    }
    font_score = font_menub;

    if (olvl >= O_DEBUG) {
        confirmLoading("arrowsheet.png", arrows);
        confirmLoading("ball.png", ball);
        confirmLoading("logo.png", logo);
        confirmLoading("wi_bg.png", wiBg);
        confirmLoading("wi_lightningspeed.png", wiSpeed);
        confirmLoading("wi_frostwave.png", wiFrost);
        confirmLoading("wi_confusion.png", wiConf);
        confirmLoading("wi_confusion.png", wiTurn);
        confirmLoading("wi_timestep.png", wiStep);
        confirmLoading("wi_mole.png", wiMole);
        confirmLoading("wi_warp.png", wiWarp);
        confirmLoading("wi_ghost.png", wiGhost);
        confirmLoading("wi_switch.png", wiSwitch);
    }

    /* Clip arrow sprite sheet */
    for (y = i = 0; y < arrows->h; y += 16) {
        for (x = 0; x < arrows->w; x += 16, ++i) {
            arrowClip[i].x = x;
            arrowClip[i].y = y;
            arrowClip[i].w = 16;
            arrowClip[i].h = 16;
        }
    }

    /* Make arrow copies */
    parrows = malloc(MAX_PLAYERS * sizeof(SDL_Surface));
    p = parrows;
    for (i = 0; i < MAX_PLAYERS; ++i, ++p) {
        *p = SDL_CreateRGBSurface(arrows->flags, arrows->w,
                                  arrows->h, arrows->format->BitsPerPixel,
                                  arrows->format->Rmask,
                                  arrows->format->Gmask,
                                  arrows->format->Bmask, 0);
    }

    /* Make ball copies */
    pballs = malloc((MAX_PLAYERS + 1) * sizeof(SDL_Surface));
    p = pballs;
    for (i = 0; i < MAX_PLAYERS + 1; ++i, ++p) {
        *p = SDL_CreateRGBSurface(ball->flags, ball->w,
                                  ball->h, ball->format->BitsPerPixel,
                                  ball->format->Rmask, ball->format->Gmask,
                                  ball->format->Bmask, 0);
    }

    /* Initialize weapon pointer array */
    wepIcons[0] = wiBg;
    wepIcons[WEP_LIGHTNINGSPEED + 1] = wiSpeed;
    wepIcons[WEP_FROSTWAVE + 1] = wiFrost;
    wepIcons[WEP_CONFUSION + 1] = wiConf;
    wepIcons[WEP_SHARPTURN + 1] = wiTurn;
    wepIcons[WEP_TIMESTEP + 1] = wiStep;
    wepIcons[WEP_MOLE + 1] = wiMole;
    wepIcons[WEP_WARP + 1] = wiWarp;
    wepIcons[WEP_GHOST + 1] = wiGhost;
    wepIcons[WEP_TRON + 1] = wiTron;
    wepIcons[WEP_DISABLE + 1] = wiDisable;
    wepIcons[WEP_SWITCH + 1] = wiSwitch;

    smallWepIcons[WEP_LIGHTNINGSPEED] = wisSpeed;
    smallWepIcons[WEP_FROSTWAVE] = wisFrost;
    smallWepIcons[WEP_CONFUSION] = wisConf;
    smallWepIcons[WEP_SHARPTURN] = wisTurn;
    smallWepIcons[WEP_TIMESTEP] = wisStep;
    smallWepIcons[WEP_MOLE] = wisMole;
    smallWepIcons[WEP_WARP] = wisWarp;
    smallWepIcons[WEP_GHOST] = wisGhost;
    smallWepIcons[WEP_TRON] = wisTron;
    smallWepIcons[WEP_DISABLE] = wisDisable;
    smallWepIcons[WEP_SWITCH] = wisSwitch;

    return 1;
}

/**
 * Prints a message that confirms a loaded file.
 *
 * @param name Name of the file.
 * @param sprite The loaded image file.
 */
void confirmLoading(char *name, SDL_Surface *sprite)
{
    printf("Loaded: %s\t(w:%d h:%d bpp:%d)\n", name, sprite->w, sprite->h,
           sprite->format->BitsPerPixel);
}

/**
 * Saves the current settings to a configfile.
 *
 * @param filename Name of the file to save current settings to.
 */
void saveSettings(char *filename)
{
    unsigned int i;
    FILE *savefile = NULL;

    if ((savefile = fopen(filename, "w")) == NULL) {
        if (olvl >= O_NORMAL) {
            fprintf(stderr, "Couldn't open file '%s' for writing.\n",
                    filename);
        }
        return;
    } else {
        for (i = 0; i < sizeof(settings) / sizeof(bool*); ++i) {
            fprintf(savefile, "%s = %d\n", settingNames[i], *settings[i]);
        }
        fprintf(savefile, "scorecap = %d\n", scorecap);
        for (i = 0; i < MAX_PLAYERS; ++i) {
            fprintf(savefile, "%dn = %s\n", i + 1, (&players[i])->name);
            fprintf(savefile, "%dc = %d\n", i + 1, (&players[i])->color);
            fprintf(savefile, "%da = %d\n", i + 1, (&players[i])->ai);
            fprintf(savefile, "%dl = %d\n", i + 1, (&players[i])->lkey);
            fprintf(savefile, "%dw = %d\n", i + 1, (&players[i])->wkey);
            fprintf(savefile, "%dr = %d\n", i + 1, (&players[i])->rkey);
        }
    }

    if (olvl >= O_NORMAL) {
        if (ferror(savefile)) {
            if (olvl >= O_NORMAL)
                fprintf(stderr, "Error writing to file '%s'.\n", filename);
        }
    }

    fclose(savefile);
}

/**
 * Restores previous settings from a configure file.
 *
 * @param filename Name of the file to load the settings from.
 */
void restoreSettings(char *filename)
{
    FILE *savefile = NULL;

    if ((savefile = fopen(filename, "r")) == NULL) {
        if (olvl >= O_DEBUG)
            fprintf(stderr, "Couldn't restore settings from file %s.\n",
                    filename);
        return;
    } else {

        char settingHandle[STRBUF];
        char settingParam[STRBUF];
        unsigned int i;
        int line = 0;
        bool valid;

        for (;;) {
            if ((fscanf(savefile, "%s = %s\n", settingHandle,
                        settingParam)) != EOF) {
                valid = 0;
                for (i = 0; i < sizeof(settings) / sizeof(bool*); ++i) {
                    if (strncmp(settingNames[i], settingHandle, STRBUF) == 0) {
                        /* We have a matched setting */
                        *settings[i] = atoi(settingParam);
                        valid = 1;
                        break;
                    }
                }
                if (!valid && strncmp("scorecap", settingHandle, STRBUF) == 0)
                {
                    scorecap = atoi(settingParam);
                    valid = 1;
                } else if (!valid && isdigit(settingHandle[0])) {
                    switch (settingHandle[1]) {
                    case 'n': /* Name */
                        strncpy((&players[settingHandle[0] - '0' - 1])->name,
                                settingParam, PLAYER_NAME_LEN);
                        valid = 1;
                        break;
                    case 'c': /* Color */
                        (&players[settingHandle[0] - '0' - 1])->
                            color = atoi(settingParam);
                        valid = 1;
                        break;
                    case 'a': /* AI */
                        (&players[settingHandle[0] - '0' - 1])->
                            ai = atoi(settingParam);
                        valid = 1;
                        break;
                    case 'l': /* Left key */
                        (&players[settingHandle[0] - '0' - 1])->
                            lkey = atoi(settingParam);
                        valid = 1;
                        break;
                    case 'w': /* Weapon key */
                        (&players[settingHandle[0] - '0' - 1])->
                            wkey = atoi(settingParam);
                        valid = 1;
                        break;
                    case 'r': /* Right key */
                        (&players[settingHandle[0] - '0' - 1])->
                            rkey = atoi(settingParam);
                        valid = 1;
                        break;
                    default:
                        break;
                    }
                }
                if (valid == 0) {
                    if (olvl >= O_NORMAL)
                        fprintf(stderr, "Unknown config format in '%s', line "
                                "%d: %s\n", filename, line, settingHandle);
                }
                ++line;
            } else {
                break;
            }
        }
    }

    fclose(savefile);
}

/**
 * Turns a pressed key on.
 *
 * @param key The pressed key.
 */
void keyPress(unsigned int key) {
    keyDown[key] = 1;
}

/**
 * Turns a pressed key off.
 *
 * @param key The released key.
 */
void keyRelease(unsigned int key)
{
    keyDown[key] = 0;
}

/**
 * Generates an appropriate name for a given key.
 *
 * @param key Key to be named.
 * @return The name of the key, zerobytes if it's not valid.
 */
char *keyName(unsigned int key)
{
    char *keyname = calloc(MAX_KEYNAME, sizeof(char));

    if ((key >= SDLK_a && key <= SDLK_z) || (key >= SDLK_0 && key <= SDLK_9))
        snprintf(keyname, MAX_KEYNAME, "%c", key);
    else if (key >= SDLK_F1 && key <= SDLK_F15)
        snprintf(keyname, MAX_KEYNAME, "F%d", key - SDLK_F1 + 1);
    else {
        switch (key) {
        case SDLK_UNKNOWN:
            snprintf(keyname, MAX_KEYNAME, "none"); break;
        case SDLK_LEFT:
            snprintf(keyname, MAX_KEYNAME, "left"); break;
        case SDLK_RIGHT:
            snprintf(keyname, MAX_KEYNAME, "right"); break;
        case SDLK_UP:
            snprintf(keyname, MAX_KEYNAME, "up"); break;
        case SDLK_DOWN:
            snprintf(keyname, MAX_KEYNAME, "down"); break;
        case SDLK_PAUSE:
            snprintf(keyname, MAX_KEYNAME, "pause"); break;
        case SDLK_DELETE:
            snprintf(keyname, MAX_KEYNAME, "del"); break;
        case SDLK_INSERT:
            snprintf(keyname, MAX_KEYNAME, "ins"); break;
        case SDLK_HOME:
            snprintf(keyname, MAX_KEYNAME, "home"); break;
        case SDLK_END:
            snprintf(keyname, MAX_KEYNAME, "end"); break;
        case SDLK_MENU:
            snprintf(keyname, MAX_KEYNAME, "menu"); break;
        case SDLK_PAGEUP:
            snprintf(keyname, MAX_KEYNAME, "pg up"); break;
        case SDLK_PAGEDOWN:
            snprintf(keyname, MAX_KEYNAME, "pg dn"); break;
        case SDLK_RSHIFT:
            snprintf(keyname, MAX_KEYNAME, "r-shift"); break;
        case SDLK_LSHIFT:
            snprintf(keyname, MAX_KEYNAME, "l-shift"); break;
        case SDLK_RCTRL:
            snprintf(keyname, MAX_KEYNAME, "r-ctrl"); break;
        case SDLK_LCTRL:
            snprintf(keyname, MAX_KEYNAME, "l-ctrl"); break;
        case SDLK_RALT:
            snprintf(keyname, MAX_KEYNAME, "r-alt"); break;
        case SDLK_LALT:
            snprintf(keyname, MAX_KEYNAME, "l-alt"); break;
        case SDLK_MODE:
            snprintf(keyname, MAX_KEYNAME, "alt gr"); break;
        case SDLK_RSUPER:
            snprintf(keyname, MAX_KEYNAME, "r-super"); break;
        case SDLK_LSUPER:
            snprintf(keyname, MAX_KEYNAME, "l-super"); break;
        case SDLK_TAB:
            snprintf(keyname, MAX_KEYNAME, "tab"); break;
        case SDLK_PERIOD:
            snprintf(keyname, MAX_KEYNAME, "."); break;
        case SDLK_COMMA:
            snprintf(keyname, MAX_KEYNAME, ","); break;
        case SDLK_MINUS:
            snprintf(keyname, MAX_KEYNAME, "-"); break;
        case SDLK_QUOTE:
            snprintf(keyname, MAX_KEYNAME, "'"); break;
        case SDLK_PLUS:
            snprintf(keyname, MAX_KEYNAME, "+"); break;
        case SDLK_COMPOSE:
            snprintf(keyname, MAX_KEYNAME, "^"); break;
        case SDLK_BACKSLASH:
            snprintf(keyname, MAX_KEYNAME, "\\"); break;
        case SDLK_LESS:
            snprintf(keyname, MAX_KEYNAME, "<"); break;
        case SDLK_BACKSPACE:
            snprintf(keyname, MAX_KEYNAME, "b-space"); break;
        case SDLK_RETURN:
            snprintf(keyname, MAX_KEYNAME, "enter"); break;
        case SDLK_SPACE:
            snprintf(keyname, MAX_KEYNAME, "space"); break;
        case SDL_BUTTON_LEFT:
            snprintf(keyname, MAX_KEYNAME, "l-mouse"); break;
        case SDL_BUTTON_MIDDLE:
            snprintf(keyname, MAX_KEYNAME, "m-mouse"); break;
        case SDL_BUTTON_RIGHT:
            snprintf(keyname, MAX_KEYNAME, "r-mouse"); break;
        case SDLK_WORLD_70:
            snprintf(keyname, MAX_KEYNAME, "æ"); break;
        case SDLK_WORLD_88:
            snprintf(keyname, MAX_KEYNAME, "ø"); break;
        case SDLK_WORLD_69:
            snprintf(keyname, MAX_KEYNAME, "å"); break;
        default:
            break;
        }
    }
    return keyname;
}

/**
 * Display nothing.
 */
void displayVoid(void) {}

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

    saveSettings(".zatackax");

    exit(status);
}

int main(void)
{
    static Uint32 prevtime = 0;

    WINDOW_W = DEFAULT_WINDOW_W;
    WINDOW_H = DEFAULT_WINDOW_H;

    initPlayers1();
    restoreSettings(".zatackax");

    if (!init())
        return 1;

    if (!initScreen())
        return 1;

    if (!loadFiles()) {
        if (olvl >= O_NORMAL)
            fprintf(stderr, "ERROR: Failed to load files.\n");
        return 1;
    }

    initColors();
    initMainMenu();

    curScene = &mainMenu;
    curScene->displayFunc();

    if (music)
        playBGM();

    for (;;) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN
                || event.type == SDL_MOUSEBUTTONDOWN) {

                int k;
                if (event.type == SDL_KEYDOWN)
                    k = event.key.keysym.sym;
                else
                    k = event.button.button;

                if (screenFreeze && k == SDLK_RETURN) {
                    screenFreeze = false;
                    endRound();
                    curScene = &mainMenu;
                    curScene->displayFunc();
                }

                if (k == SDLK_ESCAPE) {
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
                } else {
                    if (olvl >= O_DEBUG)
                        fprintf(stderr, "Pressed: %c (%d)\n", k, k);
                    keyDown[k] = 1;
                }
            } else if (event.type == SDL_KEYUP)
                keyDown[event.key.keysym.sym] = 0;
            else if (event.type == SDL_MOUSEBUTTONUP)
                keyDown[event.button.button] = 0;
            else if (event.type == SDL_QUIT)
                exitGame(0);
            else if (event.type == SDL_VIDEORESIZE) {

                WINDOW_W = event.resize.w;
                WINDOW_H = event.resize.h;

                if (!initScreen())
                    return 1;

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
