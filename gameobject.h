#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct GameObject{
    bool isActive;

    Vector3 position;
    Vector3 velocity;
    
    float size;
    BoundingBox bb;
    Color color;
    Color bbColor;
} GameObject;

// GameObject* This_Construct(){
//     GameObject* obj = (GameObject*)malloc(sizeof(GameObject));
//     obj->isActive = false;

//     obj->position = (Vector3){0,0,0};
//     obj->velocity = (Vector3){0,0,0};

//     obj->size = 1;
//     obj->color = BLACK;
//     obj->bbColor = WHITE;

//     return obj;
// }

// void This_Spawn(GameObject* obj, Vector3 newPos){
//     obj->isActive = true;
//     obj->position = newPos;
// }

// void This_Destroy(GameObject* obj){
//     if (!obj->isActive) return;
//     obj->isActive = false;
// }

// void This_Update(GameObject* obj, float deltaTime){
//     if (!obj->isActive) return;

//     obj->velocity = (Vector3){0,0,0};
//     obj->position = Vector3Add(obj->position, obj->velocity);
// }

// void This_Draw(GameObject* obj){
//     if (!obj->isActive) return;

//     DrawCube(obj->position, obj->size, obj->size, obj->size, obj->color);
//     DrawBoundingBox(obj->bb, obj->bbColor);
// }