#pragma once

#include "jsanchez_shared.h"
#include "jsanchez_math.h"

typedef struct clock
{
    u64 PerfCounterNow;
    u64 PerfCounterLast;
    f64 DeltaTime;
    f64 SecondsElapsed;
} clock;
