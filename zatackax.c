#include "zatackax.h"

SDL_Rect arrowClip[32];
SDL_Surface *screen = NULL;
SDL_Surface *msg = NULL;
SDL_Surface *image = NULL;
SDL_Surface *arrows = NULL;
SDL_Surface *ball = NULL;
SDL_Surface *broadcast[BROADC_LIMIT];
SDL_Event event;

void displayVoid(void) {}

struct scene {
    void (*logicFunc)(void);
    void (*displayFunc)(void);
    struct scene *parentScene;
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

struct scene *curScene = NULL;

void SDL_SurfaceInfo(char *name, SDL_Surface *sprite)
{
    printf("Loaded: %s\t(w:%d h:%d bpp:%d)\n", name, sprite->w, sprite->h,
            sprite->format->BitsPerPixel);
}

void putPixel(int x, int y, SDL_Color c, unsigned char *target)
{
    target += 4 * ((WINDOW_W * y) + x);
    target[0] = c.b;
    target[1] = c.g;
    target[2] = c.r;
}

void colorFill(SDL_Color c, SDL_Surface *sprite)
{
    int xx, yy;
    unsigned char *target = sprite->pixels;

    for (yy = 0; yy < sprite->h; yy++) {
        for (xx = 0; xx < sprite->w; xx++, target += 4) {
            target[0] *= c.b / 255.0;
            target[1] *= c.g / 255.0;
            target[2] *= c.r / 255.0;
        }
    }
}

void drawScores()
{
    int i;
    struct player *p;
    char score_str[SCORE_BUF];
    SDL_Surface *scoreText;

    for (i = 0; i < nPlayers; i++) {
        SDL_Rect offset = {7, SCORE_SPACING * i, 0, 0};
        p = &players[i];
        snprintf(score_str, SCORE_BUF, "%d", p->score);
        scoreText = TTF_RenderUTF8_Shaded(font_score, score_str, p->c,
                cMenuBG);
        SDL_BlitSurface(scoreText, NULL, screen, &offset);
        SDL_FreeSurface(scoreText);
    }

    if (broadcasts) {
        for (i = 0; i < BROADC_LIMIT; i++) {
            if (broadcast[i] != NULL) {
                SDL_Rect offset = {WINDOW_W - broadcast[i]->w - 4,
                    WINDOW_H - (broadcast[i]->h * (i + 1)), 0, 0};
                SDL_BlitSurface(broadcast[i], NULL, screen, &offset);
            }
        }
    }
}

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

    SDL_SurfaceInfo("arrowsheet.png", arrows);
    SDL_SurfaceInfo("ball.png", ball);

    /* Clip arrow sprite sheet */
    for (y = i = 0; y < 128; y += 16) {
        for (x = 0; x < 64; x += 16, i++) {
            arrowClip[i].x = x;
            arrowClip[i].y = y;
            arrowClip[i].w = 16;
            arrowClip[i].h = 16;
        }
    }

    /* Make arrow copies */
    parrows = malloc(MAX_PLAYERS * sizeof(SDL_Surface));
    p = parrows;
    for (i = 0; i < MAX_PLAYERS; i++, p++) {
        *p = SDL_CreateRGBSurface(arrows->flags, arrows->w,
                arrows->h, arrows->format->BitsPerPixel, 
                arrows->format->Rmask, arrows->format->Gmask,
                arrows->format->Bmask, 0);
    }

    /* Make ball copies */
    pballs = malloc((MAX_PLAYERS + 1) * sizeof(SDL_Surface));
    p = pballs;
    for (i = 0; i < MAX_PLAYERS + 1; i++, p++) {
        *p = SDL_CreateRGBSurface(ball->flags, ball->w,
                ball->h, ball->format->BitsPerPixel, 
                ball->format->Rmask, ball->format->Gmask,
                ball->format->Bmask, 0);
    }

    return 1;
}

struct vel spawn()
{
    long rnd1, rnd2, rnd3, rnd4;
    struct vel initPos;
    Uint32 timeseed = SDL_GetTicks();

    if (randomizer > 100000) {
        randomizer = 1;
    }

    srand(timeseed);
    rnd1 = rand() * randomizer;
    randomizer++;
    rnd2 = rand() * randomizer;
    randomizer += randomizer;
    rnd3 = rand() * randomizer;
    randomizer++;
    rnd4 = rand() * randomizer;
    randomizer++;
    initPos.x = SPAWN_SPACE_MIN
        + (rnd1 % (WINDOW_W - (2 * SPAWN_SPACE_MIN)));
    initPos.y = SPAWN_SPACE_MIN
        + (rnd2 % (WINDOW_H - (2 * SPAWN_SPACE_MIN)));
    initPos.dir = rnd3 * (2 * PI) / RAND_MAX;
    if (initPos.dir < 0) initPos.dir *= -1;
    initPos.holecount = (rnd4 % (HOLE_FREQ - HOLE_FIRST_DELAY));

