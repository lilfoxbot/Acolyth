#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 30
#define MAX_FEED 30

typedef struct Console{
    bool isActive;
    bool hovered;

    Rectangle inputRect;
    char inputText[MAX_INPUT+1];
    int inputCharCount;
    int inputFontSize;
    Color inputRectColor;
    Color inputOutlineColor;
    int frameCounter;

    char feed[MAX_FEED][MAX_INPUT];
    Rectangle feedRect;
    int feedOffset;

    char history[MAX_FEED][MAX_INPUT];
    int historyCount;
    int historyIdx;

} Console;

void SetText(char *dest, size_t dest_size, const char *source){
    snprintf(dest, dest_size, "%s", source);
}

Console* Create_Console(){
    Console* obj = (Console*)malloc(sizeof(Console));
    obj->isActive = true;
    obj->hovered = false;

    obj->inputRect.x = 50;
    obj->inputRect.y = 900;
    obj->inputRect.width = 500;
    obj->inputRect.height = 30;
    obj->inputFontSize = 10;
    obj->inputText[MAX_INPUT + 1] = '\0';
    obj->inputCharCount = 0;
    obj->inputRectColor = LIGHTGRAY;
    obj->inputOutlineColor = BLACK;
    SetText(obj->inputText, sizeof(obj->inputText), "");

    for (int i = 0; i < MAX_FEED; i++){
        SetText(obj->feed[i], sizeof(obj->feed[i]), "---");
    }
    obj->feedRect.width = 500;
    obj->feedRect.height = 500;
    obj->feedRect.x = obj->inputRect.x;
    obj->feedRect.y = obj->inputRect.y - obj->feedRect.height;
    obj->feedOffset = 10;

    for (int i = 0; i < MAX_FEED; i++){
        SetText(obj->history[i], sizeof(obj->feed[i]), "");
    }
    obj->historyCount = 0;
    obj->historyIdx = -1;
    
    obj->frameCounter = 0;

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
    if ((key >= 32) && (key <= 125) && key != 96 && (obj->inputCharCount < MAX_INPUT) )
    {
        obj->inputText[obj->inputCharCount] = (char)key;
        obj->inputText[obj->inputCharCount+1] = '\0'; // Add null terminator at the end of the string
        obj->inputCharCount++;
    }

    if (IsKeyPressed(KEY_BACKSPACE)){
        obj->inputCharCount--;
        if (obj->inputCharCount < 0) obj->inputCharCount = 0;
        obj->inputText[obj->inputCharCount] = '\0';
    }

    // Submission history
    if (IsKeyPressed(KEY_UP)){
        if (obj->historyCount > obj->historyIdx + 1){
            obj->historyIdx++;

            SetText(obj->inputText, sizeof(obj->inputText), obj->history[obj->historyIdx]);
            obj->inputCharCount = strlen(obj->history[obj->historyIdx]);
            obj->inputText[obj->inputCharCount+1] = '\0';
        }
    }

    if (IsKeyPressed(KEY_DOWN)){
        if (obj->historyIdx > 0){
            obj->historyIdx--;
            
            SetText(obj->inputText, sizeof(obj->inputText), obj->history[obj->historyIdx]);
            obj->inputCharCount = strlen(obj->history[obj->historyIdx]);
            obj->inputText[obj->inputCharCount+1] = '\0';
        }
    }

    if (obj->hovered) obj->frameCounter++;
    else obj->frameCounter = 0;
}

void Submit_Console(Console* obj){
    // shuffle feed
    for (int i = MAX_FEED-1; i >= 0; i--){
        SetText(obj->feed[i], sizeof(obj->feed[i]), obj->feed[i-1]);
    }
    SetText(obj->feed[0], sizeof(obj->feed[0]), obj->inputText);

    // add to history
    // shuffle history
    for (int i = MAX_FEED-1; i >= 0; i--){
        SetText(obj->history[i], sizeof(obj->history[i]), obj->history[i-1]);
    }
    SetText(obj->history[0], sizeof(obj->history[0]), obj->inputText);
    obj->historyCount++;

    // reset input
    obj->historyIdx = -1;
    obj->inputCharCount = 0;
    SetText(obj->inputText, sizeof(obj->inputText), "");
    obj->inputText[obj->inputCharCount+1] = '\0';
}

void Print_Console(Console* obj, const char* out){
    // shuffle feed
    for (int i = MAX_FEED-1; i >= 0; i--){
        SetText(obj->feed[i], sizeof(obj->feed[i]), obj->feed[i-1]);
    }
    SetText(obj->feed[0], sizeof(obj->feed[0]), out);

}

void Draw_Console(Console* obj){
    if (!obj->isActive) return;

    // INPUT
    DrawRectangle(obj->inputRect.x, obj->inputRect.y, obj->inputRect.width, obj->inputRect.height, obj->inputRectColor);
    DrawRectangleLines(obj->inputRect.x, obj->inputRect.y, obj->inputRect.width, obj->inputRect.height, obj->inputOutlineColor);
    DrawText(obj->inputText, obj->inputRect.x+5, obj->inputRect.y+obj->inputRect.height/2-6, obj->inputFontSize, BLACK);

    if (obj->inputCharCount < MAX_INPUT){
        if (((obj->frameCounter/30)%2) == 0) 
        DrawText("_", (int)obj->inputRect.x + 8 + MeasureText(obj->inputText, obj->inputFontSize), (int)obj->inputRect.y + 10, obj->inputFontSize, BLACK);
    }

    // FEED
    DrawRectangle(obj->feedRect.x, obj->feedRect.y, obj->feedRect.width, obj->feedRect.height, DARKGRAY);
    DrawRectangleLines(obj->feedRect.x, obj->feedRect.y, obj->feedRect.width, obj->feedRect.height, obj->inputOutlineColor);

    for (int i = 0; i < MAX_FEED; i++){
        DrawText(obj->feed[i], obj->inputRect.x+5, obj->inputRect.y - obj->feedOffset*(i+1), obj->inputFontSize, WHITE);
    }
}