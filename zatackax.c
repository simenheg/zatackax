#include "zatackax.h"

/* GIMP RGB C-Source image dump (ball.c) */
static const struct {
    int width;
    int height;
    int bytes_per_pixel; /* 3:RGB, 4:RGBA */ 
    char pixel_data[20 * 20 * 3 + 1];
} ball = {
    20, 20, 3,
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0"
        "\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0"
        "\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\0\0\0\0"
        "\0\0\0\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\0\0\0\0\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377"
        "\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377"
        "\0\0\377\0\0\377\0\0\0\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\0\0\0\377\0\0\377\0\0"
        "\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0"
        "\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\0\0\0\0\0\0\377"
        "\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377"
        "\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\0\0"
        "\0\0\0\0\0\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377"
        "\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377"
        "\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\377\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\377\0"
        "\0\377\0\0\377\0\0\377\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0",
};

SDL_Rect arrowClip[32];

SDL_Surface *screen = NULL;
SDL_Surface *msg = NULL;
SDL_Event event;
SDL_Surface *image;
SDL_Surface *arrows;

void (*logicFunc)(void);
void (*displayFunc)(void);

void SDL_SurfaceInfo(char *name, SDL_Surface *sprite)
{
    printf("Loaded: %s (w:%d h:%d bpp:%d)\n", name, sprite->w, sprite->h,
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

    for (yy = 0; yy < 128; yy++) {
        for (xx = 0; xx < 64; xx++, target += 4) {
            target[0] *= c.b / 255.0;
            target[1] *= c.g / 255.0;
            target[2] *= c.r / 255.0;
        }
    }
}

void drawCircle(int x, int y, SDL_Color c)
{
    int xx, yy;
    unsigned char *target = screen->pixels;
    target += 4 * ((WINDOW_W * y) + x);

    for (yy = 0; yy < ball.height; yy++, target += WINDOW_W*4) {
        target -= ball.width * 4;
        for (xx = 0; xx < ball.width; xx++, target += 4) {
            target[0] = c.b;
            target[1] = c.g;
            target[2] = c.r;
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
        p = &players[i];
        sprintf(score_str, "%d", p->score);
        scoreText = TTF_RenderUTF8_Shaded(font_score, score_str, p->c,
                cMenuBG);
        SDL_Rect offset = {7, SCORE_SPACING * i};
        SDL_BlitSurface(scoreText, NULL, screen, &offset);
        SDL_FreeSurface(scoreText);
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
    if ((font_menu = TTF_OpenFont("data/fonts/jura/JuraLight.ttf",
                    MENU_FONT_SIZE)) == NULL) {
        return 0;
    }
    /*
    if ((font_score = TTF_OpenFont("data/fonts/ankacoder/AnkaCoder-r.ttf",
                    MENU_FONT_SIZE)) == NULL) {
        return 0;
    }
    */
    if ((font_score = TTF_OpenFont("data/fonts/OCR-A/tarzeau-OCR-A.ttf",
                    MENU_FONT_SIZE)) == NULL) {
        return 0;
    }

    SDL_SurfaceInfo("arrowsheet.png", arrows);

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

    return 1;
}

struct recentMapPiece {
    Uint32 count;
    int x, y;
    struct recentMapPiece *next;
};

struct pos {
    GLdouble x, y;
};

struct vel {
    GLdouble x, y, dir;
    unsigned int holecount;
};

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

/* This needs some fixing... !!! */
void cleanHitMap()
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

    recents = NULL;
}

void refreshGameScreen()
{
    unsigned char *target;

    SDL_UnlockSurface(screen);
    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format,
                0x00, 0x00, 0x00));

    drawScores();

    SDL_LockSurface(screen);

    target = screen->pixels;

    unsigned int xx;
    unsigned int yy;
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

void killPlayer(struct player *p)
{
    int i;

    alivecount--;
    p->alive = 0;

    for (i = 0; i < MAX_PLAYERS; i++) {
        struct player *p = &players[i];
        if (p->active) {
            if (p->alive) {
                p->score++;
            }
        } else {
            break;
        }
    }

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

    logicFunc = logicGameStart;
    displayFunc = displayGameStart;

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

void addToHitMap(unsigned int x, unsigned int y, unsigned char player)
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

                if (*hit == 0 || *hit == player + MAX_PLAYERS) {
                    struct recentMapPiece *new
                        = malloc(sizeof(struct recentMapPiece));
                    *hit = player + MAX_PLAYERS;
                    new->count = COUNTDOWN_TOLERANCE;
                    new->x = xpx;
                    new->y = ypx;
                    new->next = recents;
                    recents = new;
                } else {
                    if (player == *hit) {
                        printf("Player %d committed suicide!\n", player);
                    } else {
                        if (*hit > MAX_PLAYERS) {
                            printf("Player %d crashed into Player %d!\n",
                                    player, *hit - MAX_PLAYERS);
                        } else {
                            printf("Player %d crashed into Player %d!\n",
                                    player, *hit);
                        }
                    }
#ifdef DEBUG
                    fprintf(stderr, "Player %d crashed at: (%d, %d)\n",
                            player, xpx, ypx);
#endif
                    killPlayer(&players[player - 1]);
                    return;
                }
            } else {
                printf("Player %d hit the wall!\n", player);
#ifdef DEBUG
                fprintf(stderr, "Player %d walled at: (%d, %d)\n",
                        player, xpx, ypx);
#endif
                killPlayer(&players[player - 1]);
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
        if (cur->count <= 0) {
            unsigned char *at =
                &hitmap[sizeof(bool) * ((WINDOW_W * cur->y) + cur->x)];
            prev->next = cur->next;
            if (*at > MAX_PLAYERS) (*at) -= MAX_PLAYERS;
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

/*
   void displayGame(void)
   {
// Draw scores
for (i = 0; i < nPlayers; i++) {
unsigned int j;
p = &players[i];
glColor3f(p->c.r, p->c.g, p->c.b);
glRasterPos2f(FONT_CHAR_WIDTH + 0.5,
WINDOW_H - (FONT_CHAR_HEIGHT * SCORE_SPACING * (i + 1))
+ 0.5);
sprintf(score_str, "%d", p->score);
for (j = 0; j < strlen(score_str); j++) {
glutBitmapCharacter(font, score_str[j]);
}
}
*/

void displayGame(void) {

    /*
       unsigned char *target;

       SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format,
       0x00, 0x00, 0x00));

       SDL_LockSurface(screen);

       target = screen->pixels;

       unsigned int xx;
       unsigned int yy;
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
     target[0] = 0xFF * p->c.b;
     target[1] = 0xFF * p->c.g;
     target[2] = 0xFF * p->c.r;
     }
     }
     }

     SDL_UnlockSurface(screen);

     if (SDL_Flip(screen) == -1) {
     exit(1);
     }
     */
}

/*
   void displayGameStart(void)
   {
   char score_str[SCORE_BUF];
   struct player *p;
   int i;
// Draw scores
for (i = 0; i < nPlayers; i++) {
unsigned int j;
p = &players[i];
glColor3f(p->c.r, p->c.g, p->c.b);
glRasterPos2f(FONT_CHAR_WIDTH + 0.5,
WINDOW_H - (FONT_CHAR_HEIGHT * SCORE_SPACING * (i + 1))
+ 0.5);
sprintf(score_str, "%d", p->score);
for (j = 0; j < strlen(score_str); j++) {
glutBitmapCharacter(font, score_str[j]);
}
}
*/

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

                GLuint curx, cury;

                if (keyDown[p->lkey]) {
                    p->dir -= 0.0022 * delta;
                } else if (keyDown[p->rkey]) {
                    p->dir += 0.0022 * delta;
                }

                p->posx += 0.1 * cos(p->dir) * delta;
                p->posy += 0.1 * sin(p->dir) * delta;
                if (nPlayers == 1) players[0].score += delta;

                curx = (GLuint)p->posx;
                cury = (GLuint)p->posy;

                p->holecount += delta;
                if (p->holecount > HOLE_FREQ) {
                    p->holecount = 0;
                }

                if (/*p->holecount < HOLE_SIZE && */(p->prevx != curx
                            || p->prevy != cury)) {
#ifdef DEBUG
                    fprintf(stderr, "Adding to hitmap: %d, %d...\n",
                            p->prevx, p->prevy);
#endif
                    addToHitMap(p->prevx, p->prevy, p->active);
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
        logicFunc = logicGame;
        displayFunc = displayGame;
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
                SDL_Rect offset = {(int)p->posx - 8, (int)p->posy - 8};
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
    SDL_Color col = {0x00, 0x00, 0xFF};

    char *c[MENU_MAIN_CHOICES];
    c[0] = "START GAME";
    c[1] = "SETTINGS";
    c[2] = "EXIT";

    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format,
                0x00, 0x00, 0x00));

    SDL_LockSurface(screen);
    drawCircle(50, 80, col);
    SDL_UnlockSurface(screen);

    /*
       for (i = 0; i < nPlayers; i++) {
       struct player *p = &players[i];
       glBegin(GL_POINTS);
       glVertex2i((WINDOW_W / 2) - (FONT_CHAR_WIDTH * strlen(c[0])
       / 2.4) + ((i - nPlayers) * 30) + 0.5,
       (WINDOW_H / 2) + (MENU_MAIN_CHOICES * 7)
       + (FONT_CHAR_HEIGHT / 2.0) + 0.5);
       glEnd();
       }

       glColor3f(0.5, 0.5, 0.5);
       glBegin(GL_POINTS);
       for (i = nPlayers; i < MAX_PLAYERS; i++) {
       glVertex2i((WINDOW_W / 2) + (FONT_CHAR_WIDTH * strlen(c[0])
       / 1.35) + ((i - nPlayers) * 30) + 0.5,
       (WINDOW_H / 2) + (MENU_MAIN_CHOICES * 7)
       + (FONT_CHAR_HEIGHT / 2.0) + 0.5);
       }
       */

    unsigned int i;
    signed int mid = -(MENU_MAIN_CHOICES / 2);

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
        };

        SDL_BlitSurface(msg, NULL, screen, &offset);
        SDL_FreeSurface(msg);
    }

    if (SDL_Flip(screen) == -1) {
        exit(1);
    }

    /*
       unsigned int j;
       for (i = 0; i < nPlayers; i++) {
       struct player *p = &players[i];
       glColor3f(p->c.r, p->c.g, p->c.b);
       glBegin(GL_POINTS);
       glVertex2i((WINDOW_W / 2) - (FONT_CHAR_WIDTH * strlen(c[0])
       / 2.4) + ((i - nPlayers) * 30) + 0.5,
       (WINDOW_H / 2) + (MENU_MAIN_CHOICES * 7)
       + (FONT_CHAR_HEIGHT / 2.0) + 0.5);
       glEnd();
       }

       glColor3f(0.5, 0.5, 0.5);
       glBegin(GL_POINTS);
       for (i = nPlayers; i < MAX_PLAYERS; i++) {
       glVertex2i((WINDOW_W / 2) + (FONT_CHAR_WIDTH * strlen(c[0])
       / 1.35) + ((i - nPlayers) * 30) + 0.5,
       (WINDOW_H / 2) + (MENU_MAIN_CHOICES * 7)
       + (FONT_CHAR_HEIGHT / 2.0) + 0.5);
       }
       glEnd();
       */
}

