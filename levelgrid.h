/*
*   single cube struct and functions
*/

#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

struct _LevelCell{
    int exist;
    Vector3 position;
    Vector3 size;
    struct BoundingBox bb;
};

struct _LevelGrid{
    int rows;
    int cols;
    int cellCount;
    float cellSize;
};

int InitLevelGrid(struct _LevelGrid* grid, int rows, int cols, float cellSize){
    grid->rows = rows;
    grid->cols = cols;
    grid->cellSize = cellSize;
    grid->cellCount = rows * cols;

    return rows * cols;
}

void DrawLevelGrid(struct _LevelGrid* grid){
    DrawGrid(grid->rows, grid->cellSize);
    DrawCubeWires((Vector3){0,0,0}, grid->rows, 0.1f, grid->cols, WHITE);
}

void DrawCell(struct _LevelCell* cell){
    DrawBoundingBox(cell->bb, GREEN);
}