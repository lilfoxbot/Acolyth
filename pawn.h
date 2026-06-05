#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

#include "voxel.h"

typedef enum {
    PAWN_CRYSTAL,
    PAWN_TAR,
    PAWN_TURRET
} PawnType;

typedef struct Pawn{
    bool isActive;
    PawnType pawnType;

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
} Pawn;

Pawn* Create_Pawn(){
    Pawn* obj = (Pawn*)malloc(sizeof(Pawn));
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

void Spawn_Pawn(Pawn* obj, Vector3 newPos, PawnType pt){
    obj->isActive = true;
    obj->pawnType = pt;
    obj->position = newPos;
    obj->bb.min = (Vector3){newPos.x - obj->size.x / 2, newPos.y - obj->size.y / 2, newPos.z - obj->size.z / 2};
    obj->bb.max = (Vector3){newPos.x + obj->size.x / 2, newPos.y + obj->size.y / 2, newPos.z + obj->size.z / 2};

    obj->aimRay.position = obj->position;
    obj->aimRay.direction = (Vector3){0,0,-1};

    obj->hp = 3;
}

void Destroy_Pawn(Pawn* obj){
    if (!obj->isActive) return;
    obj->isActive = false;
    obj->rootVoxel->isOccupied = false;
}

void Reset_Pawn(Pawn* obj){
    if (!obj->isActive) return;

    obj->nodeCount = 0;
    memset(obj->nodes, 0, sizeof(obj->nodes));
}

int Update_Pawn(Pawn* obj, float deltaTime){
    if (!obj->isActive) return 0;

    switch (obj->pawnType){
        case PAWN_TURRET:
            obj->shootTick -= deltaTime;
            if (obj->shootTick <= 0){
                obj->shootTick = obj->shootDelay;
                return 1;
            }
            break;
        case PAWN_CRYSTAL: break;
        case PAWN_TAR: break;
        default: break;
    }

    obj->aimRay.direction = (Vector3){0,0,-1};
    
    return 0;
}

void Damage_Pawn(Pawn* obj){
    obj->hp--;
    if (obj->hp <= 0){
        Destroy_Pawn(obj);
    }
}

void Draw_Pawn(Pawn* obj){
    if (!obj->isActive) return;

    DrawCube(obj->position, obj->size.x, obj->size.y, obj->size.z, obj->color);
    DrawBoundingBox(obj->bb, obj->bbColor);
    DrawRay(obj->aimRay, obj->bbColor);

    obj->color = obj->defaultColor;
}