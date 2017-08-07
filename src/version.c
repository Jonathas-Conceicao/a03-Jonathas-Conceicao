#include <assert.h>

#include "version.h"
#include "useful.h"
#include "block.h"
#include "lfs.h"
#include "taa.h"

int initfs(char * arquivo, int blocos) {
  FILE *pFile = fopen(arquivo, "rb");
  if (pFile != NULL) {
    fclose(pFile);
    return FAIL; // Fails if fs already exists
  }

  index_fs_t pos = checkForFileSystemOnLSF(arquivo);
  if(pos >= 0)
    deleteFileSystemFromLFS(pos);

  if (blocos < getNumBlocksMetaData()) return FAIL;
  pFile = fopen(arquivo, "wb+");
  assert(pFile);

  return createFileSystem(arquivo, blocos, pFile);
}


indice_fs_t vopenfs(char * arquivo) {
  index_fs_t pos = checkForFileSystemOnLSF(arquivo);
  if(pos < 0) return FAIL;
  
  openFileSystemOnLSF(pos);

  return pos +1;
}

// TODO
// This function should
// 1) Close all files on that file system
// 2) Write the information about this FS on disk (its binary file)
// 3) Free this FS allocated memory
// 4) Mark this FS as closed
void vclosefs(indice_fs_t handler){
  checkIndexForFS(handler);
  closeAllFilesFromFSTAA(handler);
  syncToDisk(handler);
  closeFileSystemOnLSF(handler -1);
}


indice_arquivo_t vopen(indice_fs_t fs, char * nome,  int acesso, int version) {
  version = version;
  charzao_t *name = charToCharzao(nome);
  
  int isOnTAA = isFileOpenTAA(getFileIndexTAA(name, fs));

  if (isOnTAA == 0) {
    if (acesso == READ || acesso == READ_AND_WRITE) {
      return FAIL;
    }
    if (createFileDescriptorFS(fs, name) == FAIL) return FAIL;
  }
  int id = getFileDescriptorIndexFS(fs, name);
  indice_arquivo_t pos = openFileTAA(name, fs, acesso, id);

  if(isOnTAA == 0){ //file did not exist
    touchCreation(fs -1, pos -1);
  }else{ //file did exist
    if((acesso == WRITE) || (acesso == READ_AND_WRITE)){
      touchModified(fs -1, pos -1);
    }else{
      touchAccessed(fs -1, pos -1);
    }
  }

  return pos;
}

int vclose(indice_arquivo_t arquivo) {
  return closeFileTAA(arquivo);
}

uint32_t vread(indice_arquivo_t arquivo, uint32_t tamanho, char *buffer){
  touchAccessed(getFileDescriptorIndexTAA(arquivo), arquivo); // THERE IS A PROBLEM HERE
  arquivo = arquivo;
  tamanho = tamanho;
  buffer = buffer;
  return (uint32_t)0;
}

int vwrite(indice_arquivo_t arquivo, uint32_t tamanho, char *buffer){
  touchModified(getFileDescriptorIndexTAA(arquivo), arquivo);
  arquivo = arquivo;
  tamanho = tamanho;
  buffer = buffer;
  return 0;
}

int vdelete(indice_arquivo_t arquivo){
  vclose(arquivo); // Closes the file before doing anything else.
  index_fs_t fs = getFileFSTAA(arquivo);
  index_descriptor_t fdId = getFileDescriptorIndexTAA(arquivo);
  if (deleteFileContentBlock(fs, fdId) == FAIL) return FAIL; // If we failed to delete the blocks we failed to delete the file itself.
  if (deleteFileDescriptorFS(fs, fdId) == FAIL) return FAIL; // If we failed to delete the file descriptor we failed to delete the file itself.
  // If deleting the file descriptor and the file content was successful we are done.
  return SUCCESS;
}

int vseek(indice_arquivo_t arquivo, uint32_t seek){
  arquivo = arquivo;
  seek = seek;
  return 0;
}

time_t vcreation(indice_arquivo_t arquivo, int versao){
  versao = versao;
  return getCreationTime(getFileFSTAA(arquivo), getFileDescriptorIndexTAA(arquivo));
}

time_t vaccessed(indice_arquivo_t arquivo, int versao){
  versao = versao;
  return getAccessedTime(getFileFSTAA(arquivo), getFileDescriptorIndexTAA(arquivo));
}

time_t vlast_modified(indice_arquivo_t arquivo, int versao){
  versao = versao;
  return getModifiedTime(getFileFSTAA(arquivo), getFileDescriptorIndexTAA(arquivo));
}
