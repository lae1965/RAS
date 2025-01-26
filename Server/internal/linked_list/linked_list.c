#include "linked_list.h"

static Node *push(Node **head, void *content) {
  Node *node = malloc(sizeof(Node));
  if (!node) {
    perror("malloc new node");
    return NULL;
  }
  node->content = content;
  node->next    = NULL;
  if (*head == NULL) *head = node;
  else {
    Node *prevNode = *head;
    while (prevNode->next != NULL) prevNode = prevNode->next;
    prevNode->next = node;
  }

  return node;
}

static bool erase(Node **head, Node *erasingNode, bool isClearContent) {
  if (*head == NULL) return true;

  Node *node = *head, *prevNode = NULL;
  while (node != erasingNode) {
    prevNode = node;
    node     = node->next;
    if (!node) return false;
  }
  if (prevNode) prevNode->next = node->next;
  else *head = node->next;
  if (isClearContent && node->content != NULL) free(node->content);
  free(node);

  return true;
}

static void clear(Node **head, bool isClearContent) {
  Node *node = *head, *prevNode;

  *head = NULL;
  while (node) {
    prevNode = node;
    node     = node->next;
    if (isClearContent && prevNode->content != NULL) free(prevNode->content);
    free(prevNode);
  }
}

static int size(Node *head) {
  Node *node  = head;
  int   count = 0;

  while (node) {
    node = node->next;
    count++;
  }

  return count;
}

static Node *getByCondition(Node *head, void *checkingContext, bool (*checkCondition)(void *content1, void *content2)) {
  Node *node = head;

  while (node) {
    if (checkCondition(node->content, checkingContext)) break;
    node = node->next;
  }

  return node;
}

static Node *getByNumber(Node *head, int number) {
  Node *node = head;

  while (number > 0) {
    node = node->next;
    if (node == NULL) return NULL;
    number--;
  }

  return node;
}

// Конструктор
SinglyLinkedList *newSinglyLinkedList(void) {
  SinglyLinkedList *newList = malloc(sizeof(SinglyLinkedList));
  if (!newList) {
    perror("malloc new SinglyLinkedList");
    return NULL;
  }

  newList->head           = NULL;
  newList->push           = push;
  newList->erase          = erase;
  newList->clear          = clear;
  newList->size           = size;
  newList->getByNumber    = getByNumber;
  newList->getByCondition = getByCondition;
  return newList;
}

// Деструктор
void clearSinglyLinkedList(SinglyLinkedList *list, bool isClearContent) {
  list->clear(&list->head, isClearContent);
  free(list);
}