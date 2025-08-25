/*
*   single poly struct and functions
*/

#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

struct _Poly{
    int exist;
    float lifetime;
    float endtime;
    Vector3 center;
    Vector3 one;
    Vector3 two;
    Vector3 three;
    Vector3 velocity;
    float size;
    Color polyColor;
    Color polyOutline;
};

Vector3 PGetRandomVector(int range, float multiplier){
    return (Vector3){ GetRandomValue(-range,range)*(multiplier),
    GetRandomValue(-range,range)*(multiplier), 
    GetRandomValue(-range,range)*(multiplier)};
}
    
// void TestFunc(){
    // printf("\n");
    // printf("test func");
    // printf("\n");
// }

void Destroy_Poly(struct _Poly *polyArr, int i){
    polyArr[i].exist = 0;
}


void Spawn_Poly(struct _Poly *polyArr, int arrSize, Vector3 startPos){
    for (int i = 0; i < arrSize; i++){
        if (polyArr[i].exist != 1){
            polyArr[i].exist = 1;
            polyArr[i].lifetime = 0;
            polyArr[i].endtime = GetRandomValue(3,6);
            polyArr[i].center = startPos;
            polyArr[i].size = 0.2f;
            polyArr[i].one = PGetRandomVector(3,0.1f);
            polyArr[i].two = PGetRandomVector(3,0.1f);
            polyArr[i].three = PGetRandomVector(3,0.1f);
            polyArr[i].velocity = PGetRandomVector(5,0.01f);
            polyArr[i].polyColor = RED;
            polyArr[i].polyOutline = WHITE;
            return;
        }
    }
}

// collide, update, draw
void Poly_CUD(struct _Poly *polyArr, int arrSize){
    //int arrSize = sizeof(myArray) / sizeof(myArray[0]);
    for (int i = 0; i < arrSize; i++){
        if (polyArr[i].exist == 1){
            // Collide
            
            // Update
            polyArr[i].lifetime += GetFrameTime();
            if (polyArr[i].lifetime > polyArr[i].endtime){
                polyArr[i].exist = 0;
            }
            polyArr[i].center = Vector3Add(polyArr[i].center, polyArr[i].velocity);
            // move towards sky
            
            polyArr[i].center = Vector3Add(polyArr[i].center, (Vector3){0,polyArr[i].lifetime*(0.2f),0});
            
            // jitter
            polyArr[i].one = Vector3Add(polyArr[i].one, PGetRandomVector(5,0.005f));
            polyArr[i].two = Vector3Add(polyArr[i].two, PGetRandomVector(5,0.005f));
            polyArr[i].three = Vector3Add(polyArr[i].three, PGetRandomVector(5,0.005f));
            // shrink
            polyArr[i].one = Vector3Scale(polyArr[i].one, 0.95f);
            polyArr[i].two = Vector3Scale(polyArr[i].two, 0.95f);
            polyArr[i].three = Vector3Scale(polyArr[i].three, 0.95f);
            
            Vector3 newOne = Vector3Add(polyArr[i].center, polyArr[i].one);
            Vector3 newTwo = Vector3Add(polyArr[i].center, polyArr[i].two);
            Vector3 newThree = Vector3Add(polyArr[i].center, polyArr[i].three);
            
            // Draw
            DrawTriangle3D(newOne, newTwo, newThree, polyArr[i].polyColor);
            DrawTriangle3D(newOne, newThree, newTwo, polyArr[i].polyColor);
            DrawLine3D(newOne, newTwo, polyArr[i].polyOutline);
            DrawLine3D(newTwo, newThree, polyArr[i].polyOutline);
            DrawLine3D(newThree, newOne, polyArr[i].polyOutline);
        }
    }   
}



