#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct List {
    int count;
    struct Node *head;
    struct Node *tail;
} List;

typedef struct Node {
    void *data;
    struct Node *next;
} Node;

List* Create_List(){
    List* obj = (List*)malloc(sizeof(List));
    obj->count = 0;

    return obj;
}

void Push(List* list, void *new_data) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    new_node->data = new_data;
    new_node->next = list->head;
    list->head = new_node;
    list->count++;
}

// Push() example
// int int_val1 = 10, int_val2 = 20;
// Push(&head, &int_val1);
// Push(&head, &int_val2);

// void Push(Node **head_ref, void *new_data) {
//     Node *new_node = (Node *)malloc(sizeof(Node));
//     if (new_node == NULL) {
//         printf("Memory allocation failed.\n");
//         exit(1);
//     }
//     new_node->data = new_data;
//     new_node->next = (*head_ref);
//     (*head_ref) = new_node;
// }

void FreeList(Node *head) {
    Node *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}