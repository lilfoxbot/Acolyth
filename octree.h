#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

// A simple structure to represent a 3D point
typedef struct Point {
    int x, y, z;
} Point;

// Define the octree node structure
typedef struct OctreeNode {
    Point* point; // Data stored at this node (if it's a leaf/point container)
    struct OctreeNode* children[8]; // Pointers to the eight child nodes
    // Bounding box information (min and max corners) is often included
    int minX, minY, minZ;
    int maxX, maxY, maxZ;
} OctreeNode;

// Function to create a new OctreeNode
OctreeNode* createNode(int minX, int minY, int minZ, int maxX, int maxY, int maxZ) {
    OctreeNode* node = (OctreeNode*)malloc(sizeof(OctreeNode));
    node->point = NULL; // Initially no point stored
    for (int i = 0; i < 8; i++) {
        node->children[i] = NULL;
    }
    node->minX = minX;
    node->minY = minY;
    node->minZ = minZ;
    node->maxX = maxX;
    node->maxY = maxY;
    node->maxZ = maxZ;
    return node;
}

// Function to determine which octant a point belongs to
int getOctant(OctreeNode* node, Point* p) {
    int midX = (node->minX + node->maxX) / 2;
    int midY = (node->minY + node->maxY) / 2;
    int midZ = (node->minZ + node->maxZ) / 2;

    if (p->x <= midX) {
        if (p->y <= midY) {
            if (p->z <= midZ) return 0; // TopLeftFront
            else return 4; // TopLeftBottom
        } else {
            if (p->z <= midZ) return 2; // BottomLeftFront
            else return 6; // BottomLeftBack
        }
    } else {
        if (p->y <= midY) {
            if (p->z <= midZ) return 1; // TopRightFront
            else return 5; // TopRightBottom
        } else {
            if (p->z <= midZ) return 3; // BottomRightFront
            else return 7; // BottomRightBack
        }
    }
}

// Basic insertion function
void insert(OctreeNode* node, Point* p) {
    // Check if the point is within the node's bounds
    if (p->x < node->minX || p->x > node->maxX ||
        p->y < node->minY || p->y > node->maxY ||
        p->z < node->minZ || p->z > node->maxZ) {
        return;
    }

    // If this is a leaf node and doesn't have a point yet, store the point
    // A real implementation would handle capacity limits and node splitting
    if (node->point == NULL) {
        node->point = p;
        return;
    }

    // If it has a point, and we're not at max depth, subdivide and re-insert
    // For simplicity, we just find the correct child and insert
    int octant = getOctant(node, p);
    if (node->children[octant] == NULL) {
        // Create new child node with appropriate bounds (omitted for brevity)
        // This is where bounds calculation for the 8 sub-regions would go
        // For this example, we stop at the point limit of 1 per leaf
        return;
    }
    insert(node->children[octant], p);
}

// Example usage
// int main() {
//     // Create the root node with initial bounds
//     OctreeNode* root = createNode(0, 0, 0, 100, 100, 100);

//     Point* p1 = (Point*)malloc(sizeof(Point));
//     p1->x = 10; p1->y = 20; p1->z = 30;

//     insert(root, p1);

//     printf("Point inserted\n");
//     // Free allocated memory in a complete program

//     return 0;
// }