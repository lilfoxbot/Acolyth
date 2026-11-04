#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Voxel {
    bool isActive;
    bool isOccupied;

    Vector2 coordinates;
    Vector3 position;
    Vector3 size;
    BoundingBox bb;
    
    Color color;
    Color defaultColor;
    Color bbColor;

    bool selected;
    Vector3 selectedNormal;
    bool fading;
} Voxel;

Voxel* Voxel_Construct(Vector3 position, Vector2 coordinates) {
    Voxel* voxel = (Voxel*)malloc(sizeof(Voxel));
    voxel->position = position;
    voxel->coordinates = coordinates;
    voxel->size = (Vector3){1,1,1};
    voxel->bb.min = Vector3Add(voxel->position, (Vector3){-0.5f,-0.5f,-0.5f});
    voxel->bb.max = Vector3Add(voxel->position, (Vector3){0.5f,0.5f,0.5f});
    voxel->bbColor = BLACK;
    voxel->defaultColor = GRAY;
    voxel->color = voxel->defaultColor;
    voxel->isActive = false;
    voxel->isOccupied = false;
    voxel->selected = false;
    voxel->selectedNormal = (Vector3){0,0,0};
    voxel->fading = false;
    return voxel;
}

void Voxel_Draw(Voxel* voxel) {
    if (voxel == NULL || !voxel->isActive) return;

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

        DrawCube(voxel->position, voxel->size.x, voxel->size.y, voxel->size.z, voxel->color);
    } else {
        DrawCube(voxel->position, voxel->size.x, voxel->size.y, voxel->size.z, voxel->color);
    }
    
    DrawBoundingBox(voxel->bb, voxel->bbColor);
}

void Voxel_Reset(Voxel* voxel) {
    if (voxel == NULL) return;

    voxel->bbColor = BLACK;
    voxel->selected = false;
}

void Voxel_Destroy(Voxel* voxel) {
    voxel->isActive = false;
}

// #GRID

typedef struct VoxelGrid{
    bool isActive;
    bool isWall;

    Vector3 position;
    Vector3 velocity;

    int voxelCount;
    struct Voxel* voxels[100];

} VoxelGrid;

VoxelGrid* VoxelGrid_Construct(int length, int width){
    VoxelGrid* obj = (VoxelGrid*)malloc(sizeof(VoxelGrid));
    obj->isActive = false;
    obj->isWall = false;

    obj->position = (Vector3){0,0,0};
    obj->velocity = (Vector3){0,0,0};

    // create voxels
    obj->voxelCount = 0;
    for (int x = 0; x < length; x++){
        for (int y = 0; y < width; y++){
            Voxel* newVoxel = Voxel_Construct((Vector3){obj->position.x + x, obj->position.y + y, obj->position.z}, (Vector2){x, y});
            newVoxel->isActive = true;
            newVoxel->size.z = 0.5f * x;
            Color newColor = {20 * x, 20 * x, 20 * x, 255};
            newVoxel->color = newColor; 
            obj->voxels[obj->voxelCount] = newVoxel;
            obj->voxelCount++;
        }
    }

    return obj;
}

void VoxelGrid_Update(VoxelGrid* obj, float deltaTime){
    if (!obj->isActive) return;

    obj->velocity = (Vector3){0.01f,0,0};
    obj->position = Vector3Add(obj->position, obj->velocity);
    for (int i = 0; i < obj->voxelCount; i++){
        //Voxel_Update(obj->voxels[i]);
        obj->voxels[i]->position = (Vector3){obj->position.x + obj->voxels[i]->coordinates.x,
                                            obj->position.y + obj->voxels[i]->coordinates.y,
                                            obj->position.z};
        obj->voxels[i]->bb.min = Vector3Add(obj->voxels[i]->position, (Vector3){-0.5f,-0.5f,-0.5f});
        obj->voxels[i]->bb.max = Vector3Add(obj->voxels[i]->position, (Vector3){0.5f,0.5f,0.5f});
    }
}

void VoxelGrid_Draw(VoxelGrid* obj){
    if (!obj->isActive) return;

    DrawLine3D(Vector3Add(obj->position, (Vector3){-1,0,0}), Vector3Add(obj->position, (Vector3){1,0,0}), MAGENTA);
    DrawLine3D(Vector3Add(obj->position, (Vector3){0,-1,0}), Vector3Add(obj->position, (Vector3){0,1,0}), MAGENTA);
    DrawLine3D(Vector3Add(obj->position, (Vector3){0,0,-1}), Vector3Add(obj->position, (Vector3){0,0,1}), MAGENTA);

    for (int i = 0; i < obj->voxelCount; i++){
        Voxel_Draw(obj->voxels[i]);
    }
}