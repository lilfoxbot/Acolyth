#pragma once

#include "raylib.h"

// Store levels as strings of ints, each character determines the size of the voxel when it is constructed.
// [stages][grids][voxels]
char levels[10][4][300] = {
    {"0123456789", "0123456789", "0123456789", "0123456789"},
    {"0123456789", "0123456789", "0123456789", "0123456789"},
    {"0123456789", "0123456789", "0123456789", "0123456789"}
};
int levelSelection = 0;

Sound bullet_shot;

Model testModel;

void LoadSounds(){
    bullet_shot = LoadSound("resources/sound/shot.wav");
}

void LoadModels(){
    testModel = LoadModel("resources/models/myCube.obj");
}