void displaySettingsMenu(void)
{
    char *c[MENU_SETTINGS_CHOICES];
    c[0] = "FULLSCREEN";
    c[1] = "BACK";

    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format,
                0x00, 0x00, 0x00));

    /*
    if (menuchoice_s == 0) {
        glColor3f(0.5, 0.5, 0.5);
        glBegin(GL_TRIANGLES);

        // Theese are nasty. Should come up with something cleaner.
        if (dim < maxdim) {
            glVertex2i((WINDOW_W / 2) + (FONT_CHAR_WIDTH * strlen(c[0]))
                    / TRIANGLE_PANNING_X + 0.5, WINDOW_H / 2
                    + (MENU_SETTINGS_CHOICES) * TRIANGLE_PANNING_Y + 0.5); 
            glVertex2i((WINDOW_W / 2) + (FONT_CHAR_WIDTH * strlen(c[0]))
                    / TRIANGLE_PANNING_X + 5.5, WINDOW_H / 2
                    + (MENU_SETTINGS_CHOICES) * TRIANGLE_PANNING_Y
                    + (FONT_CHAR_HEIGHT / 2) * 1.2 + 0.5);
            glVertex2i((WINDOW_W / 2) + (FONT_CHAR_WIDTH * strlen(c[0]))
                    / TRIANGLE_PANNING_X + 0.5, WINDOW_H / 2
                    + (MENU_SETTINGS_CHOICES) * TRIANGLE_PANNING_Y
                    + FONT_CHAR_HEIGHT * 1.2 + 0.5);
        }
        if (dim > 0) {
            glVertex2i((WINDOW_W / 2) - (FONT_CHAR_WIDTH * strlen(c[0]))
                    / TRIANGLE_PANNING_X + 0.5, WINDOW_H / 2
                    + (MENU_SETTINGS_CHOICES) * TRIANGLE_PANNING_Y + 0.5);
            glVertex2i((WINDOW_W / 2) - (FONT_CHAR_WIDTH * strlen(c[0]))
                    / TRIANGLE_PANNING_X + 0.5, WINDOW_H / 2
                    + (MENU_SETTINGS_CHOICES) * TRIANGLE_PANNING_Y
                    + FONT_CHAR_HEIGHT * 1.2 + 0.5);
            glVertex2i((WINDOW_W / 2) - (FONT_CHAR_WIDTH * strlen(c[0]))
                    / TRIANGLE_PANNING_X - 4.5, WINDOW_H / 2
                    + (MENU_SETTINGS_CHOICES) * TRIANGLE_PANNING_Y
                    + (FONT_CHAR_HEIGHT / 2) * 1.2 + 0.5);
        }
        glEnd();
    }
    */

    unsigned int i;
    signed int mid = -(MENU_MAIN_CHOICES / 2);

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
        };

        SDL_BlitSurface(msg, NULL, screen, &offset);
        SDL_FreeSurface(msg);
    }

    /*
    for (i = 0; i < MENU_SETTINGS_CHOICES; i++) {

        if (i == menuchoice_s) {
            glColor3f(1.0, 1.0, 1.0);
        } else {
            glColor3f(0.5, 0.5, 0.5);
        }

        glRasterPos2f((WINDOW_W / 2) - (strlen(c[i]) * FONT_CHAR_WIDTH
                    / 2.0) + 0.5, (WINDOW_H / 2)
                + (MENU_SETTINGS_CHOICES * 7) - (i * MENU_SPACING) + 0.5);

        for (j = 0; j < strlen(c[i]); j++) {
            glutBitmapCharacter(font_menu, c[i][j]);
        }
    }
    */

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
                logicFunc = logicGameStart;
                displayFunc = displayGameStart;
                break;
            case 1:
                keyDown[32] = keyDown[13] = 0;
                displayFunc = displaySettingsMenu;
                logicFunc = logicSettingsMenu;
                break;
            case 2:
                exitGame(0);
                break;
            default:
                break;
        }
    }
    if (keyDown[SPEC_DOWN]) {
        keyDown[SPEC_DOWN] = 0;
        menuchoice++;
    } else if (keyDown[SPEC_UP]) {
        keyDown[SPEC_UP] = 0;
        menuchoice--;
    } else if (keyDown[SPEC_LEFT]) {
        keyDown[SPEC_LEFT] = 0;
        if (menuchoice == 0 && nPlayers < MAX_PLAYERS) nPlayers++;
    } else if (keyDown[SPEC_RIGHT]) {
        keyDown[SPEC_RIGHT] = 0;
        if (menuchoice == 0 && nPlayers > 1) nPlayers--;
    }
    if (menuchoice >= MENU_MAIN_CHOICES) {
        menuchoice = 0;
    } else if (menuchoice < 0) {
        menuchoice = MENU_MAIN_CHOICES - 1;
    }
}

