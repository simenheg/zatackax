#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include "common.h"

extern const int BUTTON_NAME_MAX_LEN;
extern bool keyDown[322];
extern bool joyButtonDown[MAX_PLAYERS][128];

/*
 * The lowest 9 bits are reserved for SDL keyboard keys and mouse
 * buttons. If bits above this are set, they are interpreted to mean
 * the joystick device index (+ 1). Then the four lowest bits
 * represent the joystick button number, while 1x10...0x40 represent
 * the four axis directions in the order up, right, down, left.
 */
typedef unsigned int button;

enum joyDir
{
    JOY_DIR_UP = 1,
    JOY_DIR_RIGHT = 2,
    JOY_DIR_DOWN = 3,
    JOY_DIR_LEFT = 4
};

// Used for multiple menu navigation input sets.
enum keySymbol
{
    KEY_UP = 0,
    KEY_RIGHT = 1,
    KEY_DOWN = 2,
    KEY_LEFT = 3
};

void openJoysticks(void);
void closeJoysticks(void);
bool isJoyButton(button b);
bool isJoyAxis(button b);
int joyIndex(button b);
int joyButtonNumber(button b);
enum joyDir joyAxisDir(button b);
int axisNumber(SDL_JoyAxisEvent e);
bool enterButtonDown(void);
void clearEnterButtons(void);
bool buttonDown(button b);
void clearButton(button b);
bool menuButtonQuery(enum keySymbol ks);
char *buttonName(button b);

#endif
