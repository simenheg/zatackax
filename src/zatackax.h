#ifndef ZATACKAX_H
#define ZATACKAX_H

#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "sound.h"
#include "error.h"
#include "ai.h"
#include "broadcast.h"
#include "weapon.h"
#include "player.h"
#include "input.h"
#include "config.h"
#include "data.h"
#include "font.h"
#include "gfx.h"
#include "video.h"
#include "common.h"

/* SCORES */
#define SCORE_BUF           8
#define SCORE_SPACING       25
#define SCORE_CAP_MAX       65536

/* HOLES */
/* Hole size.
 * Higher number -> smaller hole */
#define HOLE_SIZE           2820 * (0.9 + ZATA_SPEED)
#define HOLE_FIRST_DELAY    300     /* Minimum delay of first hole */
#define HOLE_FREQ           3000    /* How often holes appears */
#define HOLE_DELAY          108     /* Time before hole is created */

/* ZATAS */
#define ZATA_SIZE           4                   /* Thickness of the zata */
#define ZATA_SPEED          0.09                /* Speed of the zata */
#define TOLERANCE           ZATA_SIZE / 2       /* Collision detection */
#define COUNTDOWN_TOLERANCE 50 * (ZATA_SIZE)    /* Suicide detection */
#define N_COLORS            8                   /* Available colors */

/* MENUS */
#define MENU_BUF            32      /* Maximum menu string length */
#define SPACEMOD            18
#define BALL_SPACING        30
#define BALL_Y_MOD          39

/* SETTINGS MENU */
#define TRIANGLE_PANNING_X  1.8
#define TRIANGLE_PANNING_Y  6.5
#define ON_OFF              ? "[on]" : "[off]"

struct recentMapPiece {
    int x, y, count;
    struct recentMapPiece *next;
};

struct vel {
    double x, y, dir;
    unsigned int holecount;
};

struct scene {
    int (*logicFunc)(void);
    void (*displayFunc)(void);
    struct scene *parentScene;
};

struct menu {
    const char choices;
    signed char choice;
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
const SDL_Color cMenuText = {0x80, 0x80, 0x80, 0};
const SDL_Color cMenuTextH = {0xFF, 0xFF, 0xFF, 0};
static SDL_Color colors[N_COLORS];
static SDL_Rect arrowClip[32];
static SDL_Event event;
static Uint32 prevtime = 0;
static unsigned int alivecount = 0;
static int countdown;
static bool winnerDeclared = false;

/* SURFACES */
static SDL_Surface *msg = NULL;
static struct SDL_Surface **parrows;
static struct SDL_Surface **pballs;
static struct SDL_Surface *wepIcons[N_WEAPONS + 1];
static struct SDL_Surface *smallWepIcons[N_WEAPONS];

/* PLAYER */
void initPlayers1(void);
void initPlayers2(void);
void resetPlayer(int player);
void deselectWeapons(void);
SDL_Color *extractPColors(void);
void killPlayer(unsigned char killed, unsigned char killer);
void newBroadcast(char *msg);
void playerWon(unsigned char id);
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
int logicGame(void);
void displayGame(void);
int logicGameStart(void);
void displayGameStart(void);
void drawExtras(void);
void newRound(void);
void endRound(void);

/* WEAPONS */
void assignAiWeapons(void);
void resetWeapons(void);

/* MENUS */
void initMainMenu(void);
int logicMainMenu(void);
void displayMainMenu(void);
int logicWepMenu(void);
void displayWepMenu(void);
int logicSettingsMenu(void);
void displaySettingsMenu(void);
int logicPlayerMenu(void);
void displayPlayerMenu(void);
int logicPConfMenu(void);
void displayPConfMenu(void);
int handleMenu(struct menu *m);
void displayMenu(char *c[], struct menu *m, int ymod);

/* GRAPHICS */
void putPixel(int x, int y, SDL_Color c, unsigned char *target);
void colorFill(SDL_Color c, SDL_Surface *sprite);
void initColors(void);
void colorBalls(void);

/* SYSTEM */
int init(void);
void initGraphics(void);
void exitGame(int status);

#endif
