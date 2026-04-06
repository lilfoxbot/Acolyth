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
    bool fading;
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
    voxel->fading = false;
    return voxel;
}

void DrawVoxel(Voxel* voxel) {
    if (voxel == NULL || !voxel->isActive) return;

    if (voxel->selected) {
        float cursorSizeX = 0.8f;
        float cursorSizeY = 0.8f;
        float cursorSizeZ = 0.8f;
        
        if (voxel->selectedNormal.x == 1 || voxel->selectedNormal.x == -1){ cursorSizeX = 0; }
        if (voxel->selectedNormal.y == 1 || voxel->selectedNormal.y == -1){ cursorSizeY = 0; }
        if (voxel->selectedNormal.z == 1 || voxel->selectedNormal.z == -1){ cursorSizeZ = 0; }

        // "edge" cases
        if (voxel->selectedNormal.x < 1 && voxel->selectedNormal.x > -1 && voxel->selectedNormal.x != 0){ cursorSizeX = 0; }
        if (voxel->selectedNormal.y < 1 && voxel->selectedNormal.y > -1 && voxel->selectedNormal.y != 0){ cursorSizeY = 0; }
        if (voxel->selectedNormal.z < 1 && voxel->selectedNormal.z > -1 && voxel->selectedNormal.z != 0){ cursorSizeZ = 0; }
        
        DrawCubeWires(Vector3Add(voxel->position,Vector3Scale(voxel->selectedNormal,0.55)), cursorSizeX, cursorSizeY, cursorSizeZ, RAYWHITE);
        DrawCube(voxel->position, voxel->size, voxel->size, voxel->size, (Color){voxel->defaultColor.r+50,voxel->defaultColor.g+50,voxel->defaultColor.b+50,255});
    } else {
        if (voxel->fading){
            // update fade
            int newR = voxel->color.r;
            int newG = voxel->color.g;
            int newB = voxel->color.b;

            if (voxel->color.r <= voxel->defaultColor.r && 
                voxel->color.g <= voxel->defaultColor.g &&
                voxel->color.b <= voxel->defaultColor.b)
            {
                voxel->color = voxel->defaultColor;
                voxel->fading = false;
            } else {
                if (voxel->color.r > voxel->defaultColor.r) newR--;
                if (voxel->color.g > voxel->defaultColor.g) newG--;
                if (voxel->color.b > voxel->defaultColor.b) newB--;

                voxel->color = (Color){newR,newG,newB,voxel->color.a};
            }

            DrawCube(voxel->position, voxel->size, voxel->size, voxel->size, voxel->color);
        } else {
            DrawCube(voxel->position, voxel->size, voxel->size, voxel->size, voxel->color);
        }
    }

    DrawBoundingBox(voxel->bb, voxel->bbColor);
}

void ResetVoxel(Voxel* voxel) {
    if (voxel == NULL) return;
    //voxel->color = voxel->defaultColor;
    voxel->selected = false;
}

void DestroyVoxel(Voxel* voxel) {
    voxel->isActive = false;
}