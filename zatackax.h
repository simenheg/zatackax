#ifndef ZATACKAX_H
#define ZATACKAX_H

#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#define PI                  3.14159265358

/* MISC */
#define STRBUF              32

/* DEFAULT WINDOW DIMENSIONS */
#define DEFAULT_WINDOW_W    800
#define DEFAULT_WINDOW_H    600
#define SCREEN_BPP          32

/* BROADCAST MESSAGES */
#define BROADC_BUF          32  /* Maximum broadcast name length */
#define BROADC_FONT_SIZE    13
#define BROADC_LIMIT        6   /* Maximum number of broadcast messages */

/* SCORES */
#define SCORE_BUF           8
#define SCORE_SPACING       25

/* SPECIAL KEYS */
#define MAX_KEYNAME         8   /* Maximum length of key name */

/* PLAYERS */
#define MIN_PLAYERS         2   /* Minimum number of players */
#define MAX_PLAYERS         8   /* Maximal number of players */
#define DEFAULT_N_PLAYERS   2   /* Default number of players */

/* SPAWNING */
#define START_ROUND_WAIT    1500    /* Miliseconds until the game starts */
#define SPAWN_SPACE_MIN     100     /* Minimal spawning distance from
                                       screen edges */

/* HOLES */
/* Hole size.
 * Higher number -> smaller hole */
#define HOLE_SIZE           2830 * (0.9 + ZATA_SPEED)
#define HOLE_FIRST_DELAY    300     /* Minimum delay of first hole */
#define HOLE_FREQ           3000    /* How often holes appears */
#define HOLE_DELAY          108      /* Time before hole is created */

/* ZATAS */
#define ZATA_SIZE           4                   /* Thickness of the zata */
#define ZATA_SPEED          0.09                /* Speed of the zata */
#define TOLERANCE           ZATA_SIZE / 2       /* Collision detection */
#define COUNTDOWN_TOLERANCE 50 * (ZATA_SIZE)    /* Suicide detection */
#define N_COLORS            8                   /* Available colors */

/* MENUS */
#define MENU_BUF                32  /* Maxiumum menu string length */
#define MENU_FONT_SIZE          20
#define SPACEMOD                15

/* SETTINGS MENU */
#define TRIANGLE_PANNING_X      1.8
#define TRIANGLE_PANNING_Y      6.5
#define ON_OFF                  ? "[on]" : "[off]"
#define DEFAULT_FULLSCREEN      0
#define DEFAULT_HOLES           1
#define DEFAULT_BROADCASTS      1
#define DEFAULT_DUELMODE        0

typedef unsigned char bool;

struct player {
    unsigned char active;   /* 0 if activated, ID else */
    bool alive;
    SDL_Surface *arrow;
    double posx;
    double posy;
    double dir;
    double speed;
    unsigned int color;
    unsigned int prevx;
    unsigned int prevy;
    unsigned int score;
    unsigned int lkey;
    unsigned int rkey;
    unsigned int wkey;
    int holecount;
    int wepcount;           /* Remaining weapon time, -999 if unused */
    int (*wepFunc)(struct player*, bool);
};

struct recentMapPiece {
    int x, y, count;
    struct recentMapPiece *next;
};

struct vel {
    double x, y, dir;
    unsigned int holecount;
};

struct scene {
    void (*logicFunc)(void);
    void (*displayFunc)(void);
    struct scene *parentScene;
};

struct menu {
    const char choices;
    char choice;
};

/* The hitmap contains one character for each pixel on the screen. Each
 * pixel is represented with a number, according to the following code:
 *
 * 0                                - empty pixel
 * 1-MAX_PLAYERS                    - player x resides here
 * (MAX_PLAYERS+1)-MAX_PLAYERS*2    - player x resides here, but cannot
 *                                    commit suicide here
 * (MAX_PLAYERS*2+1)-MAX_PLAYERS*3  - later to become a hole
 */
