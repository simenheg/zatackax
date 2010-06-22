#ifndef COMMON_H
#define COMMON_H

#define PI      3.14159265358
#define STRBUF  32

/* output level */
#define O_SILENT    0   /* be absolutely quiet */
#define O_NORMAL    1   /* standard */
#define O_VERBOSE   2   /* important events (good for game logs, maybe?) */
#define O_DEBUG     3   /* debug mode */

#ifdef VERBOSE
static int olvl = O_VERBOSE;
#elif DEBUG
static int olvl = O_DEBUG;
#else
static int olvl = O_NORMAL;
#endif

typedef unsigned char bool;

#endif
