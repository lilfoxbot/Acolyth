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

    struct Ray aimRay;

    float size;
    BoundingBox bb;

    Color bbColor;
    Color color;
    Color defaultColor;
    Color aimRayColor;
} Player;

Player* CreatePlayer(){
    Player* player = (Player*)malloc(sizeof(Player));
    player->isActive = false;

    player->position = (Vector3){0,0,0};
    player->direction = (Vector3){0,0,0};
    player->speed = 0.1f;
    player->velocity = (Vector3){0,0,0};

    player->size = 0.5f;
    player->bb.min = (Vector3){0,0,0};
    player->bb.max = (Vector3){0,0,0};

    player->bbColor = BLACK;
    player->defaultColor = GRAY;
    player->color = player->defaultColor;
    player->aimRayColor = WHITE;
}

void SpawnPlayer(Player* player, Vector3 newPos){
    player->isActive = true;
    player->position = newPos;

    player->bb.min = (Vector3){newPos.x - player->size / 2, newPos.y - player->size / 2, newPos.z - player->size / 2};
    player->bb.max = (Vector3){newPos.x + player->size / 2, newPos.y + player->size / 2, newPos.z + player->size / 2};
}

void UpdatePlayer(Player* player){

}

void DrawPlayer(Player* player){

}

void DestroyPlayer(Player* player){

}