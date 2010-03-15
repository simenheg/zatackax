#ifndef IFISMS_H
#define IFISMS_H

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"

/* #define DEBUG */

#define SCREEN_BPP          32

#define N_DIMENSION         3

#define SCORE_BUF           8 

#define SPEC_UP             273
#define SPEC_DOWN           274
#define SPEC_RIGHT          275
#define SPEC_LEFT           276 

#define SPAWN_SPACE_MIN     100
#define SCORE_SPACING       25
#define COUNTDOWN_TOLERANCE 400
#define MAX_PLAYERS         8
#define DEFAULT_N_PLAYERS   2
#define START_ROUND_WAIT    1500

#define HOLE_FREQ           3000
#define HOLE_FIRST_DELAY    300
#define HOLE_SIZE           1875

#define ZATA_SIZE           4
#define TOLERANCE           ZATA_SIZE / 2
#define PI                  3.14159265358

/* MENUS */
#define MENU_FONT_SIZE          20
#define SPACEMOD                15

/* MAIN MENU */
#define MENU_MAIN_CHOICES       3

/* SETTINGS MENU */
#define MENU_SETTINGS_CHOICES   2
#define TRIANGLE_PANNING_X      1.8
#define TRIANGLE_PANNING_Y      6.5

typedef unsigned char bool;

struct player {
    unsigned char active;   /* 0 if activated, ID else */
    bool alive;
    SDL_Color c;
    SDL_Surface *arrow;
    GLdouble posx;
    GLdouble posy;
    GLuint prevx;
    GLuint prevy;
    GLdouble dir;
    unsigned int score;
    unsigned int lkey;
    unsigned int rkey;
    unsigned int holecount;
};

static unsigned int randomizer = 1;  /* Help distribute better random
                                        values */

static const GLint dimensions[N_DIMENSION][2] = {
    {640, 480},
    {800, 600},
    {1024, 768}
};

static int dim = 1;                     /* Chosen field dimensions */
static int maxdim = N_DIMENSION - 1;    /* Maximal dimension fit */
static GLuint WINDOW_W;                 /* Window width */
static GLuint WINDOW_H;                 /* Window height */

static unsigned int keyDown[322];       /* 322 is the number of
                                           possible SDL keys
                                           (see SDL_keysym.h) */
static struct player players[MAX_PLAYERS];
static struct SDL_Surface **parrows;

static Uint32 prevtime = 0;
static Uint32 delta = 1;

static unsigned int alivecount = 0;
static int countdown;

static unsigned char *hitmap;
static struct recentMapPiece *recents;

static unsigned char nPlayers = DEFAULT_N_PLAYERS;
static char menuchoice = 0;
static char menuchoice_s = 0;
static unsigned char fullscreen = 0;
static TTF_Font *font_menu = NULL;
static TTF_Font *font_score = NULL;
SDL_Color cMenuText = {0x80, 0x80, 0x80};
SDL_Color cMenuTextH = {0xFF, 0xFF, 0xFF};
SDL_Color cMenuBG = {0x00, 0x00, 0x00};

void logicMainMenu(void);
void logicSettingsMenu(void);
void logicGameStart(void);
void logicGame(void);
void displayMainMenu(void);
void displaySettingsMenu(void);
void displayGameStart(void);
void displayGame(void);

#endif
