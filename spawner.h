#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Spawner{
    bool isActive;
    Vector3 position;
    
    float spawnTimer;
} Spawner;

Spawner* Spawner_Construct(){
    Spawner* obj = (Spawner*)malloc(sizeof(Spawner));
    obj->isActive = false;
    obj->position = (Vector3){0,0,0};

    obj->spawnTimer = 0;
    return obj;
}

void Spawner_Update(Spawner* obj, float deltaTime){
    if (!obj->isActive) return;

    // when enough time passes, spawn a pawn
    obj->spawnTimer += deltaTime;
}

// void This_Draw(GameObject* obj){
//     if (!obj->isActive) return;

//     DrawCube(obj->position, obj->size, obj->size, obj->size, obj->color);
//     DrawBoundingBox(obj->bb, obj->bbColor);
// }