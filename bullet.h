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
    int drawDelay;
    Vector3 tri_1_1;
    Vector3 tri_1_2;
    Vector3 tri_1_3;

    int hitCount;
    void *hitTargets[8];
    
    struct BoxtreeNode* nodes[8];
    int nodeCount;
} Bullet;

Vector3 GetRandomSpherePoint(int range, float multiplier){
    return (Vector3){ GetRandomValue(-range,range)*(multiplier),
    GetRandomValue(-range,range)*(multiplier), 
    GetRandomValue(-range,range)*(multiplier)};
}

Bullet* Bullet_Construct(){
    Bullet* obj = (Bullet*)malloc(sizeof(Bullet));
    obj->isActive = false;
    obj->isArmed = false;

    obj->position = (Vector3){0,0,0};
    obj->direction = (Vector3){0,0,0};
    obj->speed = 0.5f;
    obj->velocity = (Vector3){0,0,0};

    obj->size = 0.1f;
    obj->bb.min = (Vector3){0,0,0};
    obj->bb.max = (Vector3){0,0,0};

    obj->bbColor = WHITE;
    obj->defaultColor = RED;
    obj->color = obj->defaultColor;

    obj->lifeSpan = 0;
    obj->lifeSpanLimit = 3;
    obj->armTimer = 0.2f;
    obj->destroyFlag = false;
    obj->drawDelay = 0;
    obj->tri_1_1 = (Vector3){0,0,0};
    obj->tri_1_2 = (Vector3){0,0,0};
    obj->tri_1_3 = (Vector3){0,0,0};
    
    obj->nodeCount = 0;
    memset(obj->nodes, 0, sizeof(obj->nodes));

    obj->hitCount = 0;
    memset(obj->hitTargets, 0, sizeof(obj->hitTargets));

    return obj;
}

void Bullet_Spawn(Bullet* obj, Vector3 newPos, Vector3 newDir){
    obj->isActive = true;
    obj->isArmed = false;
    obj->lifeSpan = 0;
    obj->drawDelay = 0;
    obj->position = newPos;

    obj->bb.min = (Vector3){newPos.x - obj->size / 2, newPos.y - obj->size / 2, newPos.z - obj->size / 2};
    obj->bb.max = (Vector3){newPos.x + obj->size / 2, newPos.y + obj->size / 2, newPos.z + obj->size / 2};

    obj->direction = newDir;
}

void Bullet_Destroy(Bullet* obj){
    if (!obj->isActive) return;
    
    obj->isActive = false; 
    obj->destroyFlag = false;
    obj->color = obj->defaultColor;
}

void Bullet_Reset(Bullet* obj){
    if (!obj->isActive) return;

    obj->nodeCount = 0;
    memset(obj->nodes, 0, sizeof(obj->nodes));
    obj->hitCount = 0;
    memset(obj->hitTargets, 0, sizeof(obj->hitTargets));
}

void Bullet_Update(Bullet* obj, float deltatime){
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
        Bullet_Destroy(obj);
    } else if (obj->destroyFlag){
        Bullet_Destroy(obj);
    }
}

void Bullet_Draw(Bullet* obj){
    if (!obj->isActive) return;

    if (obj->isArmed){
        //DrawCube(obj->position, obj->size, obj->size, obj->size, obj->color);
        DrawBoundingBox(obj->bb, obj->bbColor);
    } else {
        DrawCube(obj->position, obj->size, obj->size, obj->size, BLACK);
    }
    
    obj->drawDelay++;
    if (obj->drawDelay % 10 == 0){
        // update pos
        obj->tri_1_1 = Vector3Scale(Vector3Normalize(GetRandomSpherePoint(5, 1)), 0.05f);
        obj->tri_1_2 = Vector3Scale(Vector3Normalize(GetRandomSpherePoint(5, 1)), 0.05f);
        obj->tri_1_3 = Vector3Scale(Vector3Normalize(GetRandomSpherePoint(5, 1)), 0.05f);
    }

    DrawTriangle3D(Vector3Add(obj->tri_1_1, obj->position),
                    Vector3Add(obj->tri_1_2, obj->position),
                    Vector3Add(obj->tri_1_3, obj->position), LIGHTGRAY);

    DrawTriangle3D(Vector3Add(obj->tri_1_1, obj->position),
                    Vector3Add(obj->tri_1_3, obj->position),
                    Vector3Add(obj->tri_1_2, obj->position), LIGHTGRAY);

    DrawLine3D(Vector3Add(obj->tri_1_1, obj->position), Vector3Add(obj->tri_1_2, obj->position), BLACK);
    DrawLine3D(Vector3Add(obj->tri_1_2, obj->position), Vector3Add(obj->tri_1_3, obj->position), BLACK);
    DrawLine3D(Vector3Add(obj->tri_1_3, obj->position), Vector3Add(obj->tri_1_1, obj->position), BLACK);

}