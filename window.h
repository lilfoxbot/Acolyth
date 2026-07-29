#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "button.h"

typedef struct Window{
    bool isActive;
    bool isFocused;
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

    Button* buttons[4];
    int buttonCount;

    char debugString[50];
} Window;

static void SetText(char *dest, size_t dest_size, const char *source){
    snprintf(dest, dest_size, "%s", source);
}

void UpdateDebugString(Window* obj){
    SetText(obj->debugString, sizeof(obj->debugString),
    TextFormat("Position: %0.2f _ %0.2f", obj->body.x, obj->body.y));
}

Window* Create_Window(){
    Window* obj = (Window*)malloc(sizeof(Window));
    obj->isActive = false;
    obj->isFocused = false;
    obj->dragging = false;
    obj->dragPoint = (Vector2){0,0};

    obj->body.width = 60;
    obj->body.height = 30;
    obj->titleFontSize = 8;
    obj->fontSize = 8;

    obj->titleBarColor = LIGHTGRAY;
    obj->titleBarOutlineColor = BLACK;
    obj->bodyColor = DARKGRAY;
    obj->bodyOutlineColor = GRAY;

    obj->buttonCount = 0;
    
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

    obj->titleBar.x = obj->body.x;
    obj->titleBar.y = obj->body.y;

    SetText(obj->title, sizeof(obj->title), title);
}

void Destroy_Window(Window* obj){
    if (!obj->isActive) return;
    obj->isActive = false;
}

ButtonFunction Update_Window(Window* obj, Vector2 mousePoint){
    if (obj == NULL) return BTN_NONE;
    if (!obj->isActive) return BTN_NONE;

    if (CheckCollisionPointRec(mousePoint, obj->titleBar)){
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            obj->dragging = true;
            obj->dragPoint = mousePoint;
        } 
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

    // update titlebar pos
    obj->titleBar.x = obj->body.x;
    obj->titleBar.y = obj->body.y;

    // update buttons
    ButtonFunction savedBtnFunc;
    for (int i = 0; i < obj->buttonCount; i++){
        ButtonFunction btnFunc;
        btnFunc = Update_Button(obj->buttons[i], mousePoint);
        if (btnFunc != BTN_NONE){ savedBtnFunc = btnFunc; }
        // update button positions
        obj->buttons[i]->rect.x = obj->body.x + 30;
        obj->buttons[i]->rect.y = obj->body.y + 30 + (i*40);
    }

    UpdateDebugString(obj);
    return savedBtnFunc;
}

void Draw_Window(Window* obj){
    if (obj == NULL) return;
    if (!obj->isActive) return;

    // body
    DrawRectangle(obj->body.x, obj->body.y, obj->body.width, obj->body.height, obj->bodyColor);
    DrawRectangleLines(obj->body.x, obj->body.y, obj->body.width, obj->body.height, obj->bodyOutlineColor);
    
    // title
    DrawRectangle(obj->titleBar.x, obj->titleBar.y, obj->titleBar.width, obj->titleBar.height, obj->titleBarColor);
    DrawRectangleLines(obj->titleBar.x, obj->titleBar.y, obj->titleBar.width, obj->titleBar.height, obj->bodyOutlineColor);
    DrawText(obj->title, obj->titleBar.x + 4, obj->titleBar.y + obj->titleBar.height/2 - 2, obj->titleFontSize, BLACK);

    // buttons
    for (int i = 0; i < obj->buttonCount; i++){
        Draw_Button(obj->buttons[i]);
    }

    // debug
    DrawText(obj->debugString, obj->body.x + 4, obj->body.y + obj->body.height - 10, obj->titleFontSize, BLACK);
}