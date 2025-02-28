#include "device.h"
#include "linked_list.h"

bool saveFilterList(void) {
  FILE *file = fopen("filters.bin", "wb");
  if (!file) {
    perror("opening file error");
    return false;
  }

  Node *node         = filterService.list->head;
  int   filtersCount = filterService.list->size(node);
  if (fwrite(&filtersCount, sizeof(int), 1, file) != 1) {
    perror("error saving filterList to file");
    fclose(file);
    return false;
  }

  while (node) {
    if (fwrite((Filter *)(node->content), sizeof(Filter), 1, file) != 1) {
      perror("error saving filterList to file");
      fclose(file);
      return false;
    }
    node = node->next;
  }

  fclose(file);
  return true;
}

bool getFilterList(void) {
  FILE *file = fopen("filters.bin", "rb");
  if (!file) {
    perror("opening file error");
    return false;
  }

  int filtersCount;
  if (fread(&filtersCount, sizeof(int), 1, file) != 1) {
    perror("error geting filterList from file");
    fclose(file);
    return false;
  }

  for (int i = 0; i < filtersCount; i++) {
    Filter *filter = malloc(sizeof(Filter));
    if (!filter) {
      perror("error allocating memory for filterList");
      free(filter);
      fclose(file);
      return false;
    }
    if (fread(filter, sizeof(Filter), 1, file) != 1) {
      perror("error geting filterList from file");
      free(filter);
      fclose(file);
      return false;
    }
    filterService.list->push(&filterService.list->head, filter);
  }

  fclose(file);
  return true;
}