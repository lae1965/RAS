#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
  void        *content;
  struct node *next;
} Node;

typedef struct {
  Node *head;
  Node *(*push)(Node **head, void *content);
  bool (*erase)(Node **head, Node *erasingNode, void (*clearInternalContent)(void *content));
  void (*clear)(Node **head, void (*clearInternalContent)(void *content));
  int (*size)(Node *head);
  Node *(*getByNumber)(Node *head, int number);
  Node *(*getByCondition)(Node *head, void *checkingCondition, bool (*checkCondition)(void *context1, void *content2));
} SinglyLinkedList;

SinglyLinkedList *newSinglyLinkedList(void);
void              clearSinglyLinkedList(SinglyLinkedList *list, void (*clearInternalContent)(void *content));

#endif
