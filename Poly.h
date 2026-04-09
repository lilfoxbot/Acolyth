/*
*   single poly struct and functions
*/

#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Poly{
    bool isActive;

    Vector3 position;
    Vector3 direction;
    float speed;
    Vector3 velocity;

    float lifetime;
    float endtime;

    Vector3 one;
    Vector3 two;
    Vector3 three;
    float size;
    Color polyColor;
    Color polyOutlineColor;
} Poly;

Vector3 PGetRandomVector(int range, float multiplier){
    return (Vector3){ GetRandomValue(-range,range)*(multiplier),
    GetRandomValue(-range,range)*(multiplier), 
    GetRandomValue(-range,range)*(multiplier)};
}

Poly* CreatePoly(){
    Poly* poly = (Poly*)malloc(sizeof(Poly));
    poly->isActive = false;

    poly->position = (Vector3){0,0,0};
    poly->direction = (Vector3){0,0,0};
    poly->speed = 0;
    poly->velocity = (Vector3){0,0,0};

    poly->lifetime = 0;
    poly->endtime = 3;

    poly->one = (Vector3){0,0,0};
    poly->two = (Vector3){0,0,0};
    poly->three = (Vector3){0,0,0};
    poly->size = 0;
    poly->polyColor = YELLOW;
    poly->polyOutlineColor = WHITE;

    return poly;
}

// collide, update, draw
// void Poly_CUD(struct _Poly *polyArr, int arrSize){
//     //int arrSize = sizeof(myArray) / sizeof(myArray[0]);
//     for (int i = 0; i < arrSize; i++){
//         if (polyArr[i].exist == 1){
//             // Collide
            
//             // Update
//             polyArr[i].lifetime += GetFrameTime();
//             if (polyArr[i].lifetime > polyArr[i].endtime){
//                 polyArr[i].exist = 0;
//             }
//             polyArr[i].center = Vector3Add(polyArr[i].center, polyArr[i].velocity);
//             // move towards sky
            
//             polyArr[i].center = Vector3Add(polyArr[i].center, (Vector3){0,polyArr[i].lifetime*(0.2f),0});
            
//             // jitter
//             polyArr[i].one = Vector3Add(polyArr[i].one, PGetRandomVector(5,0.005f));
//             polyArr[i].two = Vector3Add(polyArr[i].two, PGetRandomVector(5,0.005f));
//             polyArr[i].three = Vector3Add(polyArr[i].three, PGetRandomVector(5,0.005f));
//             // shrink
//             polyArr[i].one = Vector3Scale(polyArr[i].one, 0.95f);
//             polyArr[i].two = Vector3Scale(polyArr[i].two, 0.95f);
//             polyArr[i].three = Vector3Scale(polyArr[i].three, 0.95f);
            
//             Vector3 newOne = Vector3Add(polyArr[i].center, polyArr[i].one);
//             Vector3 newTwo = Vector3Add(polyArr[i].center, polyArr[i].two);
//             Vector3 newThree = Vector3Add(polyArr[i].center, polyArr[i].three);
            
//             // Draw
//             DrawTriangle3D(newOne, newTwo, newThree, polyArr[i].polyColor);
//             DrawTriangle3D(newOne, newThree, newTwo, polyArr[i].polyColor);
//             DrawLine3D(newOne, newTwo, polyArr[i].polyOutline);
//             DrawLine3D(newTwo, newThree, polyArr[i].polyOutline);
//             DrawLine3D(newThree, newOne, polyArr[i].polyOutline);
//         }
//     }   
// }