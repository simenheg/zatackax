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
#include <SDL/SDL_ttf.h>
#include "sound.h"
#include "error.h"
#include "ai.h"
#include "broadcast.h"
#include "weapon.h"
#include "player.h"
#include "common.h"

/* SCORES */
#define SCORE_BUF           8
#define SCORE_SPACING       25
#define SCORE_CAP_MAX       65536

/* SPECIAL KEYS */
#define MAX_KEYNAME         8       /* Maximum length of key name */

/* SPAWNING */
#define START_ROUND_WAIT    1500    /* Miliseconds until the game starts */
#define SPAWN_SPACE_MIN     100     /* Minimal spawning distance from
                                       screen edges */
#define ZATA_SPAWN_SPACING  20      /* Minimal spawning distance between
                                       zatas */
#define TRY_SPAWN_THIS_HARD 40      /* Try this many times to make a fit */

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
#define MENU_BUF                32  /* Maxiumum menu string length */
#define MENU_FONT_SIZE          22
#define SPACEMOD                18
#define BALL_SPACING            30
#define BALL_Y_MOD              39

/* SETTINGS MENU */
#define TRIANGLE_PANNING_X      1.8
#define TRIANGLE_PANNING_Y      6.5
#define ON_OFF                  ? "[on]" : "[off]"
#define DEFAULT_FULLSCREEN      false
#define DEFAULT_SOUND           true
#define DEFAULT_MUSIC           false
#define DEFAULT_WEAPONS         true
#define DEFAULT_HOLES           true
#define DEFAULT_BROADCASTS      true
#define DEFAULT_BORDER          false
#define DEFAULT_DUELMODE        false
#define DEFAULT_SCORECAP        false

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
const SDL_Color cMenuText = {0x80, 0x80, 0x80, 0};
const SDL_Color cMenuTextH = {0xFF, 0xFF, 0xFF, 0};
static SDL_Color colors[N_COLORS];
static SDL_Rect arrowClip[32];
static SDL_Event event;
static Uint32 prevtime = 0;
static unsigned int alivecount = 0;
static int countdown;
static bool winnerDeclared = false;
static bool screenFreeze = false;
static bool weaponsDisabled = false;

static unsigned int scorecap = DEFAULT_SCORECAP;
static bool fullscreen = DEFAULT_FULLSCREEN;
static bool sound = DEFAULT_SOUND;
static bool music = DEFAULT_MUSIC;
static bool weapons = DEFAULT_WEAPONS;
static bool holes = DEFAULT_HOLES;
static bool broadcasts = DEFAULT_BROADCASTS;
static bool border = DEFAULT_BORDER;
static bool duelmode = DEFAULT_DUELMODE;
static const char *settingNames[] = {"fullscreen", "sound", "music", "weapons",
                                     "holes", "broadcasts", "border",
                                     "duelmode"};
static bool *settings[8] = {&fullscreen, &sound, &music, &weapons, &holes,
                            &broadcasts, &border, &duelmode};

static unsigned int WINDOW_W;       /* Window width */
static unsigned int WINDOW_H;       /* Window height */
static unsigned int keyDown[322];   /* 322 is the number of
                                       possible SDL keys
                                       (see SDL_keysym.h) */

/* Used for multiple menu navigation input sets */
enum keySymbols {KEY_LEFT,
                 KEY_RIGHT,
                 KEY_UP,
                 KEY_DOWN};

/* SURFACES */
static SDL_Surface *screen = NULL;
static SDL_Surface *msg = NULL;
static SDL_Surface *logo = NULL;
static SDL_Surface *arrows = NULL;
static SDL_Surface *ball = NULL;
static SDL_Surface *wiBg = NULL;
static SDL_Surface *wiSpeed = NULL;
static SDL_Surface *wisSpeed = NULL;
static SDL_Surface *wiFrost = NULL;
static SDL_Surface *wisFrost = NULL;
static SDL_Surface *wiConf = NULL;
static SDL_Surface *wisConf = NULL;
static SDL_Surface *wiTurn = NULL;
static SDL_Surface *wisTurn = NULL;
static SDL_Surface *wiStep = NULL;
static SDL_Surface *wisStep = NULL;
static SDL_Surface *wiMole = NULL;
static SDL_Surface *wisMole = NULL;
static SDL_Surface *wiWarp = NULL;
static SDL_Surface *wisWarp = NULL;
static SDL_Surface *wiGhost = NULL;
static SDL_Surface *wisGhost = NULL;
static SDL_Surface *wiDisable = NULL;
static SDL_Surface *wisDisable = NULL;
static SDL_Surface *wiSwitch = NULL;
static SDL_Surface *wisSwitch = NULL;
static struct SDL_Surface **parrows;
static struct SDL_Surface **pballs;
static struct SDL_Surface *wepIcons[N_WEAPONS + 1];
static struct SDL_Surface *smallWepIcons[N_WEAPONS];

/* FONTS */
static TTF_Font *font_menu = NULL;
static TTF_Font *font_menub = NULL;
static TTF_Font *font_score = NULL;
TTF_Font *font_broadc = NULL;
TTF_Font *font_broadcb = NULL;

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
int logicGameStart(void);
void displayGameStart(void);
void refreshGameScreen(void);
void drawExtras(void);
void newRound(void);
void endRound(void);

/* WEAPONS */
int legalWeps(void);
void assignAiWeapons(void);
void resetWeapons(void);

/* MENUS */
bool qkey(int keysym);
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
