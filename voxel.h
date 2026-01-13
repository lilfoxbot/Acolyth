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
    Color defaultColor;
    bool isActive;
    bool selected;
    Vector3 selectedNormal;
} Voxel;

Voxel* CreateVoxel(Vector3 position, Vector3 coordinates, float size) {
    Voxel* voxel = (Voxel*)malloc(sizeof(Voxel));
    voxel->position = position;
    voxel->coordinates = coordinates;
    voxel->size = size;
    voxel->bb.min = (Vector3){position.x - size / 2, position.y - size / 2, position.z - size / 2};
    voxel->bb.max = (Vector3){position.x + size / 2, position.y + size / 2, position.z + size / 2};
    voxel->bbColor = BLACK;
    voxel->defaultColor = (Color){coordinates.x*20, coordinates.y*20, coordinates.z*20, 255};
    voxel->color = voxel->defaultColor;
    voxel->isActive = true;
    voxel->selected = false;
    voxel->selectedNormal = (Vector3){0,0,0};
    return voxel;
}

void DrawVoxel(Voxel* voxel) {
    if (voxel == NULL || !voxel->isActive) return;

    if (voxel->selected) {
        float cursorSizeX = 0.9f;
        float cursorSizeY = 0.9f;
        float cursorSizeZ = 0.9f;

        if (voxel->selectedNormal.x < 0.9f && voxel->selectedNormal.x > -0.9f) cursorSizeX = 0;
        if (voxel->selectedNormal.y < 0.9f && voxel->selectedNormal.y > -0.9f) cursorSizeY = 0;
        if (voxel->selectedNormal.z < 0.9f && voxel->selectedNormal.z > -0.9f) cursorSizeZ = 0;
        
        DrawCubeWires(Vector3Add(voxel->position,Vector3Scale(voxel->selectedNormal,0.55)), cursorSizeX, cursorSizeY, cursorSizeZ, RAYWHITE);
        DrawCube(voxel->position, voxel->size, voxel->size, voxel->size, (Color){voxel->color.r+50,voxel->color.g+50,voxel->color.b+50,255});
    } else {
        DrawCube(voxel->position, voxel->size, voxel->size, voxel->size, voxel->color);
    }
    DrawBoundingBox(voxel->bb, voxel->bbColor);
}

void ResetVoxel(Voxel* voxel) {
    if (voxel == NULL) return;
    voxel->color = voxel->defaultColor;
    voxel->selected = false;
}

void DestroyVoxel(Voxel* voxel) {
    voxel->isActive = false;
}