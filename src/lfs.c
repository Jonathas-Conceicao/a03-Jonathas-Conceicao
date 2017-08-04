#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "lfs.h"
#include "taa.h"

fsList_t *pFsList = NULL;

static int getNextEmptyPostionLFS();
static int getNextEmptyPostionFS(index_fs_t fs);

fsList_t *initLFS(int size) {
  if (pFsList == NULL) {
    pFsList = malloc(sizeof(fsList_t));
    assert(pFsList);
    pFsList->list = calloc(size, sizeof(fs_t));
    assert(pFsList->list);
    pFsList->size = size;
  }
  return pFsList;
}

void destryLFS() {
  free(pFsList->list);
  free(pFsList);
}

int getNumBlocksMetaData() {
  // (File Descriptor Size x 256 entrys) / block size + 2 blocks (FAT and At least one data block)
  return ( ( ((sizeof(file_descriptor_t) * 256)) / (BLOCK_SIZE) ) + 2 );
}

int createFileSystem(char *name, int size, FILE *pFile) {
  if (pFsList == NULL) initLFS(FS_MAX);
  int pos = getNextEmptyPostionLFS();
  if (pos < 0) return FAIL;
  pFsList->list[pos].vBit = 1;
  pFsList->list[pos].open = 0; // FS is initialized but not open.
  strcpy(  pFsList->list[pos].name, name);
  pFsList->list[pos].numBlock = size;
  pFsList->list[pos].disk = pFile;
  pFsList->list[pos].blockList = calloc(size, BLOCK_SIZE);
  return SUCCESS;
}

index_fs_t openFileSystem(char *name) {
  for (int i = 0; i < pFsList->size; ++i) {
    if(strcmp(pFsList->list[i].name, name) == 0) {
      pFsList->list[i].open = 1; // Makes FS as open.
      return (i+1);
    }
  }
  return FAIL;
}

int createFileDescriptorFS(index_fs_t fs, charzao_t *name) {
  fs -= 1;
  int pos = getNextEmptyPostionFS(fs);
  if (pos < 0) {
    return FAIL;
  }
  file_descriptor_t *pFdList = pFsList->list[fs].blockList;
  pFdList[pos].vBit = 1;
  copyCharzao(pFdList[pos].name, name);
  // Time not updated yet as the file has not really been written.
  pFdList[pos].numVersion = 0;
  return SUCCESS;
}

int getFileDescriptorIndexFS(index_fs_t fs, charzao_t *name) {
  fs -= 1;
  file_descriptor_t *pFdList = pFsList->list[fs].blockList;
  for (int i = 0; i < MAX_DIR; ++i) {
    if(pFdList[i].vBit == 1 && compareCharzao(pFdList[i].name, name) == 0) {
      return (i+1);
    }
  }
  return FAIL;
}

static int getNextEmptyPostionFS(index_fs_t fs) {
  file_descriptor_t *pFdList = pFsList->list[fs].blockList;
  for (int i = 0; i < MAX_DIR; ++i) {
    if (pFdList[i].vBit == 0) {
      return i;
    }
  }
  return -1;
}

static int getNextEmptyPostionLFS() {
  for (int i = 0; i < pFsList->size; ++i) {
    if (pFsList->list[i].vBit == 0) {
      return i;
    }
  }
  return -1;
}
