#pragma once

#include "jsanchez_shared.h"
#include "jsanchez_math.h"
#include "jsanchez_dynamic_array.h"

#include "keyboard.c"
#include "mouse.c"

typedef struct camera
{
    vec3 Position;
    vec3 Acceleration; // TODO: Rename to forces applied
    vec3 Velocity;

    vec3 Front;
    vec3 Up;

    f32 Speed;
    f32 Yaw;
    f32 Pitch;

    f32 FoV;
    f32 Near;
    f32 Far;

    mat4 View;
    mat4 Projection;
    mat4 Orthographic;

    // TODO: Add MV matrix for normals!

} camera;
