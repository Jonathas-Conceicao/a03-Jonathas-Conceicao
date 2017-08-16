#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "lfs.h"
#include "taa.h"

fsList_t *pFsList = NULL;

static int getNextEmptyPositionLFS();
static int getNextEmptyPositionFS(index_fs_t fs);

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

int deleteFileDescriptorFS(index_fs_t fs, index_descriptor_t fdId){
  fs -= 1;
  fdId -= 1;
  if((pFsList == NULL) || (fs > pFsList->size)) return FAIL;

  file_descriptor_t *listFD = (file_descriptor_t *)pFsList->list[fs].blockList;
  listFD[fdId].vBit = 0;
  return SUCCESS;
}

int getNumVersionFile(index_fs_t fs, index_descriptor_t fdId){
  if(pFsList == NULL) return FAIL;
  fs -=1;
  fdId -=1;

  file_descriptor_t *desc = (file_descriptor_t *)pFsList->list[fs].blockList;
  return desc[fdId].numVersion;
}

void setNumVersionFile(index_fs_t fs, index_descriptor_t fdId, int version){
  assert(pFsList);
  fs -=1;
  fdId -=1;

  file_descriptor_t *desc = (file_descriptor_t *)pFsList->list[fs].blockList;
  desc[fdId].numVersion = version;
  return;
}

indexer_t *getBlockListFS(index_fs_t fs){
  assert(pFsList);
  fs -= 1;

  // is the initial block list pointer plus the number of file descriptor (one per file) times the size of a file descriptor
  return (pFsList->list[fs].blockList + MAX_DIR * sizeof(file_descriptor_t));

}

index_block_t *getFirstBlockList(index_fs_t fs, index_descriptor_t fdId) {
  assert(pFsList);
  fs -=1;
  fdId -=1;
  file_descriptor_t *listFD = (file_descriptor_t *)pFsList->list[fs].blockList;
  return (listFD[fdId].firstBlock);
}


int getSeekByteFile(index_fs_t fs, index_descriptor_t fdId) {
  fs -=1;
  fdId -=1;
  file_descriptor_t *listFD = (file_descriptor_t *)pFsList->list[fs].blockList;
  return listFD[fdId].seekByte;
}

int setSeekByteFile(index_fs_t fs, index_descriptor_t fdId, uint32_t val) {
  fs -=1;
  fdId -=1;
  file_descriptor_t *listFD = (file_descriptor_t *)pFsList->list[fs].blockList;
  if (val <= listFD[fdId].fileSize) {
    listFD[fdId].seekByte = val;
    return SUCCESS;
  }
  return FAIL;
}

int getSizeFile(index_fs_t fs, index_descriptor_t fdId) {
  fs -= 1;
  fdId -= 1;
  file_descriptor_t *listFD = (file_descriptor_t *)pFsList->list[fs].blockList;
  return listFD[fdId].fileSize;
}

int setSizeFile(index_fs_t fs, index_descriptor_t fdId, uint32_t newSize) {
  fs -= 1;
  fdId -= 1;
  file_descriptor_t *listFD = (file_descriptor_t *)pFsList->list[fs].blockList;
  listFD[fdId].fileSize = newSize;
  return SUCCESS;
}

int getNumBlocksMetaData() {
  // (File Descriptor Size x 256 entrys) / block size + 2 blocks (FAT and At least one data block)
  return ( ( ((sizeof(file_descriptor_t) * MAX_DIR)) / (BLOCK_SIZE) ) + 2 );
}

index_fs_t createFileSystem(char *name, int size, FILE *pFile){
  if (pFsList == NULL) initLFS(FS_MAX); // if the list os fs is NULL creates it

  int pos = getNextEmptyPositionLFS();
  if (pos < 0) return FAIL;

  pFsList->list[pos].vBit       = 1;
  pFsList->list[pos].open       = 0;
  strcpy(  pFsList->list[pos].name, name);
  pFsList->list[pos].numBlock   = size;
  pFsList->list[pos].disk       = pFile;
  pFsList->list[pos].blockList  = calloc(size, BLOCK_SIZE);
  initIndexerBlock(getBlockListFS(pos+1), 0, size - (getNumBlocksMetaData() - 1)); // Initializes the first indexer.

  //fprintf(stderr, "\n\nFile system \"%s\" in position \"%d\"\n\n", pFsList->list[pos].name, pos);

  return pos+1;
}

