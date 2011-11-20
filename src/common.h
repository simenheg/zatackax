#ifndef COMMON_H
#define COMMON_H

#define PI      3.14159265358
#define STRBUF  32

/* DEFAULT WINDOW DIMENSIONS */
#define DEFAULT_WINDOW_W    800
#define DEFAULT_WINDOW_H    600
#define SCREEN_BPP          32

/* PLAYERS */
#define MIN_PLAYERS         2   /* Minimum number of players */
#define MAX_PLAYERS         8   /* Maximal number of players */
#define DEFAULT_N_PLAYERS   2   /* Default number of players */

/* OUTPUT LEVEL */
#define O_SILENT    0   /* Be absolutely quiet */
#define O_NORMAL    1   /* Standard */
#define O_VERBOSE   2   /* Important events (good for game logs, maybe?) */
#define O_DEBUG     3   /* Debug mode */

#ifdef VERBOSE
static int olvl = O_VERBOSE;
#elif DEBUG
static int olvl = O_DEBUG;
#else
static int olvl = O_NORMAL;
#endif

#endif
