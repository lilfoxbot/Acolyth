#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct List {
    int count;
    struct Node *head;
    struct Node *cur;
    struct Node *tail;
} List;

typedef struct Node {
    void *data;
    struct Node *next;
    struct Node *prev;
} Node;

List* List_Construct(){
    List* obj = (List*)malloc(sizeof(List));
    obj->head = NULL;
    obj->cur = NULL;
    obj->tail = NULL;
    obj->count = 0;

    return obj;
}

void* List_GetItem(List* list, int idx){
    if (list->count == 0){ return NULL; }
    list->cur = list->head;
    for (int i = 0; i < list->count; i++){
        if (i == idx) { return list->cur->data; }
        else { list->cur = list->cur->next; }
    }
    return NULL;
}

Node* List_GetNode(List* list, int idx){
    if (list->count == 0){ return NULL; }
    list->cur = list->head;
    for (int i = 0; i < list->count; i++){
        if (i == idx) { return list->cur; }
        else { list->cur = list->cur->next; }
    }
    return NULL;
} 

void List_FreeNode(List* list, int idx){
    Node* nodeToFree = List_GetNode(list, idx);

    if (nodeToFree == list->head){
        list->head = nodeToFree->next;
    } else if (nodeToFree == list->tail){
        nodeToFree->prev->next = NULL;
        list->tail = nodeToFree->prev;
    } else {
        nodeToFree->prev->next = nodeToFree->next;
        nodeToFree->next->prev = nodeToFree->prev;
    }
    free(nodeToFree);
    list->count--;
}

// void Free_List(Node *head) {
//     Node *temp;
//     while (head != NULL) {
//         temp = head;
//         head = head->next;
//         free(temp);
//     }
// }

void List_Push(List* list, void *new_data){
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    new_node->data = new_data;
    new_node->next = list->head; // (new_node)->(head)
    if (list->head) list->head->prev = new_node; // (prev)->(head)
    else list->tail = new_node;
    list->head = new_node;
    new_node->prev = NULL;

    list->count++;
}

void List_MoveToFront(List* list, int idx){
    void* dataToMove = List_GetItem(list, idx);
    List_Push(list, dataToMove);
    List_FreeNode(list, idx+1);
}