int createFileDescriptorFS(index_fs_t fs, charzao_t *name) {
  fs -= 1;
  int pos = getNextEmptyPositionFS(fs);
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

static int getNextEmptyPositionFS(index_fs_t fs) {
  file_descriptor_t *pFdList = pFsList->list[fs].blockList;
  for (int i = 0; i < MAX_DIR; ++i) {
    if (pFdList[i].vBit == 0) {
      return i;
    }
  }
  return -1;
}

static int getNextEmptyPositionLFS() {
  for (int i = 0; i < pFsList->size; ++i) {
    if (pFsList->list[i].vBit == 0) {
      return i;
    }
  }
  return -1;
}

index_fs_t checkForFileSystemOnLSF(char *name) {
  if(pFsList == NULL) return -1;
  for (int i = 0; i < pFsList->size; ++i) {
    if(strcmp(pFsList->list[i].name, name) == 0)
      return i;
  }
  return -1;
}

void deleteFileSystemFromLFS(index_fs_t index){
  checkIndexForFS(index);
  if(pFsList != NULL)
    pFsList->list[index].vBit = 0;
}

void closeFileSystemOnLSF(index_fs_t index){
  checkIndexForFS(index);
  if(pFsList != NULL)
    pFsList->list[index].open = 0;
}

void openFileSystemOnLSF(index_fs_t index){
  checkIndexForFS(index);
  if(pFsList != NULL)
    pFsList->list[index].open = 1;
}

void syncToDisk(index_fs_t index){
  checkIndexForFS(index);
  if((pFsList == NULL) || (pFsList->list[index].blockList == NULL)) return;

  size_t result = fwrite(pFsList->list[index].blockList, BLOCK_SIZE, pFsList->list[index].numBlock, pFsList->list[index].disk);
  // printf("DEBUG: Writing to %s\n",pFsList->list[index].name); BUG: Aparentemente todos os fclosevs estão escrevendo no mesmo arquivo TODO: Bretana, arruma isso.
  if(result != (unsigned)pFsList->list[index].numBlock){
    fprintf(stderr, "\nThere was some problem writen on the file Expected write %lu, but wrote %lu.\n", result, (size_t)pFsList->list[index].numBlock);
    assert(0);
  }
}

void checkIndexForFS(index_fs_t handler){
  if((handler < 0) || (handler > pFsList->size) || (handler > FS_MAX)){
    fprintf(stderr, "\nInvalid file handler (%d) should be between 0 and %d", handler, FS_MAX);
    assert(0);
  }
}

time_t getCreationTime(index_fs_t fs, index_descriptor_t file){
  checkIndexForFS(file);
  if(pFsList == NULL) return (time_t)0;

  file_descriptor_t *listDesc = (file_descriptor_t *)pFsList->list[fs].blockList;
  return listDesc[file].creation;
}

time_t getAccessedTime(index_fs_t fs, index_descriptor_t file){
  checkIndexForFS(file);
  if(pFsList == NULL) return (time_t)0;

  file_descriptor_t *listDesc = (file_descriptor_t *)pFsList->list[fs].blockList;
  return listDesc[file].access;
}

time_t getModifiedTime(index_fs_t fs, index_descriptor_t file){
  checkIndexForFS(file);
  if(pFsList == NULL) return (time_t)0;

  file_descriptor_t *listDesc = (file_descriptor_t *)pFsList->list[fs].blockList;
  return listDesc[file].modification;
}

void touchCreation(index_fs_t fs, index_descriptor_t file){
  if(fs == -1) return;
  checkIndexForFS(file);
  if(pFsList == NULL) return;

  file_descriptor_t *listDesc = (file_descriptor_t *)pFsList->list[fs].blockList;
  if(listDesc != NULL)
    listDesc[file].creation = clock();
  touchModified(fs, file);
}

void touchAccessed(index_fs_t fs, index_descriptor_t file){
  if(fs == -1) return;
  checkIndexForFS(file);
  if(pFsList == NULL) return;

  file_descriptor_t *listDesc = (file_descriptor_t *)pFsList->list[fs].blockList;
  if(listDesc != NULL)
    listDesc[file].access = clock();
}

void touchModified(index_fs_t fs, index_descriptor_t file){
  if(fs == -1) return;
  checkIndexForFS(file);
  if(pFsList == NULL) return;

  file_descriptor_t *listDesc = (file_descriptor_t *)pFsList->list[fs].blockList;
  if(listDesc != NULL)
    listDesc[file].modification = clock();
  touchAccessed(fs, file);
}
