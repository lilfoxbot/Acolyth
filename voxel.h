#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Voxel {
    Vector3 coordinates;
    Vector3 position;
    float size;
    BoundingBox bb;
    Color bbColor;
    Color color;
    bool isActive;
} Voxel;

Voxel* CreateVoxel(Vector3 position, Vector3 coordinates, float size) {
    Voxel* voxel = (Voxel*)malloc(sizeof(Voxel));
    voxel->position = position;
    voxel->coordinates = coordinates;
    voxel->size = size;
    voxel->bb.min = (Vector3){position.x - size / 2, position.y - size / 2, position.z - size / 2};
    voxel->bb.max = (Vector3){position.x + size / 2, position.y + size / 2, position.z + size / 2};
    voxel->bbColor = BLACK;
    voxel->color = (Color){coordinates.x*20, coordinates.y*20, coordinates.z*20, 255};
    voxel->isActive = true;
    return voxel;
}

void DrawVoxel(Voxel* voxel) {
    if (voxel == NULL || !voxel->isActive) return;

    
    DrawCube(voxel->position, voxel->size, voxel->size, voxel->size, voxel->color);
    DrawBoundingBox(voxel->bb, voxel->bbColor);
}

void ResetVoxel(Voxel* voxel) {
    if (voxel == NULL) return;
    voxel->bbColor = BLACK;
}

void DestroyVoxel(Voxel* voxel) {
    voxel->isActive = false;
}