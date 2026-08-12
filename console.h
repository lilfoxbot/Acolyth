#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_CHARS 30

typedef struct Console{
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

} Console;

void SetConsoleText(char *dest, size_t dest_size, const char *source) {
    snprintf(dest, dest_size, "%s", source);
}

Console* Create_Console(){
    Console* obj = (Console*)malloc(sizeof(Console));
    obj->isActive = true;
    obj->hovered = false;

    obj->rect.x = 100;
    obj->rect.y = 700;
    obj->rect.width = 300;
    obj->rect.height = 30;
    obj->fontSize = 10;
    obj->text[MAX_INPUT_CHARS + 1] = '\0';
    obj->letterCount = 0;

    obj->color = LIGHTGRAY;
    obj->outlineColor = BLACK;
    obj->frameCounter = 0;

    SetConsoleText(obj->text, sizeof(obj->text), "");
    
    return obj;
}

void Close_Console(Console* obj){
    if (!obj->isActive) return;
    obj->isActive = false;
}

void Update_Console(Console* obj){
    if (!obj->isActive) return;
    
    //obj->hovered = true;
    //SetMouseCursor(MOUSE_CURSOR_IBEAM);

    int key = GetCharPressed();

    // NOTE: Only allow keys in range [32..125]
    if ((key >= 32) && (key <= 125) && key != 96 && (obj->letterCount < MAX_INPUT_CHARS) )
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

    if (obj->hovered) obj->frameCounter++;
    else obj->frameCounter = 0;
}

void Submit_Console(Console* obj){
    obj->letterCount = 0;
    SetConsoleText(obj->text, sizeof(obj->text), "");
}

void Draw_Console(Console* obj){
    if (!obj->isActive) return;

    DrawRectangle(obj->rect.x, obj->rect.y, obj->rect.width, obj->rect.height, obj->color);
    DrawRectangleLines(obj->rect.x, obj->rect.y, obj->rect.width, obj->rect.height, obj->outlineColor);
    DrawText(obj->text, obj->rect.x+5, obj->rect.y+obj->rect.height/2-6, obj->fontSize, BLACK);

    if (obj->letterCount < MAX_INPUT_CHARS){
        if (((obj->frameCounter/30)%2) == 0) DrawText("_", (int)obj->rect.x + 8 + MeasureText(obj->text, obj->fontSize), (int)obj->rect.y + 10, obj->fontSize, BLACK);
    }
}