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

    float height;
    float width;
    float size;
    BoundingBox bb;
    Color color;
    Color bbColor;
} Gridpawn;

Gridpawn* Create_Gridpawn(){
    Gridpawn* obj = (Gridpawn*)malloc(sizeof(Gridpawn));
    obj->isActive = false;

    obj->position = (Vector3){0,2,0};
    obj->velocity = (Vector3){0,0,0};
    obj->targetPos = (Vector3){0,0,0};

    obj->bb.min = (Vector3){0,0,0};
    obj->bb.max = (Vector3){0,0,0};

    obj->size = 1;
    obj->width = 1;
    obj->height = 1;

    obj->color = BLACK;
    obj->bbColor = WHITE;

    return obj;
}

void Spawn_Gridpawn(Gridpawn* obj, Vector3 newPos){
    obj->isActive = true;
    obj->position = newPos;
}

void Destroy_Gridpawn(Gridpawn* obj){
    if (!obj->isActive) return;
    obj->isActive = false;
}

void Update_Gridpawn(Gridpawn* obj, float deltaTime){
    if (!obj->isActive) return;

    //obj->velocity = obj->targetPos;
    //obj->position = Vector3Add(obj->position, obj->velocity);
    //obj->position = obj->targetPos;

    obj->bb.min = (Vector3){obj->position.x - obj->width / 2,
                            obj->position.y - obj->height / 2,
                            obj->position.z - obj->width / 2};

    obj->bb.max = (Vector3){obj->position.x + obj->width / 2, 
                            obj->position.y + obj->height / 2, 
                            obj->position.z + obj->width / 2};
}

void Draw_Gridpawn(Gridpawn* obj){
    if (!obj->isActive) return;

    DrawCube(obj->position, obj->size, obj->size, obj->size, obj->color);
    DrawBoundingBox(obj->bb, obj->bbColor);
}