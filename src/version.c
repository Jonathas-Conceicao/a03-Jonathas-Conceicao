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

void vclosefs(indice_fs_t handler){
  checkIndexForFS(handler);
  closeAllFilesFromFSTAA(handler);
  syncToDisk(handler);
  closeFileSystemOnLSF(handler -1);
}


indice_arquivo_t vopen(indice_fs_t fs, char * nome,  int acesso, int version) {
  charzao_t *name = charToCharzao(nome);
  int isOnTAA = isFileOpenTAA(getFileIndexTAA(name, fs));
  //if (isFileOpenTAA(getFileIndexTAA(name, fs)) == 1) return FAIL; // FAIL is file is already open.
  if (isOnTAA == 1) return FAIL; // FAIL is file is already open.
  int id = getFileDescriptorIndexFS(fs, name);
  if (id == FALHA) { // If failed to get file descriptor then file doesn't exists yet.
    if (version != 0) return FAIL; // Can't open another version but 0 if file doesn't even exists.
    if (acesso != LEITURA) { // If access is WRITE or WRITE_AND_READ we can create the file.
      if (createFileDescriptorFS(fs, name) == FAIL) return FAIL; // Create a file descriptor.
      id = getFileDescriptorIndexFS(fs, name);
    } else { // If file READ only.
      return FAIL; // FAIL because file still doesn't exists.
    }
  } else if (version > getNumVersionFileDescriptor(fs, id)) {
    if (version - 1 == getNumVersionFileDescriptor(fs, id) && version < MAXVERSIONS && acesso != LEITURA) {
      setNumVersionFileDescriptor(fs, id, version); // Set the new number of versions.
    }
  }

  index_file_t pos =  openFileTAA(name, fs, acesso, version, id);
  if(isOnTAA == 0){ // file did not exist
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
  touchAccessed(getFileDescriptorIndexTAA(arquivo), arquivo);
  if (isFileOpenTAA(arquivo) == FAIL) return FAIL; // Can't read if file is not opened.
  if (getFileMode(arquivo) == WRITE) return FAIL; // Can't read if it's write only.
  index_fs_t fs = getFileFSTAA(arquivo);
  index_descriptor_t fdId = getFileDescriptorIndexTAA(arquivo);
  return readFileContent(fs, fdId, tamanho, buffer, getNumVersionFile(arquivo));
}

int vwrite(indice_arquivo_t arquivo, uint32_t tamanho, char *buffer) {
  touchModified(getFileDescriptorIndexTAA(arquivo), arquivo);
  if (isFileOpenTAA(arquivo) == FAIL) return FAIL; // Can't write if file is not opened.
  if (getFileMode(arquivo) == READ) return FAIL; // Can't write if it's read only.
  index_fs_t fs = getFileFSTAA(arquivo);
  index_descriptor_t fdId = getFileDescriptorIndexTAA(arquivo);
  uint32_t newSize = writeFileContent(fs, fdId, tamanho, buffer, getNumVersionFile(arquivo));
  return setSizeFile(fs, fdId, newSize);
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

int vseek(indice_arquivo_t arquivo, uint32_t seek){
  index_fs_t fs = getFileFSTAA(arquivo);
  index_descriptor_t fdId = getFileDescriptorIndexTAA(arquivo);
  return setSeekByteFile(fs, fdId, seek);
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
