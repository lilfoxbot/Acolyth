#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

#include "voxel.h"

typedef enum {
    ROOK_CRYSTAL,
    ROOK_TAR,
    ROOK_TURRET
} RookType;

typedef struct Rook{
    bool isActive;
    RookType rookType;

    Vector3 position;
    Vector3 velocity;
    Vector3 size;

    BoundingBox bb;
    Color color;
    Color defaultColor;
    Color bbColor;

    struct Ray aimRay;
    float shootDelay;
    float shootTick;
    int hp;

    Voxel* rootVoxel;
    struct BoxtreeNode* nodes[16];
    int nodeCount;
} Rook;

Rook* Create_Rook(){
    Rook* obj = (Rook*)malloc(sizeof(Rook));
    obj->isActive = false;

    obj->position = (Vector3){0,0,0};
    obj->velocity = (Vector3){0,0,0};

    obj->aimRay.position = obj->position;
    obj->aimRay.direction = (Vector3){0,0,-1};

    obj->size = (Vector3){0.7f,2,0.7f};
    obj->color = BLACK;
    obj->defaultColor = LIGHTGRAY;
    obj->bbColor = BLACK;

    obj->shootDelay = 1;
    obj->shootTick = 1;
    obj->hp = 3;


    obj->nodeCount = 0;
    memset(obj->nodes, 0, sizeof(obj->nodes));

    return obj;
}

void Spawn_Rook(Rook* obj, Vector3 newPos){
    obj->isActive = true;
    obj->position = newPos;
    obj->bb.min = (Vector3){newPos.x - obj->size.x / 2, newPos.y - obj->size.y / 2, newPos.z - obj->size.z / 2};
    obj->bb.max = (Vector3){newPos.x + obj->size.x / 2, newPos.y + obj->size.y / 2, newPos.z + obj->size.z / 2};

    obj->aimRay.position = obj->position;
    obj->aimRay.direction = (Vector3){0,0,-1};

    obj->hp = 3;
}

void Destroy_Rook(Rook* obj){
    if (!obj->isActive) return;
    obj->isActive = false;
    obj->rootVoxel->isOccupied = false;
}

void Reset_Rook(Rook* obj){
    if (!obj->isActive) return;

    obj->nodeCount = 0;
    memset(obj->nodes, 0, sizeof(obj->nodes));
}

int Update_Rook(Rook* obj, float deltaTime){
    if (!obj->isActive) return 0;

    switch (obj->rookType){
        case ROOK_TURRET:
            obj->shootTick -= deltaTime;
            if (obj->shootTick <= 0){
                obj->shootTick = obj->shootDelay;
                return 1;
            }
            break;
        case ROOK_CRYSTAL: break;
        case ROOK_TAR: break;
        default: break;
    }

    obj->aimRay.direction = (Vector3){0,0,-1};
    
    return 0;
}

void Damage_Rook(Rook* obj){
    obj->hp--;
    if (obj->hp <= 0){
        Destroy_Rook(obj);
    }
}

void Draw_Rook(Rook* obj){
    if (!obj->isActive) return;

    DrawCube(obj->position, obj->size.x, obj->size.y, obj->size.z, obj->color);
    DrawBoundingBox(obj->bb, obj->bbColor);
    DrawRay(obj->aimRay, obj->bbColor);

    obj->color = obj->defaultColor;
}