    return initPos;
}

void cleanBroadcast(void)
{
    int i;
    for (i = 0; i < BROADC_LIMIT; i++) {
        SDL_FreeSurface(broadcast[i]);
    }
    memset(broadcast, '\0', BROADC_LIMIT * sizeof(SDL_Surface*));
}

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

void refreshGameScreen()
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

    for (yy = 0; yy < WINDOW_H; yy++) {
        for (xx = 0; xx < WINDOW_W; xx++, target += 4) {
            char charat = hitmap[sizeof(bool)
                * ((WINDOW_W * yy) + xx)]; 
            if (charat != 0) {
                struct player *p;
                if (charat > MAX_PLAYERS) {
                    p = &players[charat - MAX_PLAYERS - 1];
                } else {
                    p = &players[charat - 1];
                }
                target[0] = p->c.b;
                target[1] = p->c.g;
                target[2] = p->c.r;
            }
        }
    }

    SDL_UnlockSurface(screen);

    if (SDL_Flip(screen) == -1) {
        exit(1);
    }
}

void makeBroadcast(struct player *p, unsigned char killer)
{
    int i;
    char anomsg[2][BROADC_BUF];
    SDL_Surface *tmp[3];
    int broadw = 0;
    int nbroad = 0;
    int killed = p->active;

    SDL_FreeSurface(broadcast[BROADC_LIMIT - 1]);
    for (i = BROADC_LIMIT - 1; i > 0; i--) {
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
                p->c, cMenuBG);
        tmp[1] = TTF_RenderUTF8_Shaded(font_broadc,
                " hit the wall", cBroadcast, cMenuBG);

    } else if (killer == killed) { /* Suicide */

        nbroad = 2;
        tmp[0] = TTF_RenderUTF8_Shaded(font_broadcb, anomsg[0],
                p->c, cMenuBG);
        tmp[1] = TTF_RenderUTF8_Shaded(font_broadc,
                " committed suicide", cBroadcast, cMenuBG);

    } else { /* Killed */

        struct player *pk = &players[killer - 1];
        nbroad = 3;

        snprintf(anomsg[1], BROADC_BUF, "Player%d", killer);

        tmp[0] = TTF_RenderUTF8_Shaded(font_broadcb, anomsg[0],
                p->c, cMenuBG);
        tmp[1] = TTF_RenderUTF8_Shaded(font_broadc,
                " crashed into ", cBroadcast, cMenuBG);
        tmp[2] = TTF_RenderUTF8_Shaded(font_broadcb, anomsg[1],
                pk->c, cMenuBG);
    }

    for (i = 0; i < nbroad; i++) broadw += tmp[i]->w;
    broadcast[0] = SDL_CreateRGBSurface(SDL_HWSURFACE,
            broadw, tmp[0]->h, SCREEN_BPP, 0, 0, 0, 0);

    for (i = 0; i < nbroad; i++) {
        int j;
        SDL_Rect offset = {0, 0, 0, 0};
        for (j = i; j > 0; j--) offset.x += tmp[j-1]->w;
        SDL_BlitSurface(tmp[i], NULL, broadcast[0], &offset);
    }

    for (i = 0; i < nbroad; i++) SDL_FreeSurface(tmp[i]);
}

