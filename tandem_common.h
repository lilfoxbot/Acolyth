#pragma once

//#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
//#include <stdlib.h>

void SetString(char *dest, size_t dest_size, const char *source){
    snprintf(dest, dest_size, "%s", source);
}

float MoveToFloat(float from, float to, float delta){
    
    if (from < to){
        from += delta;
        if (from > to){ return to; }
    } else if (from > to){
        from -= delta;
        if (from < to){ return to; }
    }
    return from;
}

Vector3 GetRandomVector(int range, float multiplier){
    return (Vector3){ GetRandomValue(-range,range)*(multiplier),
    GetRandomValue(-range,range)*(multiplier), 
    GetRandomValue(-range,range)*(multiplier)};
}