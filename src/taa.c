#include <stdlib.h>
#include <assert.h>

#include "taa.h"

static index_file_t getNextEmptyPostionTAA(void);
static int idIsValid(index_file_t id);

taa_t *pTable = NULL;

taa_t *initTAA(int size) {
  if (pTable == NULL) {
    pTable = malloc(sizeof(taa_t));
    assert(pTable);
    pTable->file = calloc(size, sizeof(file_t));
    assert(pTable->file);
    pTable->size = size;
  }
  return pTable;
}

void destryTAA() {
  free(pTable->file);
  free(pTable);
}

index_file_t openFileTAA(charzao_t *name, index_fs_t fsIndex, int mode, int descriptorIndex) {
  if (pTable == NULL) initTAA(FILES_MAX);
  taa_t table = *pTable;
  for (int i = 0; i < table.size; ++i) {
    if (table.file[i].vBit > 0 && compareCharzao(table.file[i].name, name) == 0) {
      if (table.file[i].fsIndex == fsIndex) {
        table.file[i].mode = mode;
        return (i+1); // First index seams to be 1 by teacher's tests
      }
    }
  }
  // We have gone through all the files in the TAA and none match the new file.
  int pos = getNextEmptyPostionTAA();
  if (pos >= 0) { // If position is valid write file info in TAA.
    table.file[pos].vBit = 1; // Marks the position as being used.
    copyCharzao(table.file[pos].name, name);
    table.file[pos].fsIndex = fsIndex;
    table.file[pos].mode = mode;
    table.file[pos].descriptorIndex = descriptorIndex;
    return (pos+1); // First index seams to be 1 by teacher's tests
  }
  return -1; // If position was not valid then the TAA is full.
}

int isFileOpenTAA(index_file_t id) {
  if (pTable == NULL) return FAIL;
  if (id < 1) return FAIL;
  id -= 1;
  return pTable->file[id].vBit;
}

index_file_t getFileIndexTAA(charzao_t *name, index_fs_t fsIndex) {
  if (pTable == NULL) return FAIL;
  taa_t table = *pTable;
  for (int i = 0; i < table.size; ++i) {
    if (table.file[i].vBit != 0 && compareCharzao(table.file[i].name, name) == 0) {
      if (table.file[i].fsIndex == fsIndex) {
        return (i+1); // First index seams to be 1 by teacher's tests
      }
    }
  }
  return FAIL; //File was not found.
}

int closeFileTAA(index_file_t id) {
  if (pTable == NULL) return FAIL;
  taa_t table = *pTable;
  id -= 1;
  if(!idIsValid(id)) return FAIL;
  table.file[id].vBit = 0; // I can empty that cell
  return SUCCESS;
}

int getFileMode(index_file_t id) {
  taa_t table = *pTable;
  id -= 1;
  if(!idIsValid(id)) return -1;
  return table.file[id].mode;
}

int getFileDescriptorIndexTAA(index_file_t id) {
  taa_t table = *pTable;
  id -= 1;
  if(!idIsValid(id)) return -1;
  return table.file[id].descriptorIndex;
}

index_fs_t getFileFSTAA(index_file_t id) {
  id -= 1;
  return pTable->file[id].fsIndex;
}

static int idIsValid(index_file_t id) { // If index is out of range or if cell was already empty we have a fail.
  taa_t table = *pTable;
  return (id >= table.size || id < 0 || table.file[id].vBit == 0) ? FAIL : SUCCESS;
}

static index_file_t getNextEmptyPostionTAA(void) {
  taa_t table = *pTable;
  int i = 0;
  while (table.file[i].vBit != 0) { // memory was initiated at 0 with calloc.
    i++;
    if (i == table.size) { // Walked through all the list and didn't find.
      return -1; // FAIL
    }
  }
  return i;
}
