#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Bullet {
    bool isActive;
    bool isArmed;

    Vector3 position;
    Vector3 direction;
    float speed;
    Vector3 velocity;

    float size;
    BoundingBox bb;

    Color bbColor;
    Color color;
    Color defaultColor;
    
    float lifeSpan;
    float lifeSpanLimit;
    float armTimer;
    bool destroyFlag;

    int hitCount;
    void *hitTargets[8];
    
    struct BoxtreeNode* nodes[8];
    int nodeCount;
} Bullet;

Bullet* Create_Bullet() {
    Bullet* obj = (Bullet*)malloc(sizeof(Bullet));
    obj->isActive = false;
    obj->isArmed = false;

    obj->position = (Vector3){0,0,0};
    obj->direction = (Vector3){0,0,0};
    obj->speed = 3.0f;
    obj->velocity = (Vector3){0,0,0};

    obj->size = 0.1f;
    obj->bb.min = (Vector3){0,0,0};
    obj->bb.max = (Vector3){0,0,0};

    obj->bbColor = BLACK;
    obj->defaultColor = RED;
    obj->color = obj->defaultColor;

    obj->lifeSpan = 0;
    obj->lifeSpanLimit = 3;
    obj->armTimer = 0.2f;
    obj->destroyFlag = false;
    
    obj->nodeCount = 0;
    memset(obj->nodes, 0, sizeof(obj->nodes));

    obj->hitCount = 0;
    memset(obj->hitTargets, 0, sizeof(obj->hitTargets));

    return obj;
}

void Spawn_Bullet(Bullet* obj, Vector3 newPos, Vector3 newDir){
    obj->isActive = true;
    obj->isArmed = false;
    obj->lifeSpan = 0;
    obj->position = newPos;

    obj->bb.min = (Vector3){newPos.x - obj->size / 2, newPos.y - obj->size / 2, newPos.z - obj->size / 2};
    obj->bb.max = (Vector3){newPos.x + obj->size / 2, newPos.y + obj->size / 2, newPos.z + obj->size / 2};

    obj->direction = newDir;
}

void Destroy_Bullet(Bullet* obj){
    if (!obj->isActive) return;
    
    obj->isActive = false; 
    obj->destroyFlag = false;
    obj->color = obj->defaultColor;
}

void Reset_Bullet(Bullet* obj){
    if (!obj->isActive) return;

    obj->nodeCount = 0;
    memset(obj->nodes, 0, sizeof(obj->nodes));
    obj->hitCount = 0;
    memset(obj->hitTargets, 0, sizeof(obj->hitTargets));
}

void Update_Bullet(Bullet* obj, float deltatime){
    if (!obj->isActive) return;

    obj->velocity = Vector3Scale(Vector3Scale(obj->direction, obj->speed), deltatime);

    // update position
    Vector3 newPos = Vector3Add(obj->position, obj->velocity);
    obj->position = newPos;
    obj->bb.min = (Vector3){newPos.x - obj->size / 2, newPos.y - obj->size / 2, newPos.z - obj->size / 2};
    obj->bb.max = (Vector3){newPos.x + obj->size / 2, newPos.y + obj->size / 2, newPos.z + obj->size / 2};

    // update lifetime
    obj->lifeSpan += deltatime;
    if (obj->lifeSpan > obj->armTimer) obj->isArmed = true;
    if (obj->lifeSpan > obj->lifeSpanLimit){
        Destroy_Bullet(obj);
    } else if (obj->destroyFlag){
        Destroy_Bullet(obj);
    }
}

void Draw_Bullet(Bullet* obj) {
    if (!obj->isActive) return;

    if (obj->isArmed){
        DrawCube(obj->position, obj->size, obj->size, obj->size, obj->color);
    } else {
        DrawCube(obj->position, obj->size, obj->size, obj->size, BLACK);
    }
    
    DrawBoundingBox(obj->bb, obj->bbColor);
}