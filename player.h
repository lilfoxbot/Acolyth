#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Player {
    bool isActive;

    float shootDelay;
    float shootTick;

    Vector3 position;
    Vector3 direction;
    float speed;
    Vector3 velocity;
    float gravity;
    float jumpVel;

    struct Ray aimRay;

    float size;
    float height;
    float width;
    BoundingBox bb;

    Color bbColor;
    Color color;
    Color defaultColor;
    Color aimRayColor;

    int nodeCount;
    struct BoxtreeNode* nodes[8];
} Player;

Player* CreatePlayer(){
    Player* player = (Player*)malloc(sizeof(Player));
    player->isActive = false;

    player->position = (Vector3){0,0,0};
    player->direction = (Vector3){0,0,0};
    player->speed = 0.1f;
    player->velocity = (Vector3){0,0,0};
    player->gravity = -0.08f;
    player->jumpVel = 0.06f;

    player->size = 1;
    player->height = 1.5f;
    player->width = 0.5f;

    player->bb.min = (Vector3){0,0,0};
    player->bb.max = (Vector3){0,0,0};

    player->bbColor = BLACK;
    player->defaultColor = PURPLE;
    player->color = player->defaultColor;
    player->aimRayColor = WHITE;

    player->nodeCount = 0;
    memset(player->nodes, 0, sizeof(player->nodes));

    return player;
}

void SpawnPlayer(Player* player, Vector3 newPos){
    player->isActive = true;
    player->position = newPos;
}

void UpdatePlayer(Player* player, Vector4 playerInput, float dt){
    if (!player->isActive) return;

    // gravity
    player->velocity.y += player->gravity*dt;
    // clamps
    if (player->position.y < (0.5f) + player->height/2){
        player->position.y = (0.5f) + player->height/2;
        player->velocity.y = 0;
    }

    player->velocity.x = playerInput.x * dt;
    player->velocity.z = playerInput.z * dt;

    if (playerInput.w == 1){
        player->velocity.y = player->jumpVel;
    }

    player->position = Vector3Add(player->position, player->velocity);

    player->bb.min = (Vector3){player->position.x - player->width / 2,
                            player->position.y - player->height / 2,
                            player->position.z - player->width / 2};

    player->bb.max = (Vector3){player->position.x + player->width / 2, 
                            player->position.y + player->height / 2, 
                            player->position.z + player->width / 2};
}

void DrawPlayer(Player* player){
    if (!player->isActive) return;

    DrawCube(player->position, player->width, player->height, player->width, player->color);
    DrawBoundingBox(player->bb, player->bbColor);
}

void ResetPlayer(Player* player){
    if (!player->isActive) return;

    player->nodeCount = 0;
    memset(player->nodes, 0, sizeof(player->nodes));
}

void DestroyPlayer(Player* player){
    player->isActive = false;
}