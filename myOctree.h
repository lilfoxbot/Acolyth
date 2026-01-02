#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

// Define the octree node structure
typedef struct OctreeNode {
    struct OctreeNode* children[8]; // Pointers to the eight child nodes
    // Bounding box information (min and max corners) is often included
    int minX, minY, minZ;
    int maxX, maxY, maxZ;
    int size; // length of one edge of the cube
    Vector3 center; // midpoint
    BoundingBox bb;
    bool isHit;
} OctreeNode;

// Function to create a new OctreeNode
OctreeNode* CreateOctreeNode(int minX, int minY, int minZ, int maxX, int maxY, int maxZ, int newSize) {
    if (newSize == 1){return NULL;}

    OctreeNode* node = (OctreeNode*)malloc(sizeof(OctreeNode));
    for (int i = 0; i < 8; i++) {
        node->children[i] = NULL;
    }
    node->minX = minX;
    node->minY = minY;
    node->minZ = minZ;
    node->maxX = maxX;
    node->maxY = maxY;
    node->maxZ = maxZ;
    node->size = newSize;
    node->center = (Vector3){0,0,0}; // calculated afterwards
    node->bb.min = (Vector3){(float)minX, (float)minY, (float)minZ};
    node->bb.max = (Vector3){(float)maxX, (float)maxY, (float)maxZ};
    node->isHit = false;
    return node;
}

void CheckRayOctree(OctreeNode* node, struct Ray ray) {
    if(GetRayCollisionBox(ray, node->bb).hit) {
        node->isHit = true;
    }  
}

void DrawOctreeNode(OctreeNode* node) {
    if (node == NULL) return;

    if (node->isHit){
        DrawBoundingBox(node->bb, RED);
    } else {
        DrawBoundingBox(node->bb, WHITE);
    }

    // Recursively draw child nodes
    for (int i = 0; i < 8; i++) {
        DrawOctreeNode(node->children[i]);
    }
}

void DestroyOctreeNode(OctreeNode* node) {
    if (node == NULL) return;

    // Recursively free child nodes
    for (int i = 0; i < 8; i++) {
        DestroyOctreeNode(node->children[i]);
    }

    // Free the current node
    free(node);
}
