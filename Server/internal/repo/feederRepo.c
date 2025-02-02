#include "device.h"
#include "linked_list.h"

bool saveFeederList(void) {
  FILE *file = fopen("feeders.bin", "wb");
  if (!file) {
    perror("opening file error");
    return false;
  }

  Node *node         = feederService.list->head;
  int   feedersCount = feederService.list->size(node);
  if (fwrite(&feedersCount, sizeof(int), 1, file) != 1) {
    perror("error saving feederList to file");
    fclose(file);
    return false;
  }

  while (node) {
    Feeder *feeder = (Feeder *)((Device *)node->content)->properties;
    if (fwrite(feeder, sizeof(Feeder), 1, file) != 1) {
      perror("error saving feederList to file");
      fclose(file);
      return false;
    }
    node = node->next;
  }

  fclose(file);
  return false;
}

bool getFeederList(void) {
  FILE *file = fopen("feeders.bin", "rb");
  if (!file) {
    perror("opening file error");
    return false;
  }

  int feedersCount;
  if (fread(&feedersCount, sizeof(int), 1, file) != 1) {
    perror("error geting feederList from file");
    fclose(file);
    return false;
  }

  for (int i = 0; i < feedersCount; i++) {
    Device *device = calloc(1, sizeof(Device));
    if (!device) {
      perror("error allocating memory for feederList");
      fclose(file);
      return false;
    }

    Feeder *feeder = malloc(sizeof(Feeder));
    if (!feeder) {
      perror("error allocating memory for feederList");
      free(device);
      fclose(file);
      return false;
    }
    if (fread(feeder, sizeof(Feeder), 1, file) != 1) {
      perror("error geting feederList from file");
      free(device);
      free(feeder);
      fclose(file);
      return false;
    }

    device->type       = FEEDER;
    device->properties = feeder;
    feederService.list->push(&feederService.list->head, device);
  }

  fclose(file);
  return true;
}