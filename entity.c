#include "entity.h" 
#include <stdlib.h>
#include <stdio.h>

Entity* CreateEntity(int id, Vector3 position, Model model) {
    Entity* newEntity = (Entity*)malloc(sizeof(Entity));
    if (newEntity != NULL) {
        newEntity->id = id;
        newEntity->position = position;
        newEntity->velocity = (Vector3){0,0,0.05f};
        newEntity->size = (Vector3){1,1,1};
        newEntity->isActive = true;
        newEntity->model = model;
        newEntity->color = (Color){ GetRandomValue(100,255), GetRandomValue(100,255), GetRandomValue(100,255), 255 };
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
    if (entity != NULL && entity->isActive) {
        if (entity->model.meshCount > 0){
            DrawModel(entity->model, entity->position, 1.0f, entity->color);
            DrawModelWires(entity->model, entity->position, 1.0f, BLACK);
        } else {
            DrawCube(entity->position, entity->size.x, entity->size.y, entity->size.z, MAGENTA);
            DrawCubeWires(entity->position, entity->size.x, entity->size.y, entity->size.z, BLACK);
        }
    }
}