#ifndef AI_H
#define AI_H

#include <math.h>
#include "common.h"

#define AI_WEP_PROB 0.00005

char pollAi(double posx, double posy, double dir, unsigned char id,
            unsigned char *hitmap, unsigned int w, unsigned int h);

#endif
