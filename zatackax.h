#ifndef ZATACKAX_H
#define ZATACKAX_H

#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"

#define PI                  3.14159265358

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
#define SPEC_UP             273
#define SPEC_DOWN           274
#define SPEC_RIGHT          275
#define SPEC_LEFT           276 

/* PLAYERS */
#define MAX_PLAYERS         8   /* Maximal number of players */
#define DEFAULT_N_PLAYERS   2   /* Default number of players */

/* SPAWNING */
#define START_ROUND_WAIT    1500    /* Miliseconds until the game starts */
#define SPAWN_SPACE_MIN     100     /* Minimal spawning distance from
                                       screen edges */

/* HOLES */
#define HOLE_FIRST_DELAY    300     /* Minimum delay of first hole */
#define HOLE_FREQ           3000    /* How often holes appears */
#define HOLE_SIZE           2850    /* Higher number -> smaller hole */
#define HOLE_DELAY          76      /* Time before hole is created */

/* ZATAS */
#define ZATA_SIZE           4                   /* Thickness of the zata */
#define TOLERANCE           ZATA_SIZE / 2       /* Collision detection */
#define COUNTDOWN_TOLERANCE 25 * (ZATA_SIZE)    /* Suicide detection */

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

typedef unsigned char bool;

static bool fullscreen = DEFAULT_FULLSCREEN;
static bool holes = DEFAULT_HOLES;
static bool broadcasts = DEFAULT_BROADCASTS;

struct player {
    unsigned char active;   /* 0 if activated, ID else */
    bool alive;
    SDL_Surface *arrow;
    double posx;
    double posy;
    double dir;
    unsigned int color;
    unsigned int prevx;
    unsigned int prevy;
    unsigned int score;
    unsigned int lkey;
    unsigned int rkey;
    int holecount;
};

struct recentMapPiece {
    int x, y, count;
    struct recentMapPiece *next;
};

struct vel {
    double x, y, dir;
    unsigned int holecount;
};

static unsigned int randomizer = 1; /* Help distribute better random
                                       values */

static unsigned int WINDOW_W;       /* Window width */
static unsigned int WINDOW_H;       /* Window height */

static unsigned int keyDown[322];   /* 322 is the number of
                                       possible SDL keys
                                       (see SDL_keysym.h) */

static SDL_Color colors[8];
static struct player players[MAX_PLAYERS];
static struct SDL_Surface **parrows;
static struct SDL_Surface **pballs;

static Uint32 prevtime = 0;
static Uint32 delta = 1;

static unsigned int alivecount = 0;
static int countdown;

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

static unsigned char nPlayers = DEFAULT_N_PLAYERS;
static TTF_Font *font_menu = NULL;
static TTF_Font *font_score = NULL;
static TTF_Font *font_broadc = NULL;
static TTF_Font *font_broadcb = NULL;
SDL_Color cMenuText = {0x80, 0x80, 0x80, 0};
SDL_Color cMenuTextH = {0xFF, 0xFF, 0xFF, 0};
SDL_Color cMenuBG = {0x00, 0x00, 0x00, 0};
SDL_Color cBroadcast = {0xFF, 0xFF, 0xFF, 0};

static unsigned char editPlayer = 0;

void logicMainMenu(void);
void logicSettingsMenu(void);
void logicPlayerMenu(void);
void logicPConfMenu(void);
void logicGameStart(void);
void logicGame(void);
void displayMainMenu(void);
void displaySettingsMenu(void);
void displayPlayerMenu(void);
void displayPConfMenu(void);
void displayGameStart(void);

#endif
