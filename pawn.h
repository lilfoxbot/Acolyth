#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bullet.h"

typedef enum {
    SEEKER,
    SHOOTER
} PawnType;

typedef struct Pawn {
    bool isActive;
    PawnType type;

    float shootDelay;
    float shootTick;

    Vector3 position;
    Vector3 direction;
    float speed;
    Vector3 velocity;

    struct Ray aimRay;

    float size;
    BoundingBox bb;

    Color bbColor;
    Color color;
    Color defaultColor;
    Color aimRayColor;
    
    int health;
    bool destroyFlag;

    int nodeCount;
    struct BoxtreeNode* nodes[8];
} Pawn;

Pawn* CreatePawn() {
    Pawn* pawn = (Pawn*)malloc(sizeof(Pawn));
    pawn->isActive = false;
    pawn->type = SEEKER;

    pawn->shootDelay = 1;
    pawn->shootTick = 1;

    pawn->position = (Vector3){0,0,0};
    pawn->direction = (Vector3){0,0,0};
    pawn->speed = 0.1f;
    pawn->velocity = (Vector3){0,0,0};

    pawn->aimRay.position = pawn->position;
    pawn->aimRay.direction = (Vector3){0,0,1};

    pawn->size = 0.5f;
    pawn->bb.min = (Vector3){0,0,0};
    pawn->bb.max = (Vector3){0,0,0};

    pawn->bbColor = BLACK;
    pawn->defaultColor = GRAY;
    pawn->color = pawn->defaultColor;
    pawn->aimRayColor = WHITE;

    pawn->health = 3;
    pawn->destroyFlag = false;
    
    pawn->nodeCount = 0;
    memset(pawn->nodes, 0, sizeof(pawn->nodes));

    return pawn;
}

void SpawnPawn(Pawn* pawn, PawnType type, Vector3 newPos){
    pawn->isActive = true;
    pawn->type = type;
    pawn->position = newPos;

    switch(type){
        case SEEKER:
            pawn->color = GREEN;
            break;
        case SHOOTER:
            pawn->color = BLUE;
            break;
        default:
            break;
    }

    pawn->bb.min = (Vector3){newPos.x - pawn->size / 2, newPos.y - pawn->size / 2, newPos.z - pawn->size / 2};
    pawn->bb.max = (Vector3){newPos.x + pawn->size / 2, newPos.y + pawn->size / 2, newPos.z + pawn->size / 2};
}

void DestroyPawn(Pawn* pawn) {
    if (!pawn->isActive) return;
    
    pawn->isActive = false; 
    pawn->destroyFlag = false;
    pawn->color = pawn->defaultColor;
}

void ResetPawn(Pawn* pawn){
    if (!pawn->isActive) return;

    pawn->nodeCount = 0;
    memset(pawn->nodes, 0, sizeof(pawn->nodes));
}

int UpdatePawn(Pawn* pawn, float deltaTime){
    if (!pawn->isActive) return 0;

    switch(pawn->type){
        case SEEKER:
            pawn->velocity = Vector3Scale((Vector3){0,0.1f,0}, deltaTime);

            pawn->aimRay.position = pawn->position;
            pawn->aimRay.direction = (Vector3){0,0,1};
            return 0;
            break;
        case SHOOTER:
            //pawn->velocity = Vector3Scale((Vector3){0,0.1f,0}, deltatime);
            pawn->aimRay.position = pawn->position;
            pawn->aimRay.direction = (Vector3){0,0,1};

            pawn->shootTick -= deltaTime;
            if (pawn->shootTick <= 0){
                pawn->shootTick = pawn->shootDelay;
                // shoot
                return 1;
            }
            return 0;
            break;
        default:
            return 0;
            break;
    }

    // update position
    Vector3 newPos = Vector3Add(pawn->position, pawn->velocity);
    pawn->position = newPos;
    pawn->bb.min = (Vector3){newPos.x - pawn->size / 2, newPos.y - pawn->size / 2, newPos.z - pawn->size / 2};
    pawn->bb.max = (Vector3){newPos.x + pawn->size / 2, newPos.y + pawn->size / 2, newPos.z + pawn->size / 2};

    // update lifetime
    //pawn->lifeSpan -= deltatime;
    if (pawn->destroyFlag){
        DestroyPawn(pawn);
    }
}

void DrawPawn(Pawn* pawn) {
    if (!pawn->isActive) return;

    DrawCube(pawn->position, pawn->size, pawn->size, pawn->size, pawn->color);
    DrawBoundingBox(pawn->bb, pawn->bbColor);
    DrawRay(pawn->aimRay,WHITE);
}