// entity.c
#include "entity.h" // Include the header file for declarations
#include <stdlib.h> // For malloc/free
#include <stdio.h>  // For printf, if needed for debugging/logging

Entity* createEntity(int id, Vector3 position) {
        Entity* newEntity = (Entity*)malloc(sizeof(Entity));
        if (newEntity != NULL) {
            newEntity->id = id;
            newEntity->position = position;
            newEntity->isActive = true;
        }
        return newEntity;
    }

    void destroyEntity(Entity* entity) {
        if (entity != NULL) {
            free(entity);
        }
    }

    void updateEntity(Entity* entity, float deltaTime) {
        if (entity != NULL && entity->isActive) {
            // Update logic here
            //entity->x += 1.0 * deltaTime; // Example: move right
        }
    }

    void renderEntity(const Entity* entity) {
        if (entity != NULL && entity->isActive) {
            // Rendering logic here (e.g., print position)
            //printf("Entity %d at (%.2f, %.2f)\n", entity->id, entity->x, entity->y);
        }
    }