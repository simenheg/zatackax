#ifndef CONFIG_H
#define CONFIG_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "common.h"
#include "player.h"

#define DEFAULT_FULLSCREEN       false
#define DEFAULT_SOUND            true
#define DEFAULT_MUSIC            true
#define DEFAULT_WEAPONS          true
#define DEFAULT_HOLES            true
#define DEFAULT_BROADCASTS       true
#define DEFAULT_PARTICLE_EFFECTS true
#define DEFAULT_BORDER           false
#define DEFAULT_DUELMODE         false
#define DEFAULT_SCORECAP         0

extern unsigned int scorecap;
extern bool fullscreen;
extern bool sound;
extern bool music;
extern bool weapons;
extern bool holes;
extern bool broadcasts;
extern bool particleEffects;
extern bool border;
extern bool duelmode;

int ensureConfigPath(void);
char *getConfigFile(void);
void saveSettings(void);
void restoreSettings(void);

#endif
