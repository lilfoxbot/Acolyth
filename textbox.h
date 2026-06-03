#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_CHARS 10

typedef struct Textbox{
    bool isActive;
    bool hovered;

    Rectangle rect;
    char text[MAX_INPUT_CHARS+1];
    //char text[MAX_INPUT_CHARS + 1] = "\0";
    int letterCount;
    int fontSize;
    Color color;
    Color outlineColor;
    int frameCounter;

} Textbox;

void SetTextBoxArray(char *dest, size_t dest_size, const char *source) {
    snprintf(dest, dest_size, "%s", source);
}

Textbox* Create_Textbox(){
    Textbox* obj = (Textbox*)malloc(sizeof(Textbox));
    obj->isActive = false;
    obj->hovered = false;

    obj->rect.width = 60;
    obj->rect.height = 30;
    obj->fontSize = 10;
    obj->text[MAX_INPUT_CHARS + 1] = '\0';
    obj->letterCount = 0;

    obj->color = LIGHTGRAY;
    obj->outlineColor = BLACK;
    obj->frameCounter;
    
    return obj;
}

void Spawn_Textbox(Textbox* obj, Vector2 pos, Vector2 size, int fontSize){
    obj->isActive = true;
    obj->rect.x = pos.x;
    obj->rect.y = pos.y;
    obj->rect.width = size.x;
    obj->rect.height = size.y;
    obj->fontSize = fontSize;

    SetTextBoxArray(obj->text, sizeof(obj->text), "");
}

void Destroy_Textbox(Textbox* obj){
    if (!obj->isActive) return;
    obj->isActive = false;
}

void Update_Textbox(Textbox* obj, Vector2 mousePoint){
    if (!obj->isActive) return;
    if (CheckCollisionPointRec(mousePoint, obj->rect)){
        obj->hovered = true;
        SetMouseCursor(MOUSE_CURSOR_IBEAM);

        int key = GetCharPressed();

        // NOTE: Only allow keys in range [32..125]
        if ((key >= 32) && (key <= 125) && (obj->letterCount < MAX_INPUT_CHARS))
        {
            obj->text[obj->letterCount] = (char)key;
            obj->text[obj->letterCount+1] = '\0'; // Add null terminator at the end of the string
            obj->letterCount++;
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            obj->letterCount--;
            if (obj->letterCount < 0) obj->letterCount = 0;
            obj->text[obj->letterCount] = '\0';
        }

    } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        obj->hovered = false;
    }

    if (obj->hovered) obj->frameCounter++;
        else obj->frameCounter = 0;
}

void Draw_Textbox(Textbox* obj){
    if (!obj->isActive) return;

    DrawRectangle(obj->rect.x, obj->rect.y, obj->rect.width, obj->rect.height, obj->color);
    DrawRectangleLines(obj->rect.x, obj->rect.y, obj->rect.width, obj->rect.height, obj->outlineColor);
    DrawText(obj->text, obj->rect.x+5, obj->rect.y+obj->rect.height/2-6, obj->fontSize, BLACK);

    if (obj->hovered){
        if (obj->letterCount < MAX_INPUT_CHARS){
            if (((obj->frameCounter/30)%2) == 0) DrawText("_", (int)obj->rect.x + 8 + MeasureText(obj->text, obj->fontSize), (int)obj->rect.y + 10, obj->fontSize, BLACK);
        }
    }
}