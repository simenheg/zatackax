/* particle -- particle system.
 * Copyright (C) 2010-2021 Simen Heggestøyl <simenheg@runbox.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "particle.h"

SDL_Surface *particleScreen = NULL;

Particle particles[MAX_PARTICLES];

static unsigned int numParticles = 0;

static double spinCounters[MAX_PLAYERS];

void renderParticle(SDL_Surface *target, Particle *p)
{
    unsigned char *px = target->pixels;
    unsigned int offset = (target->w * (int)p->y + (int)p->x) * SCREEN_BPP/8;

    px[offset] = p->color.b * p->life;
    px[offset + 1] = p->color.g * p->life;
    px[offset + 2] = p->color.r * p->life;
}

void addParticle(double x, double y, double vel, double angle, double life,
                 double decay, SDL_Color color)
{
    if (numParticles >= MAX_PARTICLES) {
        return;
    }

    particles[numParticles].x = x;
    particles[numParticles].y = y;
    particles[numParticles].vel = vel;
    particles[numParticles].angle = angle;
    particles[numParticles].life = life;
    particles[numParticles].decay = decay;
    particles[numParticles].color = color;

    numParticles++;
}

void addParticles(int num, double x, double y, double velMin, double velMax,
                  double angleMin, double angleMax, double lifeMin,
                  double lifeMax, double decay, double rMin, double rMax,
                  double gMin, double gMax, double bMin, double bMax)
{
    for (int i = 0; i < num; ++i) {
        double vel = velMin + drand48()*(velMax - velMin);
        double angle = angleMin + drand48()*(angleMax - angleMin);
        double life = lifeMin + drand48()*(lifeMax - lifeMin);
        double r = rMin + drand48()*(rMax - rMin);
        double g = gMin + drand48()*(gMax - gMin);
        double b = bMin + drand48()*(bMax - bMin);
        SDL_Color color = {r, g, b, 0};

        addParticle(x, y, vel, angle, life, decay, color);
    }
}

void addParticleSpinner(int num, struct player *p, double speed, double offset,
                        double velMin, double velMax, double rMin, double rMax,
                        double gMin, double gMax, double bMin, double bMax)
{
    unsigned char playerIndex = p->active - 1;

    for (int i = 0; i < num; ++i) {
        spinCounters[playerIndex] += speed;

        if (spinCounters[playerIndex] > 2*M_PI) {
            spinCounters[playerIndex] -= 2*M_PI;
        }

        double vel = velMin + drand48()*(velMax - velMin);
        double r = rMin + drand48()*(rMax - rMin);
        double g = gMin + drand48()*(gMax - gMin);
        double b = bMin + drand48()*(bMax - bMin);
        SDL_Color color = {r, g, b, 0};

        double x = cos(spinCounters[playerIndex]) * offset + p->posx;
        double y = sin(spinCounters[playerIndex]) * offset + p->posy;

        addParticle(x, y, vel, spinCounters[playerIndex], 1, 0.0005, color);
    }
}

void removeParticle(unsigned int i)
{
    particles[i] = particles[numParticles - 1];
    --numParticles;
}

void updateParticles(Uint32 delta)
{
    for (unsigned int i = 0; i < numParticles; ++i) {
        Particle p = particles[i];
        double newX = p.x + cos(p.angle) * p.vel * delta;
        double newY = p.y + sin(p.angle) * p.vel * delta;

        particles[i].life -= p.decay * delta + p.decay * p.vel;

        if (particles[i].life <= 0 || newX <= 0 || newY <= 0 ||
                newX >= particleScreen->w || newY >= particleScreen->h) {
            removeParticle(i);
            --i;
            continue;
        }

        particles[i].x = newX;
        particles[i].y = newY;
    }
}

void renderParticles(SDL_Surface *target)
{
    SDL_LockSurface(particleScreen);

    clearSurface(target);

    for (unsigned int i = 0; i < numParticles; ++i) {
        renderParticle(target, &particles[i]);
    }

    SDL_UnlockSurface(particleScreen);
}

void resetParticles(void)
{
    numParticles = 0;
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        spinCounters[i] = 0;
    }
}

int initParticleScreen(int width, int height)
{
    particleScreen = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height,
                                          SCREEN_BPP, 0, 0, 0, 0);
    SDL_SetColorKey(particleScreen, SDL_SRCCOLORKEY, 0);

    if (particleScreen == NULL) {
        return 0;
    }

    return 1;
}
