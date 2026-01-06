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
    Color color;
    bool isActive;
} Voxel;

Voxel* CreateVoxel(Vector3 position, float size, Color color) {
    Voxel* voxel = (Voxel*)malloc(sizeof(Voxel));
    voxel->position = position;
    voxel->size = size;
    voxel->bb.min = (Vector3){position.x - size / 2, position.y - size / 2, position.z - size / 2};
    voxel->bb.max = (Vector3){position.x + size / 2, position.y + size / 2, position.z + size / 2};
    voxel->color = color;
    voxel->isActive = true;
    return voxel;
}

void DrawVoxel(Voxel* voxel) {
    if (voxel == NULL || !voxel->isActive) return;

    //DrawCube(voxel->position, voxel->size, voxel->size, voxel->size, voxel->color);
    //DrawCubeWires(voxel->position, voxel->size, voxel->size, voxel->size, BLACK);
    DrawBoundingBox(voxel->bb, voxel->color);
}

void DestroyVoxel(Voxel* voxel) {
    voxel->isActive = false;
}