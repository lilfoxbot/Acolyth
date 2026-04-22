#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Template{
    bool isActive;

    Vector3 position;
    Vector3 velocity;
    
    float size;
    BoundingBox bb;
    Color color;
    Color bbColor;
} Template;

Template* Create_This(){
    Template* temp = (Template*)malloc(sizeof(Template));
    temp->isActive = false;

    temp->position = (Vector3){0,0,0};
    temp->velocity = (Vector3){0,0,0};

    temp->size = 1;
    temp->color = BLACK;
    temp->bbColor = WHITE;

    return temp;
}

void Spawn_This(Template* temp, Vector3 newPos){
    temp->isActive = true;
    temp->position = newPos;
}

void Destroy_This(Template* temp){
    if (!temp->isActive) return;
    temp->isActive = false;
}

void Update_This(Template* temp, float deltaTime){
    if (!temp->isActive) return;

    temp->velocity = (Vector3){0,0,0};
    temp->position = Vector3Add(temp->position, temp->velocity);    
}

void Draw_This(Template* temp){
    if (!temp->isActive) return;

    DrawCube(temp->position, temp->size, temp->size, temp->size, temp->color);
    DrawBoundingBox(temp->bb, temp->bbColor);
}