/*
 * mode: 1 = bigger, 0 = smaller
 */
/*
   void reshapeWindow(bool mode)
   {
   if (mode) dim++;
   else dim--;
   WINDOW_W = dimensions[dim][0];
   WINDOW_H = dimensions[dim][1];
   glutReshapeWindow(WINDOW_W, WINDOW_H);
   initDisplay();
   }
   */

void logicSettingsMenu(void)
{
    /*
    if (glutGet(GLUT_WINDOW_HEIGHT) != dimensions[dim][1]
            || glutGet(GLUT_WINDOW_WIDTH) != dimensions[dim][0]) {
        printf("Could not fit window!\n");
        maxdim--;
        reshapeWindow(0);
        }
        */
    if (keyDown[32] || keyDown[13]) {
        switch (menuchoice_s) {
            case MENU_SETTINGS_CHOICES - 1: /* Back */
                keyDown[32] = keyDown[13] = 0;
                displayFunc = displayMainMenu;
                logicFunc = logicMainMenu;
                menuchoice_s = 0;
                break;
            default:
                break;
        }
    }
    if (keyDown[SPEC_DOWN]) {
        keyDown[SPEC_DOWN] = 0;
        menuchoice_s--;
    } else if (keyDown[SPEC_UP]) {
        keyDown[SPEC_UP] = 0;
        menuchoice_s++;
    } else if (keyDown[SPEC_LEFT]) {
        fullscreen = 0;
        initScreen();
        keyDown[SPEC_LEFT] = 0;
        if (menuchoice_s == 0 && dim > 0) {
            /* reshapeWindow(0); */
        }
    } else if (keyDown[SPEC_RIGHT]) {
        fullscreen = 1;
        initScreen();
        keyDown[SPEC_RIGHT] = 0;
        if (menuchoice_s == 0 && dim < maxdim) {
            /* reshapeWindow(1); */
        }
    }
    if (menuchoice_s >= MENU_SETTINGS_CHOICES) {
        menuchoice_s = 0;
    } else if (menuchoice_s < 0) {
        menuchoice_s = MENU_SETTINGS_CHOICES - 1;
    }
}

int main(int argc, char **argv)
{
    WINDOW_W = dimensions[dim][0];
    WINDOW_H = dimensions[dim][1];

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

    logicFunc = logicMainMenu;
    displayFunc = displayMainMenu;

    for (;;) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                int k = event.key.keysym.sym;
                if (k == SDLK_ESCAPE) {
                    if (logicFunc == logicGame
                            || logicFunc == logicGameStart) {
                        cleanHitMap();
                        initPlayers1();
                        logicFunc = logicMainMenu;
                        displayFunc = displayMainMenu;
                    } else if (logicFunc == logicSettingsMenu) {
                        logicFunc = logicMainMenu;
                        displayFunc = displayMainMenu;
                    } else {
                        exitGame(0);
                    }
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
        logicFunc();
        displayFunc();
    }

    return 0;
}
