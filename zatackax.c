#include "zatackax.h"

struct menu menuMain = {
    3,
    0,
};

struct menu menuSettings = {
    6,
    0,
};

struct menu menuPlayer = {
    9,
    0,
};

struct menu menuPConf = {
    5,
    0,
};

static struct scene mainMenu = {
    logicMainMenu,
    displayMainMenu,
    NULL
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
    struct player *p = &players[0];

    for (i = 0; i < MAX_PLAYERS; ++i, ++p) {
        p->color = i;
        p->speed = 1.0;
        p->invertedKeys = 0;
        switch (i) {
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
        p->wepcount = -999;
        if (i % 2 == 0) p->wepFunc = wepConfusion;
        else p->wepFunc = wepColdwave;

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
 * Kills target player.
 *
 * @param killed ID of the player that was killed.
 * @param killer ID of the player that was crashed into.
 */
void killPlayer(unsigned char killed, unsigned char killer)
{
    int i;

    struct player *p = &players[killed - 1];
    --alivecount;
    p->alive = 0;

    for (i = 0; i < MAX_PLAYERS; ++i) {
        struct player *pt = &players[i];
        if (pt->active) {
            if (pt->alive) {
                pt->score++;
            }
        } else {
            break;
        }
    }

    if (broadcasts) makeBroadcast(p, killer);

    refreshGameScreen(); /* Update scores */
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
    Uint32 timeseed = SDL_GetTicks();

    if (randomizer > RAND_MAX) {
        randomizer = 1;
    }

    srand(timeseed * randomizer++);
    rnd = (double)rand() / RAND_MAX;
    initPos.x = SPAWN_SPACE_MIN
        + (rnd * (WINDOW_W - (2 * SPAWN_SPACE_MIN)));
    srand(timeseed * randomizer++);
    rnd = (double)rand() / RAND_MAX;
    initPos.y = SPAWN_SPACE_MIN
        + (rnd * (WINDOW_H - (2 * SPAWN_SPACE_MIN)));
    srand(timeseed * randomizer++);
    rnd = (double)rand() / RAND_MAX;
    initPos.holecount = (rnd * (HOLE_FREQ - HOLE_FIRST_DELAY));
    srand(timeseed * randomizer++);
    rnd = (double)rand() / RAND_MAX;
    initPos.dir = rnd * (2 * PI);
    if (initPos.dir < 0) initPos.dir *= -1;

    return initPos;
}

/**
 * Spawns a player randomly on the map.
 *
 * @param p The player which is to be spawned.
 */
void respawn(struct player *p)
{
    struct vel initPos = spawn();
    p->alive = 1;
    p->posx = initPos.x;
    p->posy = initPos.y;
    p->dir = initPos.dir;
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
    Uint32 timeseed = SDL_GetTicks();
    srand(timeseed);
    p->alive = 1;
    p->posx = (WINDOW_W / 2) + (p->active == 1 ? -120 : 120);
    p->posy = WINDOW_H / 2;
    p->dir = p->active == 1 ? 0 : PI;
    p->prevx = p->posx;
    p->prevy = p->posy;
    p->holecount = ((++randomizer * rand())
            % (HOLE_FREQ - HOLE_FIRST_DELAY));
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
        if ((&players[pedit])->color == N_COLORS - 1) {
            (&players[pedit])->color = 0;
        } else {
            ++((&players[pedit])->color);
        }
    } else {
        if ((&players[pedit])->color == 0) {
            (&players[pedit])->color = N_COLORS - 1;
        } else {
            --((&players[pedit])->color);
        }
    }
}

/**
 * Catches and sets the next key pressed as a player's directional keys.
 *
 * @param pedit ID of the player to edit.
 * @param key l - left key, r - right key, w - weapon key
 */
void setNextKey(unsigned char pedit, unsigned char key) {
    for (;;) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {

                int k = event.key.keysym.sym;
                char *keyname = keyName(k);
                if (keyname[0] == '\0') {
#ifdef DEBUG
                    printf("Unknown key\n");
#endif
                    k = 0;
                } else {
#ifdef DEBUG
                    printf("Set new key: %s\n", keyname);
#endif
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
 * Initializes the hitmap.
 *
 * @param w Width of the map.
 * @param h Height of the map.
 */
void initHitMap(unsigned int w, unsigned int h)
{
    size_t len = sizeof(bool) * w * h;
    hitmap = (bool*)malloc(len);
    memset(hitmap, '\0', len);
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

#ifdef DEBUG
    fprintf(stderr, "Added to hitmap: %d, %d...\n", x, y);
#endif

    SDL_LockSurface(screen);

    for (i = -TOLERANCE; i < TOLERANCE; ++i) {
        for (j = -TOLERANCE; j < TOLERANCE; ++j) {

            int xpx = x + i;
            int ypx = y + j;

            if (xpx >= 0 && xpx < (int)WINDOW_W && ypx >= 0
                    && ypx < (int)WINDOW_H) {

                unsigned char *hit =
                    &hitmap[sizeof(bool) * ((WINDOW_W * ypx) + xpx)];

                putPixel(xpx, ypx, colors[(&players[player-1])->color],
                        screen->pixels);

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
                    if (player == *hit) {
                        printf("Player %d committed suicide!\n", player);
                        killPlayer(player, *hit);
                    } else {
                        int killer = *hit;
                        while (killer > MAX_PLAYERS) {
                            killer -= MAX_PLAYERS;
                        }
                        printf("Player %d crashed into Player %d!\n",
                                player, killer);
                        killPlayer(player, killer);
                    }
#ifdef DEBUG
                    fprintf(stderr, "Player %d crashed at: (%d, %d)\n",
                            player, xpx, ypx);
#endif
                    return;
                }
            } else {
                printf("Player %d hit the wall!\n", player);
#ifdef DEBUG
                fprintf(stderr, "Player %d walled at: (%d, %d)\n",
                        player, xpx, ypx);
#endif
                killPlayer(player, 0);
                return;
            }
        }
    }

    SDL_UnlockSurface(screen);

    if (SDL_Flip(screen) == -1) {
        exit(1);
    }
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
#ifdef DEBUG
    fprintf(stderr, "Updating hitmap...\n");
#endif

    struct recentMapPiece *cur;
    struct recentMapPiece *prev = recents;

    if (prev == NULL || prev->next == NULL) {
        return;
    }

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
#ifdef DEBUG
    fprintf(stderr, "Cleaning hitmap...\n");
#endif

    struct recentMapPiece *cur = recents;
    struct recentMapPiece *tmp;

    if (cur == NULL) return;

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
 */
void logicGame(void)
{
    int i;
    Uint32 timenow = SDL_GetTicks();
    Uint32 delta = timenow - prevtime;
    prevtime = timenow;

#ifdef DEBUG
    fprintf(stderr, "delta: %d    prevtime: %d\n", delta, prevtime);
#endif

    if (delta > 35) {
        delta = 35;
    }

    for (i = 0; i < MAX_PLAYERS; ++i) {
        if (players[i].active) {

            struct player *p = &players[i];

            if (p->wepcount > 0) {
                p->wepcount -= delta;
                if (p->wepcount <= 0) {
                    p->wepFunc(p, 0);
                }
            }

            if (alivecount <= 1) {
                SDL_FreeSurface(screen);
                newRound();
                return;
            } else if (p->alive) {

                unsigned int curx, cury;

                if (p->wepcount == -999 && keyDown[p->wkey]) {
                    p->wepcount = p->wepFunc(p, 1);
                }
                if (keyDown[p->lkey]) {
                    p->dir -= 0.0022 * delta * p->speed;
                } else if (keyDown[p->rkey]) {
                    p->dir += 0.0022 * delta * p->speed;
                }

                p->posx += ZATA_SPEED * cos(p->dir) * delta * p->speed;
                p->posy += ZATA_SPEED * sin(p->dir) * delta * p->speed;

                curx = (unsigned int)p->posx;
                cury = (unsigned int)p->posy;

                if (holes) {
                    p->holecount += delta;
                    if (p->holecount > HOLE_FREQ) {
                        p->holecount = 0;
                    }
                }

                if (p->prevx != curx || p->prevy != cury) {
#ifdef DEBUG
                    fprintf(stderr, "Adding to hitmap: %d, %d...\n",
                            p->prevx, p->prevy);
#endif
                    if (holes && p->holecount > HOLE_SIZE) {
                        addToHitMap(p->prevx, p->prevy, p->active,
                                MAX_PLAYERS);
                    } else {
                        addToHitMap(p->prevx, p->prevy, p->active, 0);
                    }
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
}

/**
 * Logic of the startup phase of the game.
 */
void logicGameStart(void)
{
    Uint32 timenow = SDL_GetTicks();
    Uint32 delta = timenow - prevtime;
    prevtime = timenow;

#ifdef DEBUG
    fprintf(stderr, "delta: %d    prevtime: %d\n", delta, prevtime);
#endif

    if (delta > 13) {
        delta = 13;
    }

    if (countdown > 0) {
        countdown -= delta;
    } else {
        curScene = &game;
    }
}

/**
 * Display blinking arrows at the beginning of a game.
 */
void displayGameStart(void)
{
    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format,
                0x00, 0x00, 0x00));

    drawScores();

    if ((countdown % (START_ROUND_WAIT / 4)) > (START_ROUND_WAIT / 8)) {
        int i;
        for (i = 0; i < MAX_PLAYERS; ++i) {
            if (players[i].active) {
                struct player *p = &players[i];
                SDL_Rect offset = {(int)p->posx - 8, (int)p->posy - 8,
                    0, 0};
                int diri = (int)((p->dir) * (32.0 / (2.0 * PI)));
                SDL_BlitSurface(p->arrow, &arrowClip[diri], screen,
                        &offset);
            } else {
                break;
            }
        }
    }

    if (SDL_Flip(screen) == -1) {
        exit(1);
    }
}

/**
 * Redraws the whole game screen.
 * Should only be called as a last restort when the whole screen needs to
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

    drawScores();

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

    if (SDL_Flip(screen) == -1) {
        exit(1);
    }
}

/**
 * Creates a broadcast message.
 *
 * @param p ID of the player which is to be made a broadcast for.
 * @param killer Cause of death.
 */
void makeBroadcast(struct player *p, unsigned char killer)
{
    int i;
    char anomsg[2][BROADC_BUF];
    SDL_Surface *tmp[3];
    int broadw = 0;
    int nbroad = 0;
    int killed = p->active;

    SDL_FreeSurface(broadcast[BROADC_LIMIT - 1]);
    for (i = BROADC_LIMIT - 1; i > 0; --i) {
        broadcast[i] = broadcast[i - 1];
        if (broadcast[i] != NULL) {
            int alpha = 255 - i * (255.0 / BROADC_LIMIT);
            SDL_SetAlpha(broadcast[i], SDL_SRCALPHA, alpha);
        }
    }

    /* Make broadcast message */
    snprintf(anomsg[0], BROADC_BUF, "Player%d", killed);

    if (killer == 0) { /* Walled */

        nbroad = 2;
        tmp[0] = TTF_RenderUTF8_Shaded(font_broadcb, anomsg[0],
                colors[p->color], cMenuBG);
        tmp[1] = TTF_RenderUTF8_Shaded(font_broadc,
                " hit the wall", cBroadcast, cMenuBG);

    } else if (killer == killed) { /* Suicide */

        nbroad = 2;
        tmp[0] = TTF_RenderUTF8_Shaded(font_broadcb, anomsg[0],
                colors[p->color], cMenuBG);
        tmp[1] = TTF_RenderUTF8_Shaded(font_broadc,
                " committed suicide", cBroadcast, cMenuBG);

    } else { /* Killed */

        struct player *pk = &players[killer - 1];
        nbroad = 3;

        snprintf(anomsg[1], BROADC_BUF, "Player%d", killer);

        tmp[0] = TTF_RenderUTF8_Shaded(font_broadcb, anomsg[0],
                colors[p->color], cMenuBG);
        tmp[1] = TTF_RenderUTF8_Shaded(font_broadc,
                " crashed into ", cBroadcast, cMenuBG);
        tmp[2] = TTF_RenderUTF8_Shaded(font_broadcb, anomsg[1],
                colors[pk->color], cMenuBG);
    }

    for (i = 0; i < nbroad; ++i) broadw += tmp[i]->w;
    broadcast[0] = SDL_CreateRGBSurface(SDL_HWSURFACE,
            broadw, tmp[0]->h, SCREEN_BPP, 0, 0, 0, 0);

    for (i = 0; i < nbroad; ++i) {
        int j;
        SDL_Rect offset = {0, 0, 0, 0};
        for (j = i; j > 0; --j) offset.x += tmp[j-1]->w;
        SDL_BlitSurface(tmp[i], NULL, broadcast[0], &offset);
    }

    for (i = 0; i < nbroad; ++i) SDL_FreeSurface(tmp[i]);
}

/**
 * Empties the broadcast message queue.
 */
void cleanBroadcast(void)
{
    int i;
    for (i = 0; i < BROADC_LIMIT; ++i) {
        SDL_FreeSurface(broadcast[i]);
    }
    memset(broadcast, '\0', BROADC_LIMIT * sizeof(SDL_Surface*));
}

/**
 * Draws the game scores onto the screen.
 */
void drawScores(void)
{
    int i;
    struct player *p;
    char score_str[SCORE_BUF];
    SDL_Surface *scoreText;

    for (i = 0; i < nPlayers; ++i) {
        SDL_Rect offset = {7, SCORE_SPACING * i, 0, 0};
        p = &players[i];
        snprintf(score_str, SCORE_BUF, "%d", p->score);
        scoreText = TTF_RenderUTF8_Shaded(font_score, score_str,
                colors[p->color], cMenuBG);
        SDL_BlitSurface(scoreText, NULL, screen, &offset);
        SDL_FreeSurface(scoreText);
    }

    if (broadcasts) {
        for (i = 0; i < BROADC_LIMIT; ++i) {
            if (broadcast[i] != NULL) {
                SDL_Rect offset = {WINDOW_W - broadcast[i]->w - 4,
                    WINDOW_H - (broadcast[i]->h * (i + 1)), 0, 0};
                SDL_BlitSurface(broadcast[i], NULL, screen, &offset);
            }
        }
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

    printf(" -- New round! --  ( Score: ");
    for (i = 0; i < MAX_PLAYERS; ++i) {
        struct player *p = &players[i];
        if (p->active) {

            /* Reset weapons */
            if (p->wepcount != -999) {
                p->wepFunc(p, 0);
                p->wepcount = -999;
            }

            printf("%d ", p->score);
            if (duelmode) {
                drespawn(p);
            } else {
                respawn(p);
            }
        } else {
            printf(")\n");
            return;
        }
    }

    printf(")\n");
}

/** Weapon: speedup
 *
 * @param p Weapon user.
 * @param on 1 to use weapon, 0 to disable weapon.
 */
int wepSpeedup(struct player *p, bool on)
{
    if (on) {
        p->speed = 2.0;
    } else {
        p->speed = 1.0;
    }
    return 1200;
}

/** Weapon: coldwave
 *
 * @param p Weapon user.
 * @param on 1 to use weapon, 0 to disable weapon.
 */
int wepColdwave(struct player *p, bool on)
{
    int i;
    for (i = 0; i < nPlayers; ++i) {
        struct player *target = &players[i];
        if (target != p) {
            if (on) target->speed = 0.3;
            else target->speed = 1.0;
        }
    }
    return 1500;
}

/** Weapon: sharpturn
 *
 * @param p Weapon user.
 * @param on 1 to use weapon, 0 to disable weapon.
 */
int wepSharpturn(struct player *p, bool on)
{
    if (keyDown[p->rkey]) p->dir += PI / 2;
    else p->dir -= PI / 2;
    return 0;
}

/** Weapon: confusion
 *
 * @param p Weapon user.
 * @param on 1 to use weapon, 0 to disable weapon.
 */
int wepConfusion(struct player *p, bool on)
{
    int i;
    for (i = 0; i < nPlayers; ++i) {
        struct player *target = &players[i];
        if (target != p) {
            unsigned int tmpkey = target->lkey;
            if (on && !target->invertedKeys) {
                target->invertedKeys = 1;
                target->lkey = target->rkey;
                target->rkey = tmpkey;
            } else if (!on && target->invertedKeys) {
                target->invertedKeys = 0;
                target->lkey = target->rkey;
                target->rkey = tmpkey;
            }
        }
    }
    return 2000;
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
 */
void logicMainMenu(void)
{
    if (keyDown[SDLK_SPACE] || keyDown[SDLK_RETURN]) {
        switch (menuMain.choice) {
            case 0:
                initPlayers2();
                newRound();
                curScene = &gameStart;
                break;
            case 1:
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
    } else if (keyDown[SDLK_LEFT]) {
        keyDown[SDLK_LEFT] = 0;
        if (menuMain.choice == 0 && nPlayers > MIN_PLAYERS) --nPlayers;
    } else if (keyDown[SDLK_RIGHT]) {
        keyDown[SDLK_RIGHT] = 0;
        if (!duelmode && menuMain.choice == 0 && nPlayers < MAX_PLAYERS)
            ++nPlayers;
    }
    handleMenu(&menuMain);
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

    displayMenu(c, &menuMain);

    /* This could/should be made smoother... */
    for (i = 0; i < nPlayers; ++i) {
        SDL_Rect offset = {
            (WINDOW_W / 2)              /* window offset */
                - 55                    /* temp. offset */            
                + (i - nPlayers) * 30   /* player modifier */
                ,
            (WINDOW_H / 2)              /* window offset */
                - 37                    /* temp. offset */
                , 0, 0
        };
        SDL_BlitSurface(pballs[i], NULL, screen, &offset);
    }

    for (i = nPlayers; i < MAX_PLAYERS; ++i) {
        SDL_Rect offset = {
            (WINDOW_W / 2)              /* window offset */
                + 64                    /* temp. offset */            
                + (i - nPlayers) * 30   /* player modifier */
                ,
            (WINDOW_H / 2)              /* window offset */
                - 37                    /* temp. offset */
                , 0, 0
        };
        SDL_BlitSurface(pballs[MAX_PLAYERS], NULL, screen, &offset);
    }

    if (SDL_Flip(screen) == -1) {
        exit(1);
    }
}

/**
 * Logic of the settings menu.
 */
void logicSettingsMenu(void)
{
    if (keyDown[SDLK_SPACE] || keyDown[SDLK_RETURN]) {
        switch (menuSettings.choice) {
            case 0: /* Toggle fullscreen */
                fullscreen ^= 1;
                initScreen();
                break;
            case 1: /* Toggle holes */
                holes ^= 1;
                break;
            case 2: /* Toggle broadcasts */
                broadcasts ^= 1;
                break;
            case 3: /* Toggle duel mode */
                duelmode ^= 1;
                if (duelmode) nPlayers = 2;
                break;
            case 4:
                menuPlayer.choice = 0;
                curScene = &playerMenu;
                break;
            case 5: /* Back */
                keyDown[SDLK_SPACE] = keyDown[SDLK_RETURN] = 0;
                initMainMenu();
                curScene = curScene->parentScene;
                break;
            default:
                break;
        }
        keyDown[SDLK_SPACE] = keyDown[SDLK_RETURN] = 0;
    }
    handleMenu(&menuSettings);
}

/**
 * Display the settings menu screen.
 */
void displaySettingsMenu(void)
{
    char *c[menuSettings.choices];

    char s1[MENU_BUF] = "FULLSCREEN ";
    char s2[MENU_BUF] = "HOLES ";
    char s3[MENU_BUF] = "BROADCASTS ";
    char s4[MENU_BUF] = "DUEL MODE ";
    strncat(s1, fullscreen ON_OFF, MENU_BUF - strlen(s1));
    strncat(s2, holes ON_OFF, MENU_BUF - strlen(s2));
    strncat(s3, broadcasts ON_OFF, MENU_BUF - strlen(s3));
    strncat(s4, duelmode ON_OFF, MENU_BUF - strlen(s4));
    c[0] = s1;
    c[1] = s2;
    c[2] = s3;
    c[3] = s4;
    c[4] = "PLAYER CONFIG";
    c[5] = "BACK";

    displayMenu(c, &menuSettings);

    if (SDL_Flip(screen) == -1) {
        exit(1);
    }
}

/**
 * Logic for the player menu.
 */
void logicPlayerMenu(void)
{
    if (keyDown[SDLK_SPACE] || keyDown[SDLK_RETURN]) {
        if (menuPlayer.choice == 8) {
            curScene = curScene->parentScene;
        } else {
            editPlayer = menuPlayer.choice;
            menuPConf.choice = 0;
            curScene = &pConfMenu;
        }
        keyDown[SDLK_SPACE] = keyDown[SDLK_RETURN] = 0;
    }
    handleMenu(&menuPlayer);
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

    displayMenu(c, &menuPlayer);

    if (SDL_Flip(screen) == -1) {
        exit(1);
    }
}

/**
 * Logic for the player configuration.
 */
void logicPConfMenu(void)
{
    if (keyDown[SDLK_SPACE] || keyDown[SDLK_RETURN]) {
        switch (menuPConf.choice) {
            case 0:
                setColor(editPlayer, 1);
                break;
            case 1:
                (&players[editPlayer])->lkey = SDLK_CLEAR;
                displayPConfMenu(); /* Update menu before catching key */
                setNextKey(editPlayer, 'l');
                break;
            case 2:
                (&players[editPlayer])->rkey = SDLK_CLEAR;
                displayPConfMenu(); /* Update menu before catching key */
                setNextKey(editPlayer, 'r');
                break;
            case 3:
                (&players[editPlayer])->wkey = SDLK_CLEAR;
                displayPConfMenu(); /* Update menu before catching key */
                setNextKey(editPlayer, 'w');
                break;
            case 4:
                curScene = curScene->parentScene;
                break;
            default:
                break;
        }
        keyDown[SDLK_SPACE] = keyDown[SDLK_RETURN] = 0;
    } else if (keyDown[SDLK_LEFT]) {
        keyDown[SDLK_LEFT] = 0;
        if (menuPConf.choice == 0) {
            setColor(editPlayer, 0);
        }
    } else if (keyDown[SDLK_RIGHT]) {
        if (menuPConf.choice == 0) {
            setColor(editPlayer, 1);
        }
        keyDown[SDLK_RIGHT] = 0;
    }
    handleMenu(&menuPConf);
}

/**
 * Display the player configuration screen.
 */
void displayPConfMenu(void)
{
    char *c[menuPConf.choices];

    char s1[MENU_BUF] = "LEFT KEY: ";
    char s2[MENU_BUF] = "RIGHT KEY: ";
    char s3[MENU_BUF] = "WEAPON KEY: ";
    char s4[MENU_BUF] = "~ PLAYER ";

    char tmpKey[MAX_KEYNAME];
    char *lkey = keyName((&players[editPlayer])->lkey);
    char *rkey = keyName((&players[editPlayer])->rkey);
    char *wkey = keyName((&players[editPlayer])->wkey);
    snprintf(tmpKey, MAX_KEYNAME * sizeof(char), "%s", lkey);
    free(lkey);
    strncat(s1, tmpKey, MAX_KEYNAME - 1);
    snprintf(tmpKey, MAX_KEYNAME * sizeof(char), "%s", rkey);
    free(rkey);
    strncat(s2, tmpKey, MAX_KEYNAME - 1);
    snprintf(tmpKey, MAX_KEYNAME * sizeof(char), "%s", wkey);
    free(wkey);
    strncat(s3, tmpKey, MAX_KEYNAME - 1);
    snprintf(tmpKey, 4 * sizeof(char), "%d ~", editPlayer + 1);
    strncat(s4, tmpKey, 3);

    c[0] = s4;
    c[1] = s1;
    c[2] = s2;
    c[3] = s3;
    c[4] = "BACK";

    displayMenu(c, &menuPConf);

    if (SDL_Flip(screen) == -1) {
        exit(1);
    }
}

/**
 * Handles a general menu.
 *
 * @param m The menu to do handling of.
 */
void handleMenu(struct menu *m)
{
    if (keyDown[SDLK_DOWN]) {
        keyDown[SDLK_DOWN] = 0;
        ++(m->choice);
    } else if (keyDown[SDLK_UP]) {
        keyDown[SDLK_UP] = 0;
        --(m->choice);
    }
    if (m->choice >= m->choices) {
        m->choice = 0;
    } else if (m->choice < 0) {
        m->choice = m->choices - 1;
    }
}

/**
 * Displays a general menu.
 * The menu structure is separated from the strings, so that arbitrary
 * strings can be used.
 *
 * @param c All the strings to be shown in the menu.
 * @param m The menu to be shown.
 */
void displayMenu(char *c[], struct menu *m)
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
            (WINDOW_W / 2)                          /* offset */
                - (msg->w / 2)                      /* centralize */
                ,
            (WINDOW_H / 2)                          /* offset */
                + (mid * ((msg->h / 2) + SPACEMOD)) /* spacing */
                - ((m->choices % 2) - 1)             /* halfspace */
                * (SPACEMOD / 2)
                - (msg->h / 2)                      /* centralize */
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

    if (fullscreen) {
        screen = SDL_SetVideoMode(WINDOW_W, WINDOW_H, SCREEN_BPP,
                SDL_SWSURFACE | SDL_FULLSCREEN);
    } else {
        screen = SDL_SetVideoMode(WINDOW_W, WINDOW_H, SCREEN_BPP,
                SDL_SWSURFACE | SDL_RESIZABLE);
    }
    if (screen == NULL) return 0;

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

    if (loadedImage != NULL) {
        optimizedImage = SDL_DisplayFormatAlpha(loadedImage);
    }

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
    c->r = 0x00; c->g = 0x00; c->b = 0xFF; ++c; /* Blue */
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
        return 0;
    }
    if (TTF_Init() == -1) {
        return 0;
    }

    memset(broadcast, '\0', BROADC_LIMIT * sizeof(SDL_Surface*));

    SDL_ShowCursor(SDL_DISABLE);
    SDL_WM_SetCaption("Zatacka X", NULL);

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

    if ((arrows = loadImage("data/gfx/arrowsheet.png")) == NULL) return 0;
    if ((ball = loadImage("data/gfx/ball.png")) == NULL) return 0;
    if ((font_menu = TTF_OpenFont("data/fonts/jura/JuraLight.ttf",
                    MENU_FONT_SIZE)) == NULL) {
        return 0;
    }
    if ((font_menub = TTF_OpenFont("data/fonts/jura/JuraMedium.ttf",
                    MENU_FONT_SIZE)) == NULL) {
        return 0;
    }
    if ((font_score = TTF_OpenFont("data/fonts/OCR-A/tarzeau-OCR-A.ttf",
                    MENU_FONT_SIZE)) == NULL) {
        return 0;
    }
    if ((font_broadc = TTF_OpenFont("data/fonts/ankacoder/AnkaCoder-r.ttf",
                    BROADC_FONT_SIZE)) == NULL) {
        return 0;
    }
    if ((font_broadcb = TTF_OpenFont("data/fonts/ankacoder/AnkaCoder-b.ttf"
                    , BROADC_FONT_SIZE)) == NULL) {
        return 0;
    }

#ifdef DEBUG
    confirmLoading("arrowsheet.png", arrows);
    confirmLoading("ball.png", ball);
#endif

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
                arrows->format->Rmask, arrows->format->Gmask,
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
#ifdef DEBUG
        fprintf(stderr, "Couldn't open file '%s' for writing.\n",
                filename);
#endif
        return;
    } else {
        for (i = 0; i < sizeof(settings) / sizeof(bool*); ++i) {
            fprintf(savefile, "%s = %d\n", settingNames[i], *settings[i]);
        }
        for (i = 0; i < MAX_PLAYERS; ++i) {
            fprintf(savefile, "%dc = %d\n", i + 1, (&players[i])->color);
            fprintf(savefile, "%dl = %d\n", i + 1, (&players[i])->lkey);
            fprintf(savefile, "%dr = %d\n", i + 1, (&players[i])->rkey);
            fprintf(savefile, "%dw = %d\n", i + 1, (&players[i])->wkey);
        }
    }

#ifdef DEBUG
    if (ferror(savefile)) {
        fprintf(stderr, "Error writing to file '%s'.\n", filename);
    }
#endif

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
#ifdef DEBUG
        fprintf(stderr, "Couldn't restore settings from file %s.\n",
                filename);
#endif
        return;
    } else {

        char settingHandle[STRBUF];
        unsigned int settingParam;
        unsigned int i;
        int line = 0;
        bool valid;

        for (;;) {
            if ((fscanf(savefile, "%s = %u\n", settingHandle,
                            &settingParam)) != EOF) {
                valid = 0;
                for (i = 0; i < sizeof(settings) / sizeof(bool*); ++i) {
                    if (strncmp(settingNames[i], settingHandle, STRBUF)
                            == 0) {
                        /* We have a matched setting */
                        *settings[i] = settingParam;
                        valid = 1;
                        break;
                    } else if (isdigit(settingHandle[0])) {
                        switch (settingHandle[1]) {
                            case 'c': /* Color */
                                (&players[settingHandle[0] - '0' - 1])->
                                    color = settingParam;
                                valid = 1;
                                break;
                            case 'l': /* Left key */
                                (&players[settingHandle[0] - '0' - 1])->
                                    lkey = settingParam;
                                valid = 1;
                                break;
                            case 'r': /* Right key */
                                (&players[settingHandle[0] - '0' - 1])->
                                    rkey = settingParam;
                                valid = 1;
                                break;
                            case 'w': /* Weapon key */
                                (&players[settingHandle[0] - '0' - 1])->
                                    wkey = settingParam;
                                valid = 1;
                                break;
                            default:
                                break;
                        }
                        break;
                    }
                }
                if (valid == 0) {
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
    char *keyname = malloc(MAX_KEYNAME * sizeof(char));
    memset(keyname, '\0', MAX_KEYNAME * sizeof(char));

    if ((key >= SDLK_a && key <= SDLK_z)
            || (key >= SDLK_0 && key <= SDLK_9)) {
        snprintf(keyname, 2, "%c", key);
    } else if (key >= SDLK_F1 && key <= SDLK_F15) {
        snprintf(keyname, 4, "F%d", key - SDLK_F1 + 1);
    } else {
        switch (key) {
            case SDLK_UNKNOWN:
                snprintf(keyname, 5, "none"); break;
            case SDLK_LEFT:
                snprintf(keyname, 5, "left"); break;
            case SDLK_RIGHT:
                snprintf(keyname, 6, "right"); break;
            case SDLK_UP:
                snprintf(keyname, 3, "up"); break;
            case SDLK_DOWN:
                snprintf(keyname, 5, "down"); break;
            case SDLK_DELETE:
                snprintf(keyname, 4, "del"); break;
            case SDLK_INSERT:
                snprintf(keyname, 4, "ins"); break;
            case SDLK_HOME:
                snprintf(keyname, 5, "home"); break;
            case SDLK_END:
                snprintf(keyname, 4, "end"); break;
            case SDLK_PAGEUP:
                snprintf(keyname, 6, "pg up"); break;
            case SDLK_PAGEDOWN:
                snprintf(keyname, 6, "pg dn"); break;
            case SDLK_RSHIFT:
                snprintf(keyname, 8, "r-shift"); break;
            case SDLK_LSHIFT:
                snprintf(keyname, 8, "l-shift"); break;
            case SDLK_RCTRL:
                snprintf(keyname, 7, "r-ctrl"); break;
            case SDLK_LCTRL:
                snprintf(keyname, 7, "l-ctrl"); break;
            case SDLK_RALT:
                snprintf(keyname, 6, "r-alt"); break;
            case SDLK_LALT:
                snprintf(keyname, 6, "l-alt"); break;
            case SDLK_RSUPER:
                snprintf(keyname, 8, "r-super"); break;
            case SDLK_LSUPER:
                snprintf(keyname, 8, "l-super"); break;
            case SDLK_TAB:
                snprintf(keyname, 4, "tab"); break;
            case SDLK_PERIOD:
                snprintf(keyname, 2, "."); break;
            case SDLK_COMMA:
                snprintf(keyname, 2, ","); break;
            case SDLK_MINUS:
                snprintf(keyname, 2, "-"); break;
            case SDLK_PLUS:
                snprintf(keyname, 2, "+"); break;
            case SDLK_BACKSLASH:
                snprintf(keyname, 2, "\\"); break;
            case SDLK_LESS:
                snprintf(keyname, 2, "<"); break;
            case SDLK_BACKSPACE:
                snprintf(keyname, 8, "b-space"); break;
            case SDLK_RETURN:
                snprintf(keyname, 6, "enter"); break;
            case SDLK_SPACE:
                snprintf(keyname, 6, "space"); break;
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

    saveSettings(".zataconf");

    exit(status);
}

int main(int argc, char *argv[])
{
    WINDOW_W = DEFAULT_WINDOW_W;
    WINDOW_H = DEFAULT_WINDOW_H;

    initPlayers1();
    restoreSettings(".zataconf");

    if (!init()) {
        return 1;
    }

    if (!initScreen()) {
        return 1;
    }

    if (!loadFiles()) {
        fprintf(stderr, "Failed to load files.\n");
        return 1;
    }

    initColors();
    initMainMenu();

    curScene = &mainMenu;

    for (;;) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                int k = event.key.keysym.sym;
                if (k != SDLK_ESCAPE) {
#ifdef DEBUG
                    printf("Pressed: %c (%d)\n", k, k);
#endif
                    keyDown[k] = 1;
                } else {
                    if (curScene == &game || curScene == &gameStart) {
                        cleanHitMap();
                        if (broadcasts) cleanBroadcast();
                    } else if (curScene == &settingsMenu) {
                        initMainMenu();
                    } else if (curScene->parentScene == NULL) {
                        exitGame(0);
                    }
                    curScene = curScene->parentScene;
                }
            } else if (event.type == SDL_KEYUP) {
                keyDown[event.key.keysym.sym] = 0;
            } else if (event.type == SDL_QUIT) {
                exitGame(0);
            } else if (event.type == SDL_VIDEORESIZE) {

                WINDOW_W = event.resize.w;
                WINDOW_H = event.resize.h;

                if (!initScreen()) {
                    return 1;
                }
            }
        }
        curScene->logicFunc();
        curScene->displayFunc();
    }

    return 0;
}
