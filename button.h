#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Vector2 btn_edit_origin = (Vector2){200, 10};
Vector2 btn_edit_offset = (Vector2){80, 60};

typedef enum {
    BTN_NONE,
    BTN_MAIN,
    BTN_PLAY,
    BTN_TEST,
    BTN_SAVE,
    BTN_LOAD,
    BTN_PREV,
    BTN_NEXT,
    BTN_VOXEL,
    BTN_TURRET
} ButtonFunction;

typedef struct Button{
    bool isActive;

    Rectangle rect;
    char label[20];
    int fontSize;
    Color defaultColorUP;
    Color defaultColorDOWN;
    Color color;
    Color colorTL;
    Color colorBR;

    ButtonFunction btnfunc;

} Button;

static void SetBtnTextArray(char *dest, size_t dest_size, const char *source) {
    snprintf(dest, dest_size, "%s", source);
}

Button* Create_Button(Vector2 pos, Vector2 size, char *label, ButtonFunction btnfunc){
    Button* obj = (Button*)malloc(sizeof(Button));
    obj->isActive = false;

    obj->rect.width = 60;
    obj->rect.height = 30;
    obj->fontSize = 10;

    obj->defaultColorUP = LIGHTGRAY;
    obj->defaultColorDOWN = DARKGRAY;
    obj->colorTL = WHITE;
    obj->colorBR = BLACK;

    obj->isActive = true;
    obj->rect.x = pos.x;
    obj->rect.y = pos.y;
    obj->rect.width = size.x;
    obj->rect.height = size.y;

    SetBtnTextArray(obj->label, sizeof(obj->label), label);
    obj->btnfunc = btnfunc;
    
    return obj;
}

void Destroy_Button(Button* obj){
    if (!obj->isActive) return;
    obj->isActive = false;
}

ButtonFunction Update_Button(Button* obj, Vector2 mousePoint){
    if (obj == NULL) return BTN_NONE;
    if (!obj->isActive) return BTN_NONE;

    if (CheckCollisionPointRec(mousePoint, obj->rect)){
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            obj->colorTL = BLACK;
            obj->colorBR = WHITE;
            obj->color = obj->defaultColorDOWN;
        } else {
            obj->colorTL = WHITE;
            obj->colorBR = BLACK;
            obj->color = obj->defaultColorUP;
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) return obj->btnfunc;
        
    } else {
        obj->colorTL = WHITE;
        obj->colorBR = BLACK;
        obj->color = obj->defaultColorUP;
    }

    return BTN_NONE;
}

void Draw_Button(Button* obj){
    if (obj == NULL) return;
    if (!obj->isActive) return;

    DrawRectangle(obj->rect.x, obj->rect.y, obj->rect.width, obj->rect.height, obj->color);
    DrawLine(obj->rect.x, obj->rect.y, obj->rect.x + obj->rect.width, obj->rect.y, obj->colorTL); // TOP
    DrawLine(obj->rect.x, obj->rect.y, obj->rect.x, obj->rect.y + obj->rect.height, obj->colorTL); // LEFT
    DrawLine(obj->rect.x, obj->rect.y + obj->rect.height, obj->rect.x + obj->rect.width, obj->rect.y + obj->rect.height, obj->colorBR); // BOT
    DrawLine(obj->rect.x + obj->rect.width, obj->rect.y, obj->rect.x + obj->rect.width, obj->rect.y + obj->rect.height, obj->colorBR); // RIGHT

    DrawText(obj->label, obj->rect.x+5, obj->rect.y+obj->rect.height/2-6, obj->fontSize, BLACK);
}