/*
*   single poly struct and functions
*/

#include "raylib.h"
#include "raymath.h"
//#include <stdio.h>

struct _Poly{
        int exist;
        Vector3 center;
        Vector3 one;
        Vector3 two;
        Vector3 three;
        Vector3 velocity;
        float size;
        Color polyColor;
        Color polyOutline;
    };
    
// void TestFunc(){
    // printf("\n");
    // printf("test func");
    // printf("\n");
// }

void Destroy_Poly(struct _Poly *polyArr, int i){
        polyArr[i].exist = 0;
    }

// this can be made more efficient with a stack of indices to allocate new spawns
void Spawn_Poly(struct _Poly *polyArr, int arrSize, Vector3 startPos){
    for (int i = 0; i < arrSize; i++){
        if (polyArr[i].exist != 1){
            polyArr[i].exist = 1;
            polyArr[i].center = startPos;
            polyArr[i].one = Vector3Add(startPos, (Vector3){ 0.0f, 0.2f, 0.0f });
            polyArr[i].two = Vector3Add(startPos, (Vector3){ 0.2f, -0.2f, 0.0f });
            polyArr[i].three = Vector3Add(startPos, (Vector3){ -0.2f, -0.2f, 0.0f });
            polyArr[i].velocity = (Vector3){ 0, 0.02f, 0 };
            polyArr[i].size = 0.5f;
            polyArr[i].polyColor = MAGENTA;
            polyArr[i].polyOutline = BLACK;
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
            polyArr[i].center = Vector3Add(polyArr[i].center, polyArr[i].velocity);
            polyArr[i].one = Vector3Add(polyArr[i].center, (Vector3){0,polyArr[i].size,0});
            polyArr[i].two = Vector3Add(polyArr[i].center, (Vector3){polyArr[i].size,-polyArr[i].size,0});
            polyArr[i].three = Vector3Add(polyArr[i].center, (Vector3){-polyArr[i].size,-polyArr[i].size,0});
            
            // Draw
            DrawTriangle3D(polyArr[i].one, polyArr[i].two, polyArr[i].three, polyArr[i].polyColor);
            DrawTriangle3D(polyArr[i].one, polyArr[i].three, polyArr[i].two, polyArr[i].polyColor);
            DrawLine3D(polyArr[i].one, polyArr[i].two, polyArr[i].polyOutline);
            DrawLine3D(polyArr[i].two, polyArr[i].three, polyArr[i].polyOutline);
            DrawLine3D(polyArr[i].three, polyArr[i].one, polyArr[i].polyOutline);
        }
    }
    
}

