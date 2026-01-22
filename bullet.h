#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Bullet {
    bool isActive;

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
    bool destroyFlag;

    int nodeCount;
    struct BoxtreeNode* nodes[8];
} Bullet;

Bullet* CreateBullet() {
    Bullet* bullet = (Bullet*)malloc(sizeof(Bullet));
    bullet->isActive = false;

    bullet->position = (Vector3){0,0,0};
    bullet->direction = (Vector3){0,0,0};
    bullet->speed = 0.1f;
    bullet->velocity = (Vector3){0,0,0};

    bullet->size = 0.1f;
    bullet->bb.min = (Vector3){0,0,0};
    bullet->bb.max = (Vector3){0,0,0};

    bullet->bbColor = BLACK;
    bullet->defaultColor = RED;
    bullet->color = bullet->defaultColor;

    bullet->lifeSpan = 3;
    bullet->destroyFlag = false;
    
    bullet->nodeCount = 0;
    memset(bullet->nodes, 0, sizeof(bullet->nodes));

    return bullet;
}

void SpawnBullet(Bullet* bullet, Vector3 newPos, Vector3 newDir){
    bullet->isActive = true;
    bullet->lifeSpan = 3;
    bullet->position = newPos;

    bullet->bb.min = (Vector3){newPos.x - bullet->size / 2, newPos.y - bullet->size / 2, newPos.z - bullet->size / 2};
    bullet->bb.max = (Vector3){newPos.x + bullet->size / 2, newPos.y + bullet->size / 2, newPos.z + bullet->size / 2};

    bullet->direction = newDir;
}

void DestroyBullet(Bullet* bullet) {
    if (!bullet->isActive) return;
    
    bullet->isActive = false; 
    bullet->destroyFlag = false;
    bullet->color = bullet->defaultColor;

    //free(bullet);
}

void ResetBullet(Bullet* bullet){
    if (!bullet->isActive) return;

    bullet->nodeCount = 0;
    memset(bullet->nodes, 0, sizeof(bullet->nodes));
}

void UpdateBullet(Bullet* bullet, float deltatime){
    if (!bullet->isActive) return;

    bullet->velocity = Vector3Scale(bullet->direction, bullet->speed);

    // update position
    Vector3 newPos = Vector3Add(bullet->position, bullet->velocity);
    bullet->position = newPos;
    bullet->bb.min = (Vector3){newPos.x - bullet->size / 2, newPos.y - bullet->size / 2, newPos.z - bullet->size / 2};
    bullet->bb.max = (Vector3){newPos.x + bullet->size / 2, newPos.y + bullet->size / 2, newPos.z + bullet->size / 2};

    // update lifetime
    bullet->lifeSpan -= deltatime;
    if (bullet->lifeSpan <= 0){
        DestroyBullet(bullet);
    } else if (bullet->destroyFlag){
        DestroyBullet(bullet);
    }
}

void DrawBullet(Bullet* bullet) {
    if (!bullet->isActive) return;

    DrawCube(bullet->position, bullet->size, bullet->size, bullet->size, bullet->color);
    DrawBoundingBox(bullet->bb, bullet->bbColor);
}