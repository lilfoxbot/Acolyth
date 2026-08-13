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

    Rectangle inputRect;
    char inputText[MAX_INPUT_CHARS+1];
    int inputCharCount;
    int inputFontSize;
    Color inputRectColor;
    Color inputOutlineColor;
    int frameCounter;

    char feed[3][30];
    Rectangle feedRect;

} Console;

void SetConsoleText(char *dest, size_t dest_size, const char *source) {
    snprintf(dest, dest_size, "%s", source);
}

Console* Create_Console(){
    Console* obj = (Console*)malloc(sizeof(Console));
    obj->isActive = true;
    obj->hovered = false;

    obj->inputRect.x = 50;
    obj->inputRect.y = 900;
    obj->inputRect.width = 300;
    obj->inputRect.height = 30;
    obj->inputFontSize = 10;
    obj->inputText[MAX_INPUT_CHARS + 1] = '\0';
    obj->inputCharCount = 0;
    obj->inputRectColor = LIGHTGRAY;
    obj->inputOutlineColor = BLACK;

    obj->frameCounter = 0;

    SetConsoleText(obj->inputText, sizeof(obj->inputText), "");

    // set default feed strings
    obj->feed[0][0] = '0';
    obj->feed[1][0] = '0';
    obj->feed[2][0] = '0';

    obj->feedRect.width = 500;
    obj->feedRect.height = 500;
    obj->feedRect.x = obj->inputRect.x;
    obj->feedRect.y = obj->inputRect.y - obj->feedRect.height;
    
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

    // NOTE: Only allow keys in range [32..125] ... and ignore '~'
    if ((key >= 32) && (key <= 125) && key != 96 && (obj->inputCharCount < MAX_INPUT_CHARS) )
    {
        obj->inputText[obj->inputCharCount] = (char)key;
        obj->inputText[obj->inputCharCount+1] = '\0'; // Add null terminator at the end of the string
        obj->inputCharCount++;
    }

    if (IsKeyPressed(KEY_BACKSPACE))
    {
        obj->inputCharCount--;
        if (obj->inputCharCount < 0) obj->inputCharCount = 0;
        obj->inputText[obj->inputCharCount] = '\0';
    }

    if (obj->hovered) obj->frameCounter++;
    else obj->frameCounter = 0;
}

void Submit_Console(Console* obj){
    obj->inputCharCount = 0;
    SetConsoleText(obj->inputText, sizeof(obj->inputText), "");
}

void Draw_Console(Console* obj){
    if (!obj->isActive) return;

    DrawRectangle(obj->inputRect.x, obj->inputRect.y, obj->inputRect.width, obj->inputRect.height, obj->inputRectColor);
    DrawRectangleLines(obj->inputRect.x, obj->inputRect.y, obj->inputRect.width, obj->inputRect.height, obj->inputOutlineColor);
    DrawText(obj->inputText, obj->inputRect.x+5, obj->inputRect.y+obj->inputRect.height/2-6, obj->inputFontSize, BLACK);

    if (obj->inputCharCount < MAX_INPUT_CHARS){
        if (((obj->frameCounter/30)%2) == 0) 
        DrawText("_", (int)obj->inputRect.x + 8 + MeasureText(obj->inputText, obj->inputFontSize), (int)obj->inputRect.y + 10, obj->inputFontSize, BLACK);
    }

    DrawRectangle(obj->feedRect.x, obj->feedRect.y, obj->feedRect.width, obj->feedRect.height, DARKGRAY);
}