void killPlayer(unsigned char killed, unsigned char killer)
{
    int i;

    struct player *p = &players[killed - 1];
    alivecount--;
    p->alive = 0;

    for (i = 0; i < MAX_PLAYERS; i++) {
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

void newRound(void)
{
    int i;

    cleanHitMap();

    alivecount = nPlayers;
    countdown = START_ROUND_WAIT;
    memset(hitmap, '\0', sizeof(bool) * WINDOW_W * WINDOW_H);

    curScene = &gameStart;

    printf(" -- New round! --  ( Score: ");
    for (i = 0; i < MAX_PLAYERS; i++) {
        struct player *p = &players[i];
        if (p->active) {
            printf("%d ", p->score);
            if (nPlayers == 1) players[0].score = 0;
            respawn(p);
        } else {
            printf(")\n");
            return;
        }
    }

    printf(")\n");
}

void addToHitMap(unsigned int x, unsigned int y, unsigned char player,
        unsigned char modifier)
{
    int i, j;

#ifdef DEBUG
    fprintf(stderr, "Added to hitmap: %d, %d...\n", x, y);
#endif

    SDL_LockSurface(screen);

    for (i = -TOLERANCE; i < TOLERANCE; i++) {
        for (j = -TOLERANCE; j < TOLERANCE; j++) {

            int xpx = x + i;
            int ypx = y + j;

            if (xpx >= 0 && xpx < (int)WINDOW_W && ypx >= 0
                    && ypx < (int)WINDOW_H) {

                unsigned char *hit =
                    &hitmap[sizeof(bool) * ((WINDOW_W * ypx) + xpx)];

                putPixel(xpx, ypx, (&players[player-1])->c,
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
                        if (*hit > MAX_PLAYERS) {
                            printf("Player %d crashed into Player %d!\n",
                                    player, *hit - MAX_PLAYERS);
                            killPlayer(player, *hit - MAX_PLAYERS);
                        } else {
                            printf("Player %d crashed into Player %d!\n",
                                    player, *hit);
                            killPlayer(player, *hit);
                        }
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

void updateHitMap()
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

void initHitMap(unsigned int w, unsigned int h)
{
    size_t len = sizeof(bool) * w * h;
    hitmap = (bool*)malloc(len);
    memset(hitmap, '\0', len);
    recents = NULL;
}

void exitGame(int status)
{
    free(hitmap);
    free(parrows);
    free(pballs);

    screen = SDL_SetVideoMode(WINDOW_W, WINDOW_H, SCREEN_BPP,
            SDL_SWSURFACE);

    printf("Bye!\n");

    exit(status);
}

int init(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        return 0;
    }
    if (TTF_Init() == -1) {
        return 0;
    }

    memset(broadcast, '\0', BROADC_LIMIT * sizeof(SDL_Surface*));

    SDL_WM_SetCaption("Zatacka X", NULL);

    return 1;
}

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

void colorBalls(void)
{
    int i;
    SDL_Color inactive = {127, 127, 127, 0};
    SDL_Surface **s = pballs;
    struct player *p = &players[0];
    for (i = 0; i < MAX_PLAYERS; i++, p++, s++) {
        SDL_BlitSurface(ball, NULL, *s, NULL);
        SDL_LockSurface(*s);
        colorFill(p->c, *s);
        SDL_UnlockSurface(*s);
    }

    SDL_BlitSurface(ball, NULL, *s, NULL);
    SDL_LockSurface(*s);
    colorFill(inactive, *s);
    SDL_UnlockSurface(*s);
}

void initMainMenu(void)
{
    menuchoice = 0;
    colorBalls();
}

void initPlayers1(void)
{
    int i;
    struct player *p = &players[0];
    SDL_Surface **s = parrows;

    for (i = 0; i < MAX_PLAYERS; i++, p++, s++) {
        p->score = 0;
        switch (i) {
            case 0:
                p->lkey = SPEC_LEFT; p->rkey = SPEC_RIGHT;
                p->c.r = 0xFF; p->c.g = 0x00; p->c.b = 0x00;
                break;
            case 1:
                p->lkey = 'a'; p->rkey = 's';
                p->c.r = 0x00; p->c.g = 0x00; p->c.b = 0xFF;
                break;
            case 2:
                p->lkey = 'z'; p->rkey = 'x';
                p->c.r = 0x00; p->c.g = 0xFF; p->c.b = 0x00;
                break;
            case 3:
                p->lkey = 'q'; p->rkey = 'w';
                p->c.r = 0xFF; p->c.g = 0xFF; p->c.b = 0x00;
                break;
            case 4:
                p->lkey = 'e'; p->rkey = 'r';
                p->c.r = 0xFF; p->c.g = 0x00; p->c.b = 0xFF;
                break;
            case 5:
                p->lkey = 'd'; p->rkey = 'f';
                p->c.r = 0x00; p->c.g = 0xFF; p->c.b = 0xFF;
                break;
            case 6:
                p->lkey = 'c'; p->rkey = 'v';
                p->c.r = 0xFF; p->c.g = 0xFF; p->c.b = 0xFF;
                break;
            case 7:
                p->lkey = 't'; p->rkey = 'y';
                p->c.r = 0x60; p->c.g = 0x60; p->c.b = 0x60;
                break;
            default:
                break;
        }

        /* Assign arrows */
        SDL_BlitSurface(arrows, NULL, *s, NULL);
        SDL_LockSurface(*s);
        colorFill(p->c, *s);
        SDL_UnlockSurface(*s);
        p->arrow = *s;
    }
}

void initPlayers2(void)
{
    int i;
    struct player *p = &players[0];

    for (i = 0; i < nPlayers; i++, p++) {
        p->active = i + 1;
    }
    for (; i < MAX_PLAYERS; i++, p++) {
        p->active = 0;
    }
}

void keyPress(unsigned char key) {
    keyDown[key] = 1;
}

void keyRelease(unsigned char key)
{
    keyDown[key] = 0;
}

void logicGame(void)
{
    int i;
    Uint32 timenow = SDL_GetTicks();
    delta = timenow - prevtime;
    prevtime = timenow;

#ifdef DEBUG
    fprintf(stderr, "delta: %d    prevtime: %d\n", delta, prevtime);
#endif

    if (delta > 35) {
        delta = 35;
    }

    for (i = 0; i < MAX_PLAYERS; i++) {
        if (players[i].active) {

            struct player *p = &players[i];

            if (p->alive) {

                unsigned int curx, cury;

                if (keyDown[p->lkey]) {
                    p->dir -= 0.0022 * delta;
                } else if (keyDown[p->rkey]) {
                    p->dir += 0.0022 * delta;
                }

                p->posx += 0.1 * cos(p->dir) * delta;
                p->posy += 0.1 * sin(p->dir) * delta;
                if (nPlayers == 1) players[0].score += delta;

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
            } else if (alivecount <= 1) {
                SDL_FreeSurface(screen);
                newRound();
            }
        } else {
            break;
        }
    }
    updateHitMap();
    SDL_UnlockSurface(screen);
}

void logicGameStart(void)
{
    Uint32 timenow = SDL_GetTicks();
    delta = timenow - prevtime;
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

void displayGameStart(void)
{
    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format,
                0x00, 0x00, 0x00));

    drawScores();

    if ((countdown % (START_ROUND_WAIT / 4)) > (START_ROUND_WAIT / 8)) {
        int i;
        for (i = 0; i < MAX_PLAYERS; i++) {
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

void displayMainMenu(void)
{
    int i;
    char *c[MENU_MAIN_CHOICES];
    int mid = -(MENU_MAIN_CHOICES / 2);
    c[0] = "START GAME";
    c[1] = "SETTINGS";
    c[2] = "EXIT";

    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format,
                0x00, 0x00, 0x00));

    for (i = 0; i < MENU_MAIN_CHOICES; i++, mid++) {

        if (i == menuchoice) {
            msg = TTF_RenderUTF8_Solid(font_menu, c[i], cMenuTextH);
        } else {
            msg = TTF_RenderUTF8_Solid(font_menu, c[i], cMenuText);
        }

        SDL_Rect offset = {
            (WINDOW_W / 2)                          /* offset */
                - (msg->w / 2)                      /* centralize */
                ,
            (WINDOW_H / 2)                          /* offset */
                + (mid * ((msg->h / 2) + SPACEMOD)) /* spacing */
                - (msg->h / 2)                      /* centralize */
                , 0, 0};

        SDL_BlitSurface(msg, NULL, screen, &offset);
        SDL_FreeSurface(msg);
    }

    /* This could/should be made smoother... */
    for (i = 0; i < nPlayers; i++) {
        SDL_Rect offset = {
            (WINDOW_W / 2)              /* window offset */
                - 55                    /* temp. offset */            
                + (i - nPlayers) * 30   /* player modifier */
                ,
            (WINDOW_H / 2)              /* window offset */
                - 37                    /* temp. offset */
                , 0, 0};
        SDL_BlitSurface(pballs[i], NULL, screen, &offset);
    }

    for (i = nPlayers; i < MAX_PLAYERS; i++) {
        SDL_Rect offset = {
            (WINDOW_W / 2)              /* window offset */
                + 64                    /* temp. offset */            
                + (i - nPlayers) * 30   /* player modifier */
                ,
            (WINDOW_H / 2)              /* window offset */
                - 37                    /* temp. offset */
                , 0, 0};
        SDL_BlitSurface(pballs[MAX_PLAYERS], NULL, screen, &offset);
    }

    if (SDL_Flip(screen) == -1) {
        exit(1);
    }
}

void displaySettingsMenu(void)
{
    int i;
    char *c[MENU_SETTINGS_CHOICES];
    int mid = -(MENU_MAIN_CHOICES / 2);

    char s1[MENU_BUF] = "FULLSCREEN ";
    char s2[MENU_BUF] = "HOLES ";
    char s3[MENU_BUF] = "BROADCASTS ";
    strncat(s1, fullscreen ON_OFF, MENU_BUF - strlen(s1));
    strncat(s2, holes ON_OFF, MENU_BUF - strlen(s2));
    strncat(s3, broadcasts ON_OFF, MENU_BUF - strlen(s3));
    c[0] = s1;
    c[1] = s2;
    c[2] = s3;
    c[3] = "BACK";

    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format,
                0x00, 0x00, 0x00));

    for (i = 0; i < MENU_SETTINGS_CHOICES; i++, mid++) {

        if (i == menuchoice_s) {
            msg = TTF_RenderUTF8_Solid(font_menu, c[i], cMenuTextH);
        } else {
            msg = TTF_RenderUTF8_Solid(font_menu, c[i], cMenuText);
        }

        SDL_Rect offset = {
            (WINDOW_W / 2)                          /* offset */
                - (msg->w / 2)                      /* centralize */
                ,
            (WINDOW_H / 2)                          /* offset */
                + (mid * ((msg->h / 2) + SPACEMOD)) /* spacing */
                - (msg->h / 2)                      /* centralize */
                , 0, 0};

        SDL_BlitSurface(msg, NULL, screen, &offset);
        SDL_FreeSurface(msg);
    }

    if (SDL_Flip(screen) == -1) {
        exit(1);
    }
}

void logicMainMenu(void)
{
    if (keyDown[32] || keyDown[13]) {
        switch (menuchoice) {
            case 0:
                initPlayers2();
                newRound();
                curScene = &gameStart;
                break;
            case 1:
                curScene = &settingsMenu;
                break;
            case 2:
                exitGame(0);
                break;
            default:
                break;
        }
        keyDown[32] = keyDown[13] = 0;
    }
    if (keyDown[SPEC_DOWN]) {
        keyDown[SPEC_DOWN] = 0;
        menuchoice++;
    } else if (keyDown[SPEC_UP]) {
        keyDown[SPEC_UP] = 0;
        menuchoice--;
    } else if (keyDown[SPEC_LEFT]) {
        keyDown[SPEC_LEFT] = 0;
        if (menuchoice == 0 && nPlayers > 1) nPlayers--;
    } else if (keyDown[SPEC_RIGHT]) {
        keyDown[SPEC_RIGHT] = 0;
        if (menuchoice == 0 && nPlayers < MAX_PLAYERS) nPlayers++;
    }
    if (menuchoice >= MENU_MAIN_CHOICES) {
        menuchoice = 0;
    } else if (menuchoice < 0) {
        menuchoice = MENU_MAIN_CHOICES - 1;
    }
}

void logicSettingsMenu(void)
{
    if (keyDown[32] || keyDown[13]) {
        switch (menuchoice_s) {
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
            case MENU_SETTINGS_CHOICES - 1: /* Back */
                keyDown[32] = keyDown[13] = 0;
                initMainMenu();
                menuchoice_s = 0;
                curScene = curScene->parentScene;
                break;
            default:
                break;
        }
        keyDown[32] = keyDown[13] = 0;
    }
    if (keyDown[SPEC_DOWN]) {
        keyDown[SPEC_DOWN] = 0;
        menuchoice_s++;
    } else if (keyDown[SPEC_UP]) {
        keyDown[SPEC_UP] = 0;
        menuchoice_s--;
    } else if (keyDown[SPEC_LEFT]) {
        keyDown[SPEC_LEFT] = 0;
    } else if (keyDown[SPEC_RIGHT]) {
        keyDown[SPEC_RIGHT] = 0;
    }
    if (menuchoice_s >= MENU_SETTINGS_CHOICES) {
        menuchoice_s = 0;
    } else if (menuchoice_s < 0) {
        menuchoice_s = MENU_SETTINGS_CHOICES - 1;
    }
}

int main(void)
{
    WINDOW_W = DEFAULT_WINDOW_W;
    WINDOW_H = DEFAULT_WINDOW_H;

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

    initPlayers1();
    initPlayers2();

    initMainMenu();

    curScene = &mainMenu;

    for (;;) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                int k = event.key.keysym.sym;
                if (k == SDLK_ESCAPE) {
                    if (curScene == &game || curScene == &gameStart) {
                        cleanHitMap();
                        if (broadcasts) cleanBroadcast();
                        initPlayers1();
                    } else if (curScene == &settingsMenu) {
                        initMainMenu();
                    } else if (curScene->parentScene == NULL) {
                        exitGame(0);
                    }
                    curScene = curScene->parentScene;
                } else {
#ifdef DEBUG
                    printf("Pressed: %c (%d)\n", k, k);
#endif
                    keyDown[k] = 1;
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
