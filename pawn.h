#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

#include "voxel.h"

typedef enum {
    PAWN_CRYSTAL,
    PAWN_TAR,
    PAWN_TURRET,
    PAWN_TEST
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
    float colorFadeSpeed;

    struct Ray aimRay;
    float shootDelay;
    float shootTick;
    int hp;

    //Voxel* rootVoxel;
    struct BoxtreeNode* nodes[16];
    int nodeCount;
} Pawn;

Pawn* Pawn_Construct(){
    Pawn* obj = (Pawn*)malloc(sizeof(Pawn));
    obj->isActive = false;

    obj->position = (Vector3){0,0,0};
    obj->velocity = (Vector3){0,0,0};

    obj->aimRay.position = obj->position;
    obj->aimRay.direction = (Vector3){0,0,-1};

    obj->size = (Vector3){0.5f,0.5f,0.5f};
    obj->color = WHITE;
    obj->defaultColor = RED;
    obj->bbColor = BLACK;
    obj->colorFadeSpeed = 0.1f;

    obj->shootDelay = 1;
    obj->shootTick = 1;
    obj->hp = 3;

    obj->nodeCount = 0;
    memset(obj->nodes, 0, sizeof(obj->nodes));

    return obj;
}

void Pawn_Spawn(Pawn* obj, Vector3 newPos, PawnType pt){
    obj->isActive = true;
    obj->pawnType = pt;
    obj->position = newPos;
    obj->bb.min = (Vector3){newPos.x - obj->size.x / 2, newPos.y - obj->size.y / 2, newPos.z - obj->size.z / 2};
    obj->bb.max = (Vector3){newPos.x + obj->size.x / 2, newPos.y + obj->size.y / 2, newPos.z + obj->size.z / 2};

    obj->aimRay.position = obj->position;
    obj->aimRay.direction = (Vector3){0,0,-1};

    obj->color = WHITE;

    obj->hp = 3;
}

void Pawn_Destroy(Pawn* obj){
    if (!obj->isActive) return;
    obj->isActive = false;
    //obj->rootVoxel->isOccupied = false;
}

void Pawn_Reset(Pawn* obj){
    if (!obj->isActive) return;

    obj->nodeCount = 0;
    memset(obj->nodes, 0, sizeof(obj->nodes));
}

int Pawn_Update(Pawn* obj, float deltaTime){
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
        case PAWN_TEST:
            obj->position = Vector3Add(obj->position, (Vector3){-0.01f,0,0});

            obj->bb.min = Vector3Add(obj->position, (Vector3){-0.25f,-0.25f,-0.25f});
            obj->bb.max = Vector3Add(obj->position, (Vector3){0.25f,0.25f,0.25f});
            break;
        default: break;
    }

    obj->aimRay.direction = (Vector3){0,0,-1};
    
    return 0;
}

void Pawn_Damage(Pawn* obj){
    obj->hp--;
    if (obj->hp <= 0){
        Pawn_Destroy(obj);
    }
}

void Pawn_Draw(Pawn* obj){
    if (!obj->isActive) return;

    // move toward default color
    Vector3 newColor = Vector3Lerp((Vector3){obj->color.r, obj->color.g, obj->color.b},
    (Vector3){obj->defaultColor.r, obj->defaultColor.g, obj->defaultColor.b}, obj->colorFadeSpeed);
    obj->color = (Color){newColor.x, newColor.y, newColor.z, 255};

    DrawCube(obj->position, obj->size.x, obj->size.y, obj->size.z, obj->color);
    DrawBoundingBox(obj->bb, obj->bbColor);
}