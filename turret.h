#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

#include "voxel.h"

typedef struct Turret{
    bool isActive;

    Vector3 position;
    Vector3 velocity;
    
    float size;
    BoundingBox bb;
    Color color;
    Color defaultColor;
    Color bbColor;

    int hp;

    struct Voxel* occupiedVoxels[4];
    struct BoxtreeNode* nodes[8];
    int nodeCount;
} Turret;

Turret* Create_Turret(){
    Turret* obj = (Turret*)malloc(sizeof(Turret));
    obj->isActive = false;

    obj->position = (Vector3){0,0,0};
    obj->velocity = (Vector3){0,0,0};

    obj->size = 1;
    obj->color = BLACK;
    obj->defaultColor = BLACK;
    obj->bbColor = WHITE;

    obj->hp = 3;

    for (int i = 0; i < 4; i++){ obj->occupiedVoxels[i] = NULL; }
    obj->nodeCount = 0;
    memset(obj->nodes, 0, sizeof(obj->nodes));

    return obj;
}

void Spawn_Turret(Turret* obj, Vector3 newPos){
    obj->isActive = true;
    obj->position = newPos;
    obj->bb.min = (Vector3){newPos.x - obj->size / 2, newPos.y - obj->size / 2, newPos.z - obj->size / 2};
    obj->bb.max = (Vector3){newPos.x + obj->size / 2, newPos.y + obj->size / 2, newPos.z + obj->size / 2};

    obj->hp = 3;
}

void Destroy_Turret(Turret* obj){
    if (!obj->isActive) return;
    obj->isActive = false;
    for (int i = 0; i < 4; i++){
        if (obj->occupiedVoxels[i] != NULL){
            obj->occupiedVoxels[i]->isOccupied = false;
        }
    }
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

void Damage_Turret(Turret* obj){
    obj->hp--;
    if (obj->hp <= 0){
        Destroy_Turret(obj);
    }
}

void Draw_Turret(Turret* obj){
    if (!obj->isActive) return;

    DrawCube(obj->position, obj->size, obj->size, obj->size, obj->color);
    DrawBoundingBox(obj->bb, obj->bbColor);

    obj->color = obj->defaultColor;
}