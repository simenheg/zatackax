#ifndef COMMON_H
#define COMMON_H

#define STRBUF  32

/* DEFAULT WINDOW DIMENSIONS */
#define DEFAULT_WINDOW_W    800
#define DEFAULT_WINDOW_H    600
#define SCREEN_BPP          32

/* PLAYERS */
#define MIN_PLAYERS         2   /* Minimum number of players */
#define MAX_PLAYERS         8   /* Maximum number of players */
#define DEFAULT_N_PLAYERS   2   /* Default number of players */

/* SPAWNING */
#define START_ROUND_WAIT    1500 /* Milliseconds until the game starts */
#define SPAWN_SPACE_MIN     100  /* Minimal spawn distance from screen edges */
#define ZATA_SPAWN_SPACING  20   /* Minimal spawn distance between zatas */
#define TRY_SPAWN_THIS_HARD 40   /* Try this many times to make a fit */

/* OUTPUT LEVEL */
#define O_SILENT    0   /* Be absolutely quiet */
#define O_NORMAL    1   /* Standard */
#define O_VERBOSE   2   /* Important events (good for game logs, maybe?) */
#define O_DEBUG     3   /* Debug mode */

#ifdef SILENT
static const int olvl = O_SILENT;
#elif VERBOSE
static const int olvl = O_VERBOSE;
#elif DEBUG
static const int olvl = O_DEBUG;
#else
static const int olvl = O_NORMAL;
#endif

void refreshGameScreen(void);
void clearRecents(int fromX, int toX, int fromY, int toY);

#endif
