#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Turret{
    bool isActive;

    Vector3 position;
    Vector3 velocity;
    
    float size;
    BoundingBox bb;
    Color color;
    Color bbColor;

    int nodeCount;
    struct BoxtreeNode* nodes[8];
} Turret;

Turret* Create_Turret(){
    Turret* obj = (Turret*)malloc(sizeof(Turret));
    obj->isActive = false;

    obj->position = (Vector3){0,0,0};
    obj->velocity = (Vector3){0,0,0};

    obj->size = 1;
    obj->color = BLACK;
    obj->bbColor = WHITE;

    obj->nodeCount = 0;
    memset(obj->nodes, 0, sizeof(obj->nodes));

    return obj;
}

void Spawn_Turret(Turret* obj, Vector3 newPos){
    obj->isActive = true;
    obj->position = newPos;
    obj->bb.min = (Vector3){newPos.x - obj->size / 2, newPos.y - obj->size / 2, newPos.z - obj->size / 2};
    obj->bb.max = (Vector3){newPos.x + obj->size / 2, newPos.y + obj->size / 2, newPos.z + obj->size / 2};
}

void Destroy_Turret(Turret* obj){
    if (!obj->isActive) return;
    obj->isActive = false;
}

void Reset_Turret(Turret* obj){
    if (!obj->isActive) return;

    obj->nodeCount = 0;
    memset(obj->nodes, 0, sizeof(obj->nodes));
}

void Update_Turret(Turret* obj, float deltaTime){
    if (!obj->isActive) return;

    //obj->velocity = (Vector3){0,0,0};
    //obj->position = Vector3Add(obj->position, obj->velocity);
}

void Draw_Turret(Turret* obj){
    if (!obj->isActive) return;

    DrawCube(obj->position, obj->size, obj->size, obj->size, obj->color);
    DrawBoundingBox(obj->bb, obj->bbColor);
}