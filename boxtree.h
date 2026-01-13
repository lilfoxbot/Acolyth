#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

#include "voxel.h"
#include "bullet.h"

const int MAX_BOXTREE_DEPTH = 4;

typedef struct BoxtreeNode {
    BoundingBox bb;
    Vector3 position;
    int size;
    struct BoxtreeNode* children[8];
    struct Voxel* voxels[64];
    struct Bullet* bullets[32];
    int voxelCount;
    int depth;
    Color debugColor;
    bool isRayHit;
} BoxtreeNode;

BoxtreeNode* CreateBoxtreeNode(Vector3 center, int size, int depth) {
    BoxtreeNode* node = (BoxtreeNode*)malloc(sizeof(BoxtreeNode));
    node->position = center;
    node->size = size;
    node->bb.min = Vector3Subtract(center, (Vector3){size/2.0f, size/2.0f, size/2.0f});
    node->bb.max = Vector3Add(center, (Vector3){size/2.0f, size/2.0f, size/2.0f});
    for (int i = 0; i < 8; i++) { node->children[i] = NULL; }
    for (int i = 0; i < 64; i++) { node->voxels[i] = NULL; }
    for (int i = 0; i < 32; i++) { node->bullets[i] = NULL; }
    node->voxelCount = 0;

    node->debugColor = WHITE;
    node->isRayHit = false;
    node->depth = depth;
    return node;
}

BoxtreeNode* BuildBoxtree(Vector3 center, int size, int depth) {
    if (depth > MAX_BOXTREE_DEPTH) return NULL;

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
    boxTreeNode->children[0] = BuildBoxtree(RTF, newSize, depth + 1);
    // left-top-front
    boxTreeNode->children[1] = BuildBoxtree(LTF, newSize, depth + 1);
    // left-bottom-front
    boxTreeNode->children[2] = BuildBoxtree(LBF, newSize, depth + 1);
    // right-bottom-front
    boxTreeNode->children[3] = BuildBoxtree(RBF, newSize, depth + 1);
    // right-top-back
    boxTreeNode->children[4] = BuildBoxtree(RTB, newSize, depth + 1);
    // left-top-back
    boxTreeNode->children[5] = BuildBoxtree(LTB, newSize, depth + 1);
    // left-bottom-back
    boxTreeNode->children[6] = BuildBoxtree(LBB, newSize, depth + 1);
    // right-bottom-back
    boxTreeNode->children[7] = BuildBoxtree(RBB, newSize, depth + 1);
    
    return boxTreeNode;
}

void ResetBoxtree(BoxtreeNode* node) {
    if (node == NULL) return;

    node->isRayHit = false;

    for (int i = 0; i < 8; i++) {
        ResetBoxtree(node->children[i]);
    }
}

void GetRayVoxels(Ray ray, BoxtreeNode* node, Voxel** hitVoxels, int maxHits) {
    if (node == NULL) return;

    if (GetRayCollisionBox(ray, node->bb).hit){
        node->isRayHit = true;
        if (node->depth == MAX_BOXTREE_DEPTH) {
            
            for (int i = 0; i < node->voxelCount; i++){
                if (!node->voxels[i]->isActive) continue;
                RayCollision rc = GetRayCollisionBox(ray, node->voxels[i]->bb);
                if (rc.hit){
                    //node->voxels[i]->bbColor = WHITE;
                    // add to hit list
                    for (int j = 0; j < maxHits; j++){
                        if (hitVoxels[j] == NULL){
                            hitVoxels[j] = node->voxels[i];
                            break;
                        }
                    }
                }
            }
        } else {
            for (int i = 0; i < 8; i++) {
                GetRayVoxels(ray, node->children[i], hitVoxels, maxHits);
                
            }
        }
    }
}

void CheckInBullet(BoxtreeNode* node, Bullet* bullet){
    if (node == NULL || bullet == NULL) return;
    
    //place bullet into overlapping nodes
    if (CheckCollisionBoxes(node->bb, bullet->bb)){
        if (node->depth == MAX_BOXTREE_DEPTH) {

        } else {
            for (int i = 0; i < 8; i++) {
                CheckInBullet(node->children[i], bullet);
            }
        }
    }

}

void DrawBoxtreeNode(BoxtreeNode* node) {
    if (node == NULL) return;

    if (node->isRayHit) {
        if(node->depth == MAX_BOXTREE_DEPTH){
            DrawCubeWires(node->position,node->size-0.1f,node->size-0.1f,node->size-0.1f, YELLOW);
        } else {
            DrawCubeWires(node->position,node->size-0.1f,node->size-0.1f,node->size-0.1f, RED);
        }
        
    } else {
        node->debugColor = WHITE;
    }

    for (int i = 0; i < 8; i++) {
        DrawBoxtreeNode(node->children[i]);
    }
}
