#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern bool myDebug;

typedef struct BulletPoly{
    Vector3 A;
    Vector3 B;
    Vector3 C;

    Color color;
} BulletPoly;

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

    int hitCount;
    void *hitTargets[8];
    
    struct BoxtreeNode* nodes[8];
    int nodeCount;

    struct BulletPoly* bp[5];
    int polyCount;
} Bullet;

Vector3 GetRandomSpherePoint(int range, float multiplier, float scale){
    Vector3 returnVec; 
    returnVec = (Vector3){GetRandomValue(-range,range)*(multiplier),
    GetRandomValue(-range,range)*(multiplier), 
    GetRandomValue(-range,range)*(multiplier)};
    
    return Vector3Scale(Vector3Normalize(returnVec), scale);
}

void UpdateBulletPoly(BulletPoly* poly){
    // get points for a triangle that shares centroid with bullet
    // get normal from center
    poly->A = GetRandomSpherePoint(5, 1, 0.05f);
    // get a perpendicular point (dot product)
    Vector3 rand_vec = GetRandomSpherePoint(5, 1, 0.05f);
    float dot = Vector3DotProduct(poly->A, rand_vec);
    // unit tangent vector
    Vector3 utv = {rand_vec.x - dot * poly->A.x,
                    rand_vec.y - dot * poly->A.y,
                    rand_vec.z - dot * poly->A.z};
    utv = Vector3Normalize(utv);

    float sin_120 = sqrt(3.0) / 2.0;
    
    poly->B = (Vector3){-0.5 * poly->A.x + sin_120 * utv.x,
                    -0.5 * poly->A.y + sin_120 * utv.y,
                    -0.5 * poly->A.z + sin_120 * utv.z};
    
    poly->C = (Vector3){-0.5 * poly->A.x - sin_120 * utv.x,
                    -0.5 * poly->A.y - sin_120 * utv.y,
                    -0.5 * poly->A.z - sin_120 * utv.z};

    poly->B = Vector3Scale(poly->B, 0.1f);
    poly->C = Vector3Scale(poly->C, 0.1f);
}

void DrawBulletPoly(Bullet* obj, BulletPoly* poly){
    
    if (myDebug){ DrawBoundingBox(obj->bb, WHITE); }

    DrawLine3D(Vector3Add(poly->A, obj->position), Vector3Add(poly->B, obj->position), YELLOW);
    DrawLine3D(Vector3Add(poly->B, obj->position), Vector3Add(poly->C, obj->position), YELLOW);
    DrawLine3D(Vector3Add(poly->C, obj->position), Vector3Add(poly->A, obj->position), YELLOW);
}

BulletPoly* BulletPoly_Construct(){
    BulletPoly* obj = (BulletPoly*)malloc(sizeof(BulletPoly));
    obj->A = (Vector3){0,0,0};
    obj->B = (Vector3){0,0,0};
    obj->C = (Vector3){0,0,0};
    obj->color = WHITE;
    return obj;
}

Bullet* Bullet_Construct(){
    Bullet* obj = (Bullet*)malloc(sizeof(Bullet));
    obj->isActive = false;
    obj->isArmed = false;

    obj->position = (Vector3){0,0,0};
    obj->direction = (Vector3){0,0,0};
    obj->speed = 3;
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
    
    obj->nodeCount = 0;
    memset(obj->nodes, 0, sizeof(obj->nodes));

    obj->hitCount = 0;
    memset(obj->hitTargets, 0, sizeof(obj->hitTargets));

    obj->polyCount = 5;
    for (int i = 0; i < 5; i++){
        obj->bp[i] = BulletPoly_Construct();
    }

    obj->bp[0]->color = WHITE;
    obj->bp[1]->color = LIGHTGRAY;
    obj->bp[2]->color = GRAY;
    obj->bp[3]->color = DARKGRAY;
    obj->bp[4]->color = WHITE;
    
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

    obj->direction = Vector3Normalize(newDir);
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
        //DrawBoundingBox(obj->bb, obj->bbColor);
    } else {
        //DrawCube(obj->position, obj->size, obj->size, obj->size, BLACK);
    }
    
    obj->drawDelay++;
    if (obj->drawDelay % 5 == 0){
        for (int i = 0; i < 3; i++){
            UpdateBulletPoly(obj->bp[i]);
        }
    }

    for (int i = 0; i < obj->polyCount; i++){
        DrawBulletPoly(obj, obj->bp[i]);
    }
}