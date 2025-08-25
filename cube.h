/*
*   single cube struct and functions
*/

#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

struct _Cube{
    int exist;
    float lifetime;
    float speed;
    Vector3 position;
    Vector3 size;
    struct BoundingBox bb;
    struct RayCollision col;
};

Vector2 cubeSpawnX = { -50, 50 };
Vector2 cubeSpawnY = { 0, 50 };
Vector2 cubeSpawnZ = { -100, -50 };
float spawnMod = 0.1f;
float cubeSpawn = 1.0f;

int Spawn_Cube(struct _Cube *cubeArr, int arrSize){
    // allocate index for new cube
    for (int i = 0; i < arrSize; i++){
        if (cubeArr[i].exist != 1){
            cubeArr[i].exist = 1;
            cubeArr[i].lifetime = 0.0f;
            cubeArr[i].speed = 0.1f;
            cubeArr[i].position = (Vector3){(float)GetRandomValue(cubeSpawnX.x, cubeSpawnX.y)*spawnMod, 
                    (float)GetRandomValue(cubeSpawnY.x, cubeSpawnY.y)*spawnMod, 
                    (float)GetRandomValue(cubeSpawnZ.x, cubeSpawnZ.y)*spawnMod};
            cubeArr[i].size = (Vector3){0.5f,0.5f,0.5f};
            cubeArr[i].bb.min = Vector3Add(cubeArr[i].position, (Vector3){-(cubeArr[i].size.x/2),-(cubeArr[i].size.y/2),-(cubeArr[i].size.z/2)});
            cubeArr[i].bb.max = Vector3Add(cubeArr[i].position, (Vector3){(cubeArr[i].size.x/2),(cubeArr[i].size.y/2),(cubeArr[i].size.z/2)});
            
            return i;
        }
    }
    return -1;
}

void Destroy_Cube(struct _Cube *cubeArr, int cubeIndex){
    cubeArr[cubeIndex].exist = 0;
    // for (int j = 0; j < 11; j++){
    //     Spawn_Poly(polys, POLY_LIMIT, Vector3Add(cubeArr[cubeIndex].position, GetRandomVector(3,0.1)));
    // }
}

void Update_Cube(struct _Cube *cubeArr, int i, float dt, Vector3 addPos){
    if (cubeArr[i].exist == 1){ } else { return; }
    
    cubeArr[i].position = (Vector3){cubeArr[i].position.x + addPos.x,
                                    cubeArr[i].position.y + addPos.y,
                                    cubeArr[i].position.z + addPos.z};
    cubeArr[i].bb.min = Vector3Add(cubeArr[i].position, (Vector3){-(cubeArr[i].size.x/2),-(cubeArr[i].size.y/2),-(cubeArr[i].size.z/2)});
    cubeArr[i].bb.max = Vector3Add(cubeArr[i].position, (Vector3){(cubeArr[i].size.x/2),(cubeArr[i].size.y/2),(cubeArr[i].size.z/2)});
    cubeArr[i].lifetime += dt;
}

void Check_Cube(struct _Cube *cubeArr, int cubeIndex, Ray cubeRay){
    if (cubeArr[cubeIndex].exist == 1){ } else { return; }
    cubeArr[cubeIndex].col = GetRayCollisionBox(cubeRay, cubeArr[cubeIndex].bb);

    if (cubeArr[cubeIndex].col.hit && cubeArr[cubeIndex].exist){
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            Destroy_Cube(cubeArr, cubeIndex);
        }
    }
}

void Draw_Cube(struct _Cube *cubeArr, int i){
    if (cubeArr[i].exist == 1){ } else { return; }
    Color cubeColor;
    if (cubeArr[i].lifetime <= 0.2f){ cubeColor = WHITE;}
    else {
        cubeColor = (cubeArr[i].col.hit) ? RED : BLUE;
    }
    
    DrawCubeV(cubeArr[i].position, cubeArr[i].size, cubeColor);
    DrawBoundingBox(cubeArr[i].bb, WHITE);
}
