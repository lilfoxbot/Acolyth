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
    float accel;
    float accelRate;
    float drag;

    float height;
    float width;
    float size;
    BoundingBox bb;
    Color color;
    Color bbColor;

    Ray rayLeft;
    Ray rayRight;

} Gridpawn;

Gridpawn* Gridpawn_Construct(){
    Gridpawn* obj = (Gridpawn*)malloc(sizeof(Gridpawn));
    obj->isActive = false;

    obj->position = (Vector3){0,0,0};
    obj->velocity = (Vector3){0,0,0};
    obj->targetPos = (Vector3){0,0,0};
    obj->accelRate = 0.001f;
    obj->drag = 0.0005;

    obj->bb.min = (Vector3){0,0,0};
    obj->bb.max = (Vector3){0,0,0};

    obj->size = 1;
    obj->width = 1;
    obj->height = 1;

    obj->color = BLACK;
    obj->bbColor = WHITE;

    obj->rayLeft.position = (Vector3){0,0,0};
    obj->rayLeft.direction = (Vector3){0,0,-1000};
    obj->rayRight.position = (Vector3){0,0,0};
    obj->rayRight.direction = (Vector3){0,0,-1000};

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

    //obj->position = Vector3Lerp(obj->position, obj->targetPos, obj->moveSpeed + obj->accel);
    obj->position = Vector3Add(obj->position, obj->velocity);
    // drag
    obj->velocity = Vector3Lerp(obj->velocity, (Vector3){0,0,0}, obj->drag);

    obj->bb.min = (Vector3){obj->position.x - obj->width / 2,
                            obj->position.y - obj->height / 2,
                            obj->position.z - obj->width / 2};

    obj->bb.max = (Vector3){obj->position.x + obj->width / 2, 
                            obj->position.y + obj->height / 2, 
                            obj->position.z + obj->width / 2};
}

void Gridpawn_Reset(Gridpawn* obj){
    obj->position = (Vector3){0,2,7};
    obj->targetPos = (Vector3){0,2,7};
    obj->moveSpeed = 0.01f;
    obj->accel = 0.0001f;
    obj->drag = 0.015;

    obj->bb.min = (Vector3){0,0,0};
    obj->bb.max = (Vector3){0,0,0};
}

void Gridpawn_Draw(Gridpawn* obj){
    if (!obj->isActive) return;

    //DrawCube(obj->position, obj->size, obj->size, obj->size, obj->color);
    DrawBoundingBox(obj->bb, obj->bbColor);

    // targetPOS debug
    //DrawCubeWires(Vector3Add(obj->targetPos, (Vector3){0,0,-5}), obj->size, obj->size, obj->size, WHITE);

    DrawRay(obj->rayLeft, RED);
    DrawRay(obj->rayRight, RED);
}