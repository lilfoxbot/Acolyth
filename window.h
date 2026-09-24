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

    Button* closeBtn;
    bool hasCloseBtn;

    Button* buttons[4];
    int buttonCount;

    struct Window* parentWindow;

    char debugString[50];
} Window;

static void SetWindowText(char *dest, size_t dest_size, const char *source){
    snprintf(dest, dest_size, "%s", source);
}

void UpdateDebugString(Window* obj){
    SetWindowText(obj->debugString, sizeof(obj->debugString),
    TextFormat("Position: %0.2f _ %0.2f", obj->body.x, obj->body.y));
}

Window* Window_Construct(Vector2 pos, Vector2 size, char *title){
    Window* obj = (Window*)malloc(sizeof(Window));
    obj->isActive = false;
    obj->isFocused = false;
    obj->dragging = false;
    obj->dragPoint = (Vector2){0,0};

    obj->body.width = 60;
    obj->body.height = 30;
    obj->titleFontSize = 20;
    obj->fontSize = 8;

    obj->titleBarColor = LIGHTGRAY;
    obj->titleBarOutlineColor = BLACK;
    obj->bodyColor = DARKGRAY;
    obj->bodyOutlineColor = GRAY;

    obj->body.x = pos.x;
    obj->body.y = pos.y;
    obj->body.width = size.x;
    obj->body.height = size.y;

    obj->titleBar.width = obj->body.width;
    obj->titleBar.height = 30;

    obj->titleBar.x = obj->body.x;
    obj->titleBar.y = obj->body.y;

    SetWindowText(obj->title, sizeof(obj->title), title);

    obj->buttonCount = 0;

    obj->closeBtn = Button_Construct((Vector2){100,100}, (Vector2){20,20}, "", BTN_TEST);
    obj->closeBtn->defaultColorUP = GRAY;
    obj->hasCloseBtn = false;

    obj->parentWindow = NULL;
    
    return obj;
}

void Window_Destroy(Window* obj){
    if (!obj->isActive) return;
    obj->isActive = false;
}

bool Window_Check(Window* obj, Vector2 mousePoint){
    if (obj == NULL) return false;
    if (!obj->isActive) return false;

    return CheckCollisionPointRec(mousePoint, obj->body);
}

void Window_Drag(Window* obj, Vector2 mousePoint){
    if (obj == NULL) return;
    if (!obj->isActive) return;

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
}

ButtonFunction Window_Update(Window* obj, Vector2 mousePoint){
    if (obj == NULL) return BTN_NONE;
    if (!obj->isActive) return BTN_NONE;

    if (obj->isFocused){ Window_Drag(obj, mousePoint); }

    // update titlebar pos
    obj->titleBar.x = obj->body.x;
    obj->titleBar.y = obj->body.y;

    // update buttons
    ButtonFunction savedBtnFunc = BTN_NONE;
    for (int i = 0; i < obj->buttonCount; i++){
        obj->buttons[i]->rect.x = obj->body.x + 30;
        obj->buttons[i]->rect.y = obj->body.y + 60 + (i*40);
        if (obj->isFocused){
            ButtonFunction btnFunc = Button_Update(obj->buttons[i], mousePoint);
            if (btnFunc != BTN_NONE){ savedBtnFunc = btnFunc; }
        }
    }

    // update close btn
    if (obj->hasCloseBtn){
        obj->closeBtn->rect.x = obj->body.x + obj->body.width - 24;
        obj->closeBtn->rect.y = obj->body.y + 4;
        if (obj->isFocused){
            ButtonFunction btnFunc = Button_Update(obj->closeBtn, mousePoint);
            if (btnFunc != BTN_NONE){
                obj->isActive = false;
                obj->parentWindow->isFocused = true;
            }
        }
    }
    
    UpdateDebugString(obj);
    return savedBtnFunc;
}

void Window_Draw(Window* obj){
    if (obj == NULL) return;
    if (!obj->isActive) return;

    // body
    DrawRectangle(obj->body.x, obj->body.y, obj->body.width, obj->body.height, obj->bodyColor);
    if (obj->isFocused){
        DrawRectangleLines(obj->body.x, obj->body.y, obj->body.width, obj->body.height, WHITE);
        DrawRectangleLines(obj->body.x-1, obj->body.y-1, obj->body.width+2, obj->body.height+2, WHITE);
    } else {
        DrawRectangleLines(obj->body.x, obj->body.y, obj->body.width, obj->body.height, obj->bodyOutlineColor);
    }

    // title
    DrawRectangle(obj->titleBar.x, obj->titleBar.y, obj->titleBar.width, obj->titleBar.height, obj->titleBarColor);
    DrawRectangleLines(obj->titleBar.x, obj->titleBar.y, obj->titleBar.width, obj->titleBar.height, obj->bodyOutlineColor);
    DrawText(obj->title, obj->titleBar.x + 6, obj->titleBar.y + obj->titleBar.height/2 - 7, obj->titleFontSize, BLACK);

    // buttons
    for (int i = 0; i < obj->buttonCount; i++){
        Button_Draw(obj->buttons[i]);
    }
    if (obj->hasCloseBtn) Button_Draw(obj->closeBtn);

    // debug
    DrawText(obj->debugString, obj->body.x + 4, obj->body.y + obj->body.height - 20, obj->titleFontSize, BLACK);
}