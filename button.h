#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    BTN_NONE,
    BTN_PLAY,
    BTN_TEST,
    BTN_SAVE,
    BTN_LOAD,
    BTN_VOXEL,
    BTN_TURRET
} ButtonFunction;

typedef struct Button{
    bool isActive;

    Rectangle rect;
    char label[20];
    Color color;
    Color outlineColor;
    ButtonFunction btnfunc;

} Button;

void SetArray(char *dest, size_t dest_size, const char *source) {
    snprintf(dest, dest_size, "%s", source);
}

Button* Create_Button(){
    Button* obj = (Button*)malloc(sizeof(Button));
    obj->isActive = false;

    obj->rect.width = 60;
    obj->rect.height = 30;

    obj->color = WHITE;
    obj->outlineColor = BLACK;
    
    return obj;
}

void Spawn_Button(Button* obj, Vector2 newPos, char *label, ButtonFunction btnfunc){
    obj->isActive = true;
    obj->rect.x = newPos.x;
    obj->rect.y = newPos.y;
    SetArray(obj->label, sizeof(obj->label), label);
    obj->btnfunc = btnfunc;
}

void Destroy_Button(Button* obj){
    if (!obj->isActive) return;
    obj->isActive = false;
}

ButtonFunction Update_Button(Button* obj, Vector2 mousePoint){
    if (!obj->isActive) return BTN_NONE;

    if (CheckCollisionPointRec(mousePoint, obj->rect)){
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            obj->color = BLACK;
            obj->outlineColor = WHITE;
        } else {
            obj->color = WHITE;
            obj->outlineColor = BLACK;
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) return obj->btnfunc;
        
    } else {
        obj->color = WHITE;
        obj->outlineColor = BLACK;
    }

    return BTN_NONE;
}

void Draw_Button(Button* obj){
    if (!obj->isActive) return;

    DrawRectangle(obj->rect.x, obj->rect.y, obj->rect.width, obj->rect.height, obj->color);
    DrawRectangleLines(obj->rect.x, obj->rect.y, obj->rect.width, obj->rect.height, obj->outlineColor);
    DrawText(obj->label, obj->rect.x+5, obj->rect.y+obj->rect.height/2-8, 10, BLACK);
}