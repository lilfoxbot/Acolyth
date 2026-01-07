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

Voxel* CreateVoxel(Vector3 position, float size) {
    Voxel* voxel = (Voxel*)malloc(sizeof(Voxel));
    voxel->position = position;
    voxel->size = size;
    voxel->bb.min = (Vector3){position.x - size / 2, position.y - size / 2, position.z - size / 2};
    voxel->bb.max = (Vector3){position.x + size / 2, position.y + size / 2, position.z + size / 2};
    voxel->bbColor = BLACK;
    voxel->color = DARKGREEN;
    voxel->isActive = true;
    return voxel;
}

void DrawVoxel(Voxel* voxel) {
    if (voxel == NULL || !voxel->isActive) return;

    DrawCube(voxel->position, voxel->size, voxel->size, voxel->size, voxel->color);
    DrawBoundingBox(voxel->bb, voxel->bbColor);
}

void DestroyVoxel(Voxel* voxel) {
    voxel->isActive = false;
}