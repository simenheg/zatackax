#ifndef PARTICLE_H
#define PARTICLE_H

#include <math.h>
#include <SDL2/SDL.h>

#include "common.h"
#include "gfx.h"
#include "player.h"

#define MAX_PARTICLES 300000

typedef struct particle {
    double x;
    double y;
    double vel;
    double angle;
    double life;
    double decay;
    SDL_Color color;
} Particle;

extern SDL_Surface *particleScreen;
extern Particle particles[MAX_PARTICLES];

void renderParticles(SDL_Surface *target);
void updateParticles(Uint32 delta);
void resetParticles(void);
int initParticleScreen(int width, int height);
void addParticles(int num, double x, double y, double velMin, double velMax,
                  double angleMin, double angleMax, double lifeMin,
                  double lifeMax, double decay, double rMin, double rMax,
                  double gMin, double gMax, double bMin, double bMax);
void addParticleSpinner(int num, struct player *p, double speed, double offset,
                        double velMin, double velMax, double rMin, double rMax,
                        double gMin, double gMax, double bMin, double bMax);

#endif
