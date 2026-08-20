#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Gridpawn{
    bool isActive;

    Vector3 position;
    Vector3 velocity;
    
    float size;
    BoundingBox bb;
    Color color;
    Color bbColor;
} Gridpawn;

Gridpawn* Create_Gridpawn(){
    Gridpawn* obj = (Gridpawn*)malloc(sizeof(Gridpawn));
    obj->isActive = false;

    obj->position = (Vector3){0,0,0};
    obj->velocity = (Vector3){0,0,0};

    obj->size = 1;
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

    obj->velocity = (Vector3){0,0,0};
    obj->position = Vector3Add(obj->position, obj->velocity);
}

void Draw_Gridpawn(Gridpawn* obj){
    if (!obj->isActive) return;

    DrawCube(obj->position, obj->size, obj->size, obj->size, obj->color);
    DrawBoundingBox(obj->bb, obj->bbColor);
}