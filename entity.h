#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

typedef struct Entity {
    int id;
    Vector3 position;
    Vector3 velocity;
    Vector3 size;
    bool isActive;
    Model model;
    Color color;
} Entity;

const float ENTITY_PLACEHOLDER_SIZE = 1;

Entity* CreateEntity(int id, Vector3 position, Model model);
void DestroyEntity(Entity* entity);
void UpdateEntity(Entity* entity);
void DrawEntity(const Entity* entity);
