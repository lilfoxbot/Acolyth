#include "entity.h" 
#include <stdlib.h>
#include <stdio.h>

Entity* CreateEntity(int id, Vector3 position) {
    Entity* newEntity = (Entity*)malloc(sizeof(Entity));
    if (newEntity != NULL) {
        newEntity->id = id;
        newEntity->position = position;
        newEntity->velocity = (Vector3){0,0,0.05f};
        newEntity->size = (Vector3){1,GetRandomValue(1,4),1};
        newEntity->isActive = true;
        newEntity->hasModel = false;
    }
    return newEntity;
}

void DestroyEntity(Entity* entity) {
    if (entity != NULL) {
        free(entity);
    }
}

void UpdateEntity(Entity* entity) {
    if (entity != NULL && entity->isActive) {
        entity->position = Vector3Add(entity->position, entity->velocity);
    }
}

void DrawEntity(const Entity* entity) {
    if (entity != NULL && entity->isActive && !entity->hasModel) {
        DrawCube(entity->position, entity->size.x, entity->size.y, entity->size.z, SKYBLUE);
        DrawCubeWires(entity->position, entity->size.x, entity->size.y, entity->size.z, BLACK);
    }
}