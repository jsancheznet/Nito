#pragma once

#include "jsanchez_shared.h"
#include "jsanchez_dynamic_array.h"
#include "jsanchez_math.h"

#include "mesh.h"

typedef struct model
{
    mesh *Meshes; // Dynamic array

    // char *Path: Stores the current model directory while loading
    // the model, it is to be used later when loading textures
    char *Path;
} model;
