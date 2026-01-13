#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Bullet {
    Vector3 position;
    Vector3 direction;
    float speed;
    Vector3 velocity;
    float size;
    BoundingBox bb;
    Color bbColor;
    Color color;
    Color defaultColor;
    bool isActive;
    float lifeSpan;
} Bullet;

Bullet* CreateBullet(Vector3 position, Vector3 direction) {
    Bullet* bullet = (Bullet*)malloc(sizeof(Bullet));
    bullet->position = position;
    bullet->direction = direction;
    bullet->speed = 0.05f;
    bullet->velocity = Vector3Scale(direction, bullet->speed);
    bullet->size = 0.1f;
    bullet->bb.min = (Vector3){position.x - bullet->size / 2, position.y - bullet->size / 2, position.z - bullet->size / 2};
    bullet->bb.max = (Vector3){position.x + bullet->size / 2, position.y + bullet->size / 2, position.z + bullet->size / 2};
    bullet->bbColor = BLACK;
    bullet->defaultColor = RED;
    bullet->color = bullet->defaultColor;
    bullet->isActive = true;
    bullet->lifeSpan = 3;
    return bullet;
}

void DestroyBullet(Bullet* bullet) {
    if (bullet == NULL) return;
    
    free(bullet);
}

int UpdateBullet(Bullet* bullet, float deltatime){
    if (bullet == NULL) return 1;

    // update position
    Vector3 newPos = Vector3Add(bullet->position, bullet->velocity);
    bullet->position = newPos;
    bullet->bb.min = (Vector3){newPos.x - bullet->size / 2, newPos.y - bullet->size / 2, newPos.z - bullet->size / 2};
    bullet->bb.max = (Vector3){newPos.x + bullet->size / 2, newPos.y + bullet->size / 2, newPos.z + bullet->size / 2};

    // update lifetime
    bullet->lifeSpan -= deltatime;
    if (bullet->lifeSpan <= 0){
        DestroyBullet(bullet);
        return 0;
    }

    return 1;
}

void DrawBullet(Bullet* bullet) {
    if (bullet == NULL) return;

    DrawCube(bullet->position, bullet->size,bullet->size,bullet->size, bullet->color);
    DrawBoundingBox(bullet->bb, bullet->bbColor);

}