static unsigned char *hitmap;
static struct recentMapPiece *recents;
static unsigned char editPlayer = 0;
static unsigned char nPlayers = DEFAULT_N_PLAYERS;
static TTF_Font *font_menu = NULL;
static TTF_Font *font_menub = NULL;
static TTF_Font *font_score = NULL;
static TTF_Font *font_broadc = NULL;
static TTF_Font *font_broadcb = NULL;
const SDL_Color cMenuText = {0x80, 0x80, 0x80, 0};
const SDL_Color cMenuTextH = {0xFF, 0xFF, 0xFF, 0};
const SDL_Color cMenuBG = {0x00, 0x00, 0x00, 0};
const SDL_Color cBroadcast = {0xFF, 0xFF, 0xFF, 0};
static SDL_Color colors[N_COLORS];
static SDL_Rect arrowClip[32];
static SDL_Surface *screen = NULL;
static SDL_Surface *msg = NULL;
static SDL_Surface *arrows = NULL;
static SDL_Surface *ball = NULL;
static SDL_Surface *broadcast[BROADC_LIMIT];
static SDL_Event event;
static struct player players[MAX_PLAYERS];
static struct SDL_Surface **parrows;
static struct SDL_Surface **pballs;
static Uint32 prevtime = 0;
static unsigned int alivecount = 0;
static int countdown;

static const char *settingNames[] = {"fullscreen", "holes", "broadcasts", 
"duelmode"};
static bool fullscreen = DEFAULT_FULLSCREEN;
static bool holes = DEFAULT_HOLES;
static bool broadcasts = DEFAULT_BROADCASTS;
static bool duelmode = DEFAULT_DUELMODE;
static bool *settings[4] = {&fullscreen, &holes, &broadcasts, &duelmode};

static unsigned int randomizer = 1; /* Help distribute better random
                                       values */

static unsigned int WINDOW_W;       /* Window width */
static unsigned int WINDOW_H;       /* Window height */
static unsigned int keyDown[322];   /* 322 is the number of
                                       possible SDL keys
                                       (see SDL_keysym.h) */

/* PLAYER */
void initPlayers1(void);
void initPlayers2(void);
void killPlayer(unsigned char killed, unsigned char killer);
struct vel spawn(void);
void respawn(struct player *p);
void drespawn(struct player *p);
void setColor(unsigned char pedit, bool up);
void setNextKey(unsigned char pedit, unsigned char key);

/* HITMAP */
void initHitMap(unsigned int w, unsigned int h);
void addToHitMap(unsigned int x, unsigned int y, unsigned char player,
        unsigned char modifier);
void updateHitMap(Uint32 delta);
void cleanHitMap(void);

/* GAME */
void logicGame(void);
void logicGameStart(void);
void displayGameStart(void);
void refreshGameScreen(void);
void makeBroadcast(struct player *p, unsigned char killer);
void cleanBroadcast(void);
void drawScores(void);
void newRound(void);

/* WEAPONS */
int wepSpeedup(struct player *p, bool on);
int wepColdwave(struct player *p, bool on);

/* MENUS */
void initMainMenu(void);
void logicMainMenu(void);
void displayMainMenu(void);
void logicSettingsMenu(void);
void displaySettingsMenu(void);
void logicPlayerMenu(void);
void displayPlayerMenu(void);
void logicPConfMenu(void);
void displayPConfMenu(void);
void handleMenu(struct menu *m);
void displayMenu(char *c[], struct menu *m);

/* GRAPHICS */
int initScreen(void);
void putPixel(int x, int y, SDL_Color c, unsigned char *target);
void colorFill(SDL_Color c, SDL_Surface *sprite);
SDL_Surface *loadImage(const char filename[]);
void initColors(void);
void colorBalls(void);

/* SYSTEM */
int init(void);
int loadFiles(void);
void confirmLoading(char *name, SDL_Surface *sprite);
void saveSettings(char *filename);
void restoreSettings(char *filename);
void keyPress(unsigned int key);
void keyRelease(unsigned int key);
char *keyName(unsigned int key);
void displayVoid(void);
void exitGame(int status);

#endif
