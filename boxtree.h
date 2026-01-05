#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>


typedef struct BoxtreeNode {
    BoundingBox box;
    Vector3 position;
    int size;
    struct BoxtreeNode* children[8];
    struct BoundingBox* voxels[16];
    int depth;
    Color debugColor;
    bool isRayHit;
} BoxtreeNode;

BoxtreeNode* CreateBoxtreeNode(Vector3 center, int size, int depth) {
    BoxtreeNode* node = (BoxtreeNode*)malloc(sizeof(BoxtreeNode));
    node->position = center;
    node->size = size;
    node->box.min = Vector3Subtract(center, (Vector3){size/2.0f, size/2.0f, size/2.0f});
    node->box.max = Vector3Add(center, (Vector3){size/2.0f, size/2.0f, size/2.0f});
    for (int i = 0; i < 8; i++) {
        node->children[i] = NULL;
    }

    node->debugColor = WHITE;
    node->isRayHit = false;
    node->depth = depth;
    return node;
}

BoxtreeNode* BuildBoxtree(Vector3 center, int size, int depth) {
    if (depth <= 0) return NULL;

    BoxtreeNode* boxTreeNode = CreateBoxtreeNode(center, size, depth);
    int newSize = size / 2; 
    Vector3 RTF = Vector3Add(center,(Vector3){newSize/2,newSize/2,newSize/2});
    Vector3 LTF = Vector3Add(center,(Vector3){-newSize/2,newSize/2,newSize/2});
    Vector3 LBF = Vector3Add(center,(Vector3){-newSize/2,-newSize/2,newSize/2});
    Vector3 RBF = Vector3Add(center,(Vector3){newSize/2,-newSize/2,newSize/2});
    Vector3 RTB = Vector3Add(center,(Vector3){newSize/2,newSize/2,-newSize/2});
    Vector3 LTB = Vector3Add(center,(Vector3){-newSize/2,newSize/2,-newSize/2});
    Vector3 LBB = Vector3Add(center,(Vector3){-newSize/2,-newSize/2,-newSize/2});
    Vector3 RBB = Vector3Add(center,(Vector3){newSize/2,-newSize/2,-newSize/2});

    // right-top-front
    boxTreeNode->children[0] = BuildBoxtree(RTF, newSize, depth - 1);
    // left-top-front
    boxTreeNode->children[1] = BuildBoxtree(LTF, newSize, depth - 1);
    // left-bottom-front
    boxTreeNode->children[2] = BuildBoxtree(LBF, newSize, depth - 1);
    // right-bottom-front
    boxTreeNode->children[3] = BuildBoxtree(RBF, newSize, depth - 1);
    // right-top-back
    boxTreeNode->children[4] = BuildBoxtree(RTB, newSize, depth - 1);
    // left-top-back
    boxTreeNode->children[5] = BuildBoxtree(LTB, newSize, depth - 1);
    // left-bottom-back
    boxTreeNode->children[6] = BuildBoxtree(LBB, newSize, depth - 1);
    // right-bottom-back
    boxTreeNode->children[7] = BuildBoxtree(RBB, newSize, depth - 1);
    
    return boxTreeNode;
}

void ResetBoxtree(BoxtreeNode* node) {
    if (node == NULL) return;

    node->isRayHit = false;

    for (int i = 0; i < 8; i++) {
        ResetBoxtree(node->children[i]);
    }
}

void CheckBoxtree_Ray(BoxtreeNode* node, Ray ray) {
    if (node == NULL) return;

    if (GetRayCollisionBox(ray, node->box).hit){
        if (node->depth == 1) node->isRayHit = true;

        for (int i = 0; i < 8; i++) {
            CheckBoxtree_Ray(node->children[i], ray);
        }
    }
}

void CheckBoxtree_Box(BoxtreeNode* node, BoundingBox bb){
    if (node == NULL) return;

    if (CheckCollisionBoxes(node->box, bb)){
        if (node->depth == 1) {
            node->voxels[0] = &bb;
            if (node->isRayHit) {
                DrawBoundingBox(bb, WHITE);
            }
        } 

        for (int i = 0; i < 8; i++) {
            CheckBoxtree_Box(node->children[i], bb);
        }
    }
}

void DrawBoxtreeNode(BoxtreeNode* node) {
    if (node == NULL) return;

    if (node->isRayHit) {
        DrawCubeWires(node->position,node->size-0.1f,node->size-0.1f,node->size-0.1f, RED);
    } else {
        node->debugColor = WHITE;
    }

    for (int i = 0; i < 8; i++) {
        DrawBoxtreeNode(node->children[i]);
    }
}
