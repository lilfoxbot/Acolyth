#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Gridpawn{
    bool isActive;

    Vector3 position;
    Vector3 velocity;
    Vector3 targetPos;

    float moveVector;
    float targetVector;

    bool moving;
    float moveSpeed;

    float height;
    float width;
    float size;
    BoundingBox bb;
    Color color;
    Color bbColor;
} Gridpawn;

Gridpawn* Gridpawn_Construct(){
    Gridpawn* obj = (Gridpawn*)malloc(sizeof(Gridpawn));
    obj->isActive = false;

    obj->position = (Vector3){0,1,0};
    obj->velocity = (Vector3){0,0,0};
    obj->targetPos = (Vector3){0,1,0};
    obj->moving = false;
    obj->moveSpeed = 0.04f;

    obj->bb.min = (Vector3){0,0,0};
    obj->bb.max = (Vector3){0,0,0};

    obj->size = 1;
    obj->width = 1;
    obj->height = 1;

    obj->color = BLACK;
    obj->bbColor = WHITE;

    return obj;
}

void Gridpawn_Spawn(Gridpawn* obj, Vector3 newPos){
    obj->isActive = true;
    obj->position = newPos;
}

void Gridpawn_Destroy(Gridpawn* obj){
    if (!obj->isActive) return;
    obj->isActive = false;
}

void Gridpawn_Update(Gridpawn* obj, float deltaTime){
    if (!obj->isActive) return;

    if (obj->moving){
        if (Vector3Distance(obj->position, obj->targetPos) < obj->moveSpeed*2){
            obj->moving = false;
            obj->position = obj->targetPos;
        }
    }

    obj->position = Vector3Add(obj->position, obj->velocity);

    obj->bb.min = (Vector3){obj->position.x - obj->width / 2,
                            obj->position.y - obj->height / 2,
                            obj->position.z - obj->width / 2};

    obj->bb.max = (Vector3){obj->position.x + obj->width / 2, 
                            obj->position.y + obj->height / 2, 
                            obj->position.z + obj->width / 2};
}

void Gridpawn_Reset(Gridpawn* obj){
    obj->position = (Vector3){0,2,0};
    obj->velocity = (Vector3){0,0,0};
    obj->targetPos = (Vector3){0,2,0};
    obj->moving = false;
    obj->moveSpeed = 0.02f;

    obj->bb.min = (Vector3){0,0,0};
    obj->bb.max = (Vector3){0,0,0};
}

void Gridpawn_Draw(Gridpawn* obj){
    if (!obj->isActive) return;

    DrawCube(obj->position, obj->size, obj->size, obj->size, obj->color);
    DrawBoundingBox(obj->bb, obj->bbColor);

    // targetPOS debug
    //DrawCubeWires(obj->targetPos, obj->size, obj->size, obj->size, WHITE);
}