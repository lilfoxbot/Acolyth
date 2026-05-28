#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Button{
    bool isActive;

    Rectangle rect;
    char btnText[32];

    Color color;
} Button;

Button* Create_Button(){
    Button* obj = (Button*)malloc(sizeof(Button));
    obj->isActive = false;

    obj->rect.width = 60;
    obj->rect.height = 30;

    obj->color = WHITE;

    return obj;
}

void Spawn_Button(Button* obj, Vector2 newPos){
    obj->isActive = true;
    obj->rect.x = newPos.x;
    obj->rect.y = newPos.y;
}

void Destroy_Button(Button* obj){
    if (!obj->isActive) return;
    obj->isActive = false;
}

void Update_Button(Button* obj, Vector2 mousePoint){
    if (!obj->isActive) return;

    if (CheckCollisionPointRec(mousePoint, obj->rect)){
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            obj->color = BLACK;
        } else {
            obj->color = WHITE;
        }
        //if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) btnAction = true;
        
    } else {
        obj->color = WHITE;
    }
}

void Draw_Button(Button* obj){
    if (!obj->isActive) return;

    DrawRectangle(obj->rect.x, obj->rect.y, obj->rect.width, obj->rect.height, obj->color);
    DrawText(TextFormat("Btn"), obj->rect.x+obj->rect.width/2-8, obj->rect.y+obj->rect.height/2-8, 10, BLACK);
}