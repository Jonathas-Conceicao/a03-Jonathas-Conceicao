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
  charzao_t *name = charToCharzao(nome);
  if (isFileOpenTAA(getFileIndexTAA(name, fs)) == 1) return FAIL; // FAIL is file is already open.
  int id = getFileDescriptorIndexFS(fs, name);
  if (id == FALHA) { // If failed to get file descriptor then file doesn't exists yet.
    if (acesso == WRITE) { // If access is WRITE only we can create the file.
      if (createFileDescriptorFS(fs, name) == FAIL) return FAIL; // Create a file descriptor.
      id = getFileDescriptorIndexFS(fs, name);
    } else { // If file is for READ or READ_AND_WRITE.
      return FAIL; // FAIL because file still doesn't exists.
    }
  }
  setNumVersionFile(fs, id, version); // Set the opened version.
  return openFileTAA(name, fs, acesso, id);
}

int vclose(indice_arquivo_t arquivo) {
  return closeFileTAA(arquivo);
}

uint32_t vread(indice_arquivo_t arquivo, uint32_t tamanho, char *buffer){
  if (isFileOpenTAA(arquivo) == FAIL) return FAIL; // Can't read if file is not opened.
  if (getFileMode(arquivo) == WRITE) return FAIL; // Can't read if it's write only.
  index_fs_t fs = getFileFSTAA(arquivo);
  index_descriptor_t fdId = getFileDescriptorIndexTAA(arquivo);
  return readFileContent(fs, fdId, tamanho, buffer);
}

int vwrite(indice_arquivo_t arquivo, uint32_t tamanho, char *buffer) {
  if (isFileOpenTAA(arquivo) == FAIL) return FAIL; // Can't write if file is not opened.
  if (getFileMode(arquivo) == READ) return FAIL; // Can't write if it's read only.
  index_fs_t fs = getFileFSTAA(arquivo);
  index_descriptor_t fdId = getFileDescriptorIndexTAA(arquivo);
  return writeFileContent(fs, fdId, tamanho, buffer);
}

int vdelete(indice_arquivo_t arquivo) {
  index_fs_t fs = getFileFSTAA(arquivo);
  index_descriptor_t fdId = getFileDescriptorIndexTAA(arquivo);
  if (fdId < 0) return FAIL; // Fails if the file descriptor was not found.
  if (deleteFileContentBlock(fs, fdId) == FAIL) return FAIL; // If we failed to delete the blocks we failed to delete the file itself.
  if (deleteFileDescriptorFS(fs, fdId) == FAIL) return FAIL; // If we failed to delete the file descriptor we failed to delete the file itself.
  // If deleting the file descriptor and the file content was successful can close the file.
  vclose(arquivo);
  return SUCCESS;
}

//TODO: Nada implementado ainda. O teste do Pila não faz sentido. Open para leitura e escrita deve criar o arquivo?
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
