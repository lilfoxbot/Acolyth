#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Textbox{
    bool isActive;

    Rectangle rect;
    char label[20];
    int fontSize;
    Color color;
    Color outlineColor;

} Textbox;

void SetArray(char *dest, size_t dest_size, const char *source) {
    snprintf(dest, dest_size, "%s", source);
}

Textbox* Create_Textbox(){
    Textbox* obj = (Textbox*)malloc(sizeof(Textbox));
    obj->isActive = false;

    obj->rect.width = 60;
    obj->rect.height = 30;
    obj->fontSize = 10;

    obj->color = WHITE;
    obj->outlineColor = BLACK;
    
    return obj;
}

void Spawn_Textbox(Textbox* obj, Vector2 pos, Vector2 size, char *label, int fontSize){
    obj->isActive = true;
    obj->rect.x = pos.x;
    obj->rect.y = pos.y;
    obj->rect.width = size.x;
    obj->rect.height = size.y;
    obj->fontSize = fontSize;

    SetArray(obj->label, sizeof(obj->label), label);
}

void Destroy_Textbox(Textbox* obj){
    if (!obj->isActive) return;
    obj->isActive = false;
}

void Update_Textbox(Textbox* obj, Vector2 mousePoint){
    if (!obj->isActive) return;

}

void Draw_Textbox(Textbox* obj){
    if (!obj->isActive) return;

    DrawRectangle(obj->rect.x, obj->rect.y, obj->rect.width, obj->rect.height, obj->color);
    DrawRectangleLines(obj->rect.x, obj->rect.y, obj->rect.width, obj->rect.height, obj->outlineColor);
    DrawText(obj->label, obj->rect.x+5, obj->rect.y+obj->rect.height/2-8, obj->fontSize, BLACK);
}