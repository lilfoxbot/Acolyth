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
} Node;

List* Create_List(){
    List* obj = (List*)malloc(sizeof(List));
    obj->head = NULL;
    obj->count = 0;

    return obj;
}

void Push(List* list, void *new_data){
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

void* GetListItem(List* list, int idx){
    if (list->count == 0){ return NULL; }
    list->cur = list->head;
    for (int i = 0; i < list->count; i++){
        if (i == idx) { return list->cur->data; }
        else { list->cur = list->cur->next; }
    }
    return NULL;
}

void FreeList(Node *head) {
    Node *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}