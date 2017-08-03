#include "version.h"
#include "useful.h"
#include "taa.h"

#include <stdlib.h>

static int getNextEmptyPostionTAA(taa_t table);

taa_t *initTAA(int size) {
  taa_t *ret;
  ret = malloc(sizeof(taa_t));
  ret->file = calloc(size, sizeof(file_t));
  ret->size = size;
  return ret;
}

int openFileTAA(taa_t table, charzao_t *name, index_fs_t fsIndex, int mode, int descriptorIndex) {
  for (int i = 0; i < table.size; ++i) {
    if (compareCharzao(table.file[i].name, name) == 0) {
      if (table.file[i].fsIndex == fsIndex) {
        return FAIL;
      }
    }
  }
  // We have gone through all the files in the TAA and none match the new file.
  int pos = getNextEmptyPostionTAA(table);
  if (pos >= 0) { // If position is valid write file info in TAA.
    copyCharzao(table.file[pos].name, name);
    table.file[pos].fsIndex = fsIndex;
    table.file[pos].mode = mode;
    table.file[pos].descriptorIndex = descriptorIndex;
    return SUCCESS;
  }
  return FAIL; // If position was not valid then the TAA is full.
}

static int getNextEmptyPostionTAA(taa_t table) {
  int i = 0;
  while (table.file[i].name[0] != 0) { // memory was initiated at 0 with calloc.
    i++;
    if (i == table.size) { // Walked through all the list and didn't find.
      return -1; // FAIL
    }
  }
  return i;
}
