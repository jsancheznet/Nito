#pragma once

#include "jsanchez_shared.h"
#include <SDL.h>

typedef struct keyboard
{
    const u8 *State;
    u8 *CurrentState;
    u8 *PrevState;
    i32 Numkeys;
    SDL_Keymod ModState;
} keyboard;
