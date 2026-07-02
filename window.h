#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Window{
    bool isActive;
    bool dragging;
    Vector2 dragPoint;

    Rectangle rect;
    char title[30];
    int fontSize;
    Color color;
    Color outlineColor;

} Window;

static void SetTextArray(char *dest, size_t dest_size, const char *source){
    snprintf(dest, dest_size, "%s", source);
}

Window* Create_Window(){
    Window* obj = (Window*)malloc(sizeof(Window));
    obj->isActive = false;
    obj->dragging = false;
    obj->dragPoint = (Vector2){0,0};

    obj->rect.width = 60;
    obj->rect.height = 30;
    obj->fontSize = 10;

    obj->color = WHITE;
    obj->outlineColor = BLACK;
    
    return obj;
}

void Spawn_Window(Window* obj, Vector2 pos, Vector2 size, char *title, int fontSize){
    obj->isActive = true;
    obj->rect.x = pos.x;
    obj->rect.y = pos.y;
    obj->rect.width = size.x;
    obj->rect.height = size.y;
    obj->fontSize = fontSize;

    SetTextArray(obj->title, sizeof(obj->title), title);
}

void Destroy_Window(Window* obj){
    if (!obj->isActive) return;
    obj->isActive = false;
}

void Update_Window(Window* obj, Vector2 mousePoint){
    if (!obj->isActive) return;

    if (CheckCollisionPointRec(mousePoint, obj->rect)){
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            obj->dragging = true;
            obj->dragPoint = mousePoint;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            obj->color = BLACK;
            obj->outlineColor = WHITE;
        } else {
            obj->color = WHITE;
            obj->outlineColor = BLACK;
        }
        
    } else {
        obj->color = WHITE;
        obj->outlineColor = BLACK;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
        obj->dragging = false;
    }

    if (obj->dragging){
        Vector2 tempVec = Vector2Subtract(mousePoint, obj->dragPoint);
        tempVec = Vector2Add((Vector2){obj->rect.x,obj->rect.y}, tempVec);
        obj->rect.x = tempVec.x;
        obj->rect.y = tempVec.y;
        obj->dragPoint = mousePoint;
    }
}

void Draw_Window(Window* obj){
    if (!obj->isActive) return;

    DrawRectangle(obj->rect.x, obj->rect.y, obj->rect.width, obj->rect.height, obj->color);
    DrawRectangleLines(obj->rect.x, obj->rect.y, obj->rect.width, obj->rect.height, obj->outlineColor);
    DrawText(obj->title, obj->rect.x+5, obj->rect.y+obj->rect.height/2-6, obj->fontSize, BLACK);
}