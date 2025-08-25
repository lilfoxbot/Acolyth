#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

#ifndef ENTITY_H
#define ENTITY_H

typedef struct {
    int id;
    Vector3 position;
    bool isActive;
} Entity;

// Function declarations related to Entity management
Entity* createEntity(int id, Vector3 position);
void destroyEntity(Entity* entity);
void updateEntity(Entity* entity, float deltaTime);
void renderEntity(const Entity* entity);

#endif // ENTITY_H