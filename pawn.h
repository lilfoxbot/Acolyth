#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    SEEKER,
    SHOOTER
} PawnType;

typedef struct Pawn {
    bool isActive;
    PawnType type;

    Vector3 position;
    Vector3 direction;
    float speed;
    Vector3 velocity;

    float size;
    BoundingBox bb;

    Color bbColor;
    Color color;
    Color defaultColor;
    
    int health;
    bool destroyFlag;

    int nodeCount;
    struct BoxtreeNode* nodes[8];
} Pawn;

Pawn* CreatePawn() {
    Pawn* pawn = (Pawn*)malloc(sizeof(Pawn));
    pawn->isActive = false;
    pawn->type = SEEKER;

    pawn->position = (Vector3){0,0,0};
    pawn->direction = (Vector3){0,0,0};
    pawn->speed = 0.1f;
    pawn->velocity = (Vector3){0,0,0};

    pawn->size = 0.5f;
    pawn->bb.min = (Vector3){0,0,0};
    pawn->bb.max = (Vector3){0,0,0};

    pawn->bbColor = BLACK;
    pawn->defaultColor = GRAY;
    pawn->color = pawn->defaultColor;

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

void UpdatePawn(Pawn* pawn, float deltatime){
    if (!pawn->isActive) return;

    // what kind of pawn?

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
}