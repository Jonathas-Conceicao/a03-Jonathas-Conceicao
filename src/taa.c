#include "version.h"
#include "useful.h"
#include "taa.h"

#include <stdlib.h>

static index_file_t getNextEmptyPostionTAA(taa_t table);
static int idIsValid(taa_t table, index_file_t id);

taa_t *initTAA(int size) {
  taa_t *ret;
  ret = malloc(sizeof(taa_t));
  ret->file = calloc(size, sizeof(file_t));
  ret->size = size;
  return ret;
}

index_file_t openFileTAA(taa_t table, charzao_t *name, index_fs_t fsIndex, int mode, int descriptorIndex) {
  for (int i = 0; i < table.size; ++i) {
    if (table.file[i].vBit > 0 && compareCharzao(table.file[i].name, name) == 0) {
      if (table.file[i].fsIndex == fsIndex) {
        table.file[i].mode = mode;
        return (i+1); // First index seams to be 1 by teacher's tests
      }
    }
  }
  // We have gone through all the files in the TAA and none match the new file.
  int pos = getNextEmptyPostionTAA(table);
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

index_file_t getFileIndex(taa_t table, charzao_t *name, index_fs_t fsIndex) {
  for (int i = 0; i < table.size; ++i) {
    if (table.file[i].vBit > 0 && compareCharzao(table.file[i].name, name) == 0) {
      if (table.file[i].fsIndex == fsIndex) {
        return (i+1); // First index seams to be 1 by teacher's tests
      }
    }
  }
  return -1; //File was not found.
}

int closeFileTAA(taa_t table, index_file_t id) {
  id -= 1;
  if(!idIsValid(table, id)) return FAIL;
  table.file[id].vBit = 0; // I can empty that cell
  return SUCCESS;
}

int getFileMode(taa_t table, index_file_t id) {
  id -= 1;
  if(!idIsValid(table, id)) return -1;
  return table.file[id].mode;
}

int getFileDescriptorIndex(taa_t table, index_file_t id) {
  id -= 1;
  if(!idIsValid(table, id)) return -1;
  return table.file[id].descriptorIndex;
}

static int idIsValid(taa_t table, index_file_t id) { // If index is out of range or if cell was already empty we have a fail.
  return (id >= table.size || id < 0 || table.file[id].vBit == 0) ? FAIL : SUCCESS;
}

static index_file_t getNextEmptyPostionTAA(taa_t table) {
  int i = 0;
  while (table.file[i].vBit != 0) { // memory was initiated at 0 with calloc.
    i++;
    if (i == table.size) { // Walked through all the list and didn't find.
      return -1; // FAIL
    }
  }
  return i;
}
