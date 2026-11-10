#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

#include "voxel.h"
#include "bullet.h"
#include "pawn.h"
#include "player.h"

extern bool myDebug;

const int MAX_BOXTREE_DEPTH = 4;

typedef struct BoxtreeNode {
    BoundingBox bb;
    Vector3 position;
    int size;
    int depth;
    Color debugColor;
    bool isRayHit;
    bool nodeTouched;
    struct BoxtreeNode* children[8];

    int voxelCount;
    int bulletCount;
    int pawnCount;
    struct Voxel* voxels[64];
    struct Bullet* bullets[32];
    struct Pawn* pawns[32];
} BoxtreeNode;

BoxtreeNode* Boxtree_CreateNode(Vector3 center, int size, int depth) {
    BoxtreeNode* node = (BoxtreeNode*)malloc(sizeof(BoxtreeNode));
    node->position = center;
    node->size = size;
    node->bb.min = Vector3Subtract(center, (Vector3){size/2.0f, size/2.0f, size/2.0f});
    node->bb.max = Vector3Add(center, (Vector3){size/2.0f, size/2.0f, size/2.0f});

    for (int i = 0; i < 8; i++) { node->children[i] = NULL; }
    for (int i = 0; i < 64; i++) { node->voxels[i] = NULL; }
    for (int i = 0; i < 32; i++) { node->bullets[i] = NULL; }
    for (int i = 0; i < 32; i++) { node->pawns[i] = NULL; }
    node->voxelCount = 0;
    node->bulletCount = 0;
    node->pawnCount = 0;

    node->debugColor = WHITE;
    node->isRayHit = false;
    node->depth = depth;
    return node;
}

BoxtreeNode* Boxtree_Build(Vector3 center, int size, int depth){
    if (depth > MAX_BOXTREE_DEPTH) return NULL;

    BoxtreeNode* boxTreeNode = Boxtree_CreateNode(center, size, depth);
    int newSize = size / 2; 
    Vector3 RUF = Vector3Add(center,(Vector3){newSize/2,newSize/2,newSize/2});
    Vector3 LUF = Vector3Add(center,(Vector3){-newSize/2,newSize/2,newSize/2});
    Vector3 LDF = Vector3Add(center,(Vector3){-newSize/2,-newSize/2,newSize/2});
    Vector3 RDF = Vector3Add(center,(Vector3){newSize/2,-newSize/2,newSize/2});
    Vector3 RUB = Vector3Add(center,(Vector3){newSize/2,newSize/2,-newSize/2});
    Vector3 LUB = Vector3Add(center,(Vector3){-newSize/2,newSize/2,-newSize/2});
    Vector3 LDB = Vector3Add(center,(Vector3){-newSize/2,-newSize/2,-newSize/2});
    Vector3 RDB = Vector3Add(center,(Vector3){newSize/2,-newSize/2,-newSize/2});

    // right-up-front
    boxTreeNode->children[0] = Boxtree_Build(RUF, newSize, depth + 1);
    // left-up-front
    boxTreeNode->children[1] = Boxtree_Build(LUF, newSize, depth + 1);
    // left-down-front
    boxTreeNode->children[2] = Boxtree_Build(LDF, newSize, depth + 1);
    // right-down-front
    boxTreeNode->children[3] = Boxtree_Build(RDF, newSize, depth + 1);
    // right-up-back
    boxTreeNode->children[4] = Boxtree_Build(RUB, newSize, depth + 1);
    // left-up-back
    boxTreeNode->children[5] = Boxtree_Build(LUB, newSize, depth + 1);
    // left-down-back
    boxTreeNode->children[6] = Boxtree_Build(LDB, newSize, depth + 1);
    // right-down-back
    boxTreeNode->children[7] = Boxtree_Build(RDB, newSize, depth + 1);
    
    return boxTreeNode;
}

void Boxtree_Reset(BoxtreeNode* node){
    if (node == NULL) return;

    node->isRayHit = false;
    node->nodeTouched = false;

    node->bulletCount = 0;
    memset(node->bullets, 0, sizeof(node->bullets));

    node->pawnCount = 0;
    memset(node->pawns, 0, sizeof(node->pawns));

    for (int i = 0; i < 8; i++){
        Boxtree_Reset(node->children[i]);
    }
}

void Boxtree_DrawNode(BoxtreeNode* node){
    if (node == NULL) return;
    if (!myDebug) return;

    DrawCubeWires(node->position, node->size, node->size, node->size, RED); 
    
    // if(node->depth == 1){
    //     DrawCubeWires(node->position, node->size, node->size, node->size, RED);  
    // } 

    for (int i = 0; i < 8; i++){
        Boxtree_DrawNode(node->children[i]);
    }
}

// @CHECKIN ===

// void Boxtree_GetRayVoxels(Ray ray, BoxtreeNode* node, Voxel** hitVoxels, int maxHits) {
//     if (node == NULL) return;

//     if (GetRayCollisionBox(ray, node->bb).hit){
//         node->isRayHit = true;
//         if (node->depth == MAX_BOXTREE_DEPTH) {
//             for (int i = 0; i < node->voxelCount; i++){
//                 if (!node->voxels[i]->isActive) continue;
//                 RayCollision rc = GetRayCollisionBox(ray, node->voxels[i]->bb);
//                 if (rc.hit){
                    
//                     for (int j = 0; j < maxHits; j++){
//                         if (hitVoxels[j] == NULL){
//                             hitVoxels[j] = node->voxels[i];
//                             break;
//                         }
//                     }
//                 }
//             }
//         } else {
//             for (int i = 0; i < 8; i++) {
//                 Boxtree_GetRayVoxels(ray, node->children[i], hitVoxels, maxHits);
//             }
//         }
//     }
// }

void Boxtree_GetBulletNodes(Bullet* bullet, BoxtreeNode* node){
    if (node == NULL || !bullet->isActive) return;
    
    if (CheckCollisionBoxes(node->bb, bullet->bb)){
        if (node->depth == MAX_BOXTREE_DEPTH) {
            node->nodeTouched = true;

            // place node into bullet
            bullet->nodes[bullet->nodeCount] = node;
            bullet->nodeCount++;

            // place bullet into node
            node->bullets[node->bulletCount] = bullet;
            node->bulletCount++;

        } else {
            for (int i = 0; i < 8; i++) {
                Boxtree_GetBulletNodes(bullet, node->children[i]);
            }
        }
    }
}

void Boxtree_GetPawnNodes(Pawn* pawn, BoxtreeNode* node){
    if (node == NULL || !pawn->isActive) return;

    if (CheckCollisionBoxes(node->bb, pawn->bb)){
        if (node->depth == MAX_BOXTREE_DEPTH){
            node->nodeTouched = true;
            pawn->nodes[pawn->nodeCount] = node;
            pawn->nodeCount++;
            node->pawns[node->pawnCount] = pawn;
            node->pawnCount++;
        } else {
            for (int i = 0; i < 8; i++) {
                Boxtree_GetPawnNodes(pawn, node->children[i]);
            }
        }
    }
}

void Boxtree_GetPlayerNodes(Player* player, BoxtreeNode* node){
    if (node == NULL || !player->isActive) return;

    if (CheckCollisionBoxes(node->bb, player->bb)){
        if (node->depth == MAX_BOXTREE_DEPTH){
            player->nodes[player->nodeCount] = node;
            player->nodeCount++;
            node->nodeTouched = true;
        } else {
            for (int i = 0; i < 8; i++) {
                Boxtree_GetPlayerNodes(player, node->children[i]);
            }
        }
    }
}