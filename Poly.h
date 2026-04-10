/*
*   single poly struct and functions
*/

#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int RAND_RANGE = 5;
const float RAND_MULTI = 0.001f;
const float SHRINK_RATE = 0.99f;

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

    Vector3 onePos;
    Vector3 twoPos;
    Vector3 threePos;

    float size;
    Color polyColor;
    Color polyOutlineColor;
} Poly;

Vector3 GetRandomVector(int range, float multiplier){
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

    poly->size = 0.1f;
    poly->polyColor = BLACK;
    poly->polyOutlineColor = WHITE;

    return poly;
}

void SpawnPoly(Poly* poly, Vector3 newPos){
    poly->isActive = true;

    poly->position = newPos;
    poly->lifetime = 0;
    poly->velocity = (Vector3){GetRandomValue(-3,3)*(0.005f), GetRandomValue(-3,3)*(0.005f), GetRandomValue(-3,3)*(0.005f)};

    poly->one = (Vector3){GetRandomValue(-5,5)*(0.01),0,0};
    poly->two = (Vector3){GetRandomValue(-5,5)*(0.01),0,0};
    poly->three = (Vector3){0,GetRandomValue(-5,5)*(0.01),0};

    poly->onePos = Vector3Add(poly->position, poly->one);
    poly->twoPos = Vector3Add(poly->position, poly->two);
    poly->threePos = Vector3Add(poly->position, poly->three);
}

void DestroyPoly(Poly* poly){
    if (!poly->isActive) return;

    poly->isActive = false;
}

void UpdatePoly(Poly* poly, float deltaTime){
    if (!poly->isActive) return;

    poly->lifetime += deltaTime;
    if (poly->lifetime > poly->endtime) poly->isActive = false;

    // TODO: add vertical speed
    poly->velocity = Vector3Add(poly->velocity, (Vector3){0,0.01f*deltaTime,0});
    poly->position = Vector3Add(poly->position, poly->velocity);

    // jitter
    poly->one = Vector3Add(poly->one, GetRandomVector(RAND_RANGE,RAND_MULTI));
    poly->two = Vector3Add(poly->two, GetRandomVector(RAND_RANGE,RAND_MULTI));
    poly->three = Vector3Add(poly->three, GetRandomVector(RAND_RANGE,RAND_MULTI));

    // shrink
    poly->one = Vector3Scale(poly->one, SHRINK_RATE);
    poly->two = Vector3Scale(poly->two, SHRINK_RATE);
    poly->three = Vector3Scale(poly->three, SHRINK_RATE);

    // new positions
    poly->onePos = Vector3Add(poly->position, poly->one);
    poly->twoPos = Vector3Add(poly->position, poly->two);
    poly->threePos = Vector3Add(poly->position, poly->three);
    
}

void Draw_Poly(Poly* poly){
    if (!poly->isActive) return;

    DrawTriangle3D(poly->onePos, poly->twoPos, poly->threePos, poly->polyColor);
    DrawTriangle3D(poly->onePos, poly->threePos, poly->twoPos, poly->polyColor);
    DrawLine3D(poly->onePos, poly->twoPos, poly->polyOutlineColor);
    DrawLine3D(poly->twoPos, poly->threePos, poly->polyOutlineColor);
    DrawLine3D(poly->threePos, poly->onePos, poly->polyOutlineColor);
}