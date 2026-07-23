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

    Rectangle body;
    Color bodyColor;
    Color bodyOutlineColor;
    
    Rectangle titleBar;
    Color titleBarColor;
    Color titleBarOutlineColor;

    char title[30];
    int titleFontSize;
    int fontSize;

} Window;

static void SetTextArray(char *dest, size_t dest_size, const char *source){
    snprintf(dest, dest_size, "%s", source);
}

Window* Create_Window(){
    Window* obj = (Window*)malloc(sizeof(Window));
    obj->isActive = false;
    obj->dragging = false;
    obj->dragPoint = (Vector2){0,0};

    obj->body.width = 60;
    obj->body.height = 30;
    obj->titleFontSize = 8;
    obj->fontSize = 8;

    obj->titleBarColor = LIGHTGRAY;
    obj->titleBarOutlineColor = BLACK;
    obj->bodyColor = DARKGRAY;
    obj->bodyOutlineColor = BLACK;
    
    return obj;
}

void Spawn_Window(Window* obj, Vector2 pos, Vector2 size, char *title){
    obj->isActive = true;
    obj->body.x = pos.x;
    obj->body.y = pos.y;
    obj->body.width = size.x;
    obj->body.height = size.y;

    obj->titleBar.width = obj->body.width;
    obj->titleBar.height = 20;

    SetTextArray(obj->title, sizeof(obj->title), title);
}

void Destroy_Window(Window* obj){
    if (!obj->isActive) return;
    obj->isActive = false;
}

void Update_Window(Window* obj, Vector2 mousePoint){
    if (!obj->isActive) return;

    if (CheckCollisionPointRec(mousePoint, obj->titleBar)){
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            obj->dragging = true;
            obj->dragPoint = mousePoint;
        }

        // if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
        //     obj->bodyColor = BLACK;
        //     obj->bodyOutlineColor = WHITE;
        // } else {
        //     obj->bodyColor = WHITE;
        //     obj->bodyOutlineColor = BLACK;
        // }   
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
        obj->dragging = false;
    }

    if (obj->dragging){
        Vector2 tempVec = Vector2Subtract(mousePoint, obj->dragPoint);
        tempVec = Vector2Add((Vector2){obj->body.x,obj->body.y}, tempVec);
        obj->body.x = tempVec.x;
        obj->body.y = tempVec.y;
        obj->dragPoint = mousePoint;
    }

    obj->titleBar.x = obj->body.x;
    obj->titleBar.y = obj->body.y;
}

void Draw_Window(Window* obj){
    if (!obj->isActive) return;

    // body
    DrawRectangle(obj->body.x, obj->body.y, obj->body.width, obj->body.height, obj->bodyColor);
    DrawRectangleLines(obj->body.x, obj->body.y, obj->body.width, obj->body.height, obj->bodyOutlineColor);
    
    // title
    DrawRectangle(obj->titleBar.x, obj->titleBar.y, obj->titleBar.width, obj->titleBar.height, obj->titleBarColor);
    DrawRectangleLines(obj->titleBar.x, obj->titleBar.y, obj->titleBar.width, obj->titleBar.height, obj->bodyOutlineColor);
    DrawText(obj->title, obj->titleBar.x + 4, obj->titleBar.y + obj->titleBar.height/2, obj->titleFontSize, BLACK);

    // debug
    
}