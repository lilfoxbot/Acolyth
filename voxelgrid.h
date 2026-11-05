#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

#include "tandem_common.h"

typedef struct Voxel {
    bool isActive;
    bool isOccupied;

    Vector2 coordinates;
    Vector3 position;
    Vector3 size;
    float stretchTarget;
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
    voxel->bbColor = WHITE;
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
        // use lerp

        DrawCube(voxel->position, voxel->size.x, voxel->size.y, voxel->size.z, voxel->color);
    } else {
        DrawCube(voxel->position, voxel->size.x, voxel->size.y, voxel->size.z, voxel->color);
    }
    
    DrawBoundingBox(voxel->bb, voxel->bbColor);
}

void Voxel_Reset(Voxel* voxel){
    if (voxel == NULL) return;

    voxel->bbColor = BLACK;
    voxel->selected = false;
}

void Voxel_Destroy(Voxel* voxel){
    voxel->isActive = false;
}

// #GRID

typedef struct VoxelGrid{
    bool isActive;
    bool isHorizontal;

    Vector3 position;
    Vector3 velocity;

    int voxelCount;
    struct Voxel* voxels[200];

} VoxelGrid;

VoxelGrid* VoxelGrid_Construct(int length, int width, bool horizontal){
    VoxelGrid* obj = (VoxelGrid*)malloc(sizeof(VoxelGrid));
    obj->isActive = false;
    obj->isHorizontal = horizontal;

    obj->position = (Vector3){0,0,0};
    obj->velocity = (Vector3){0,0,0};
    obj->voxelCount = 0;
    
    if (obj->isHorizontal){
        for (int x = 0; x < length; x++){
            for (int z = 0; z < width; z++){
                Voxel* newVoxel = Voxel_Construct((Vector3){obj->position.x + x, obj->position.y, obj->position.z + z}, (Vector2){x, z});
                newVoxel->isActive = true;
                Color newColor = {20 * x, 20 * x, 20 * x, 255};
                newVoxel->color = newColor; 
                obj->voxels[obj->voxelCount] = newVoxel;
                obj->voxelCount++;
            }
        }
    } else {
        for (int x = 0; x < length; x++){
            for (int y = 0; y < width; y++){
                Voxel* newVoxel = Voxel_Construct((Vector3){obj->position.x + x, obj->position.y + y, obj->position.z}, (Vector2){x, y});
                newVoxel->isActive = true;
                Color newColor = {20 * x, 20 * x, 20 * x, 255};
                newVoxel->color = newColor; 
                obj->voxels[obj->voxelCount] = newVoxel;
                obj->voxelCount++;
            }
        }
    }

    // create floor voxel grid

    return obj;
}

void VoxelGrid_Update(VoxelGrid* obj, float deltaTime){
    if (!obj->isActive) return;

    //obj->velocity = (Vector3){0.01f,0,0};
    obj->position = Vector3Add(obj->position, obj->velocity);
    for (int i = 0; i < obj->voxelCount; i++){
        
        if (obj->isHorizontal){
            obj->voxels[i]->position = 
            (Vector3){obj->position.x + obj->voxels[i]->coordinates.x,
                    obj->position.y,
                    obj->position.z + obj->voxels[i]->coordinates.y};
        } else {
            obj->voxels[i]->position = 
            (Vector3){obj->position.x + obj->voxels[i]->coordinates.x,
                    obj->position.y + obj->voxels[i]->coordinates.y,
                    obj->position.z};
        }
        
        //obj->voxels[i]->stretchTarget = GetRandomValue(1,3);
        //obj->voxels[i]->size.z = MoveToFloat(obj->voxels[i]->size.z, obj->voxels[i]->stretchTarget, 0.01f);

        obj->voxels[i]->bb.min = Vector3Add(obj->voxels[i]->position,
            (Vector3){-obj->voxels[i]->size.x/2, -obj->voxels[i]->size.y/2, -obj->voxels[i]->size.z/2});
        obj->voxels[i]->bb.max = Vector3Add(obj->voxels[i]->position,
            (Vector3){obj->voxels[i]->size.x/2, obj->voxels[i]->size.y/2, obj->voxels[i]->size.z/2});
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