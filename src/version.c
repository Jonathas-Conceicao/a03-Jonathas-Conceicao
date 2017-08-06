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
  if (blocos < getNumBlocksMetaData()) return FAIL;
  pFile = fopen(arquivo, "wb+");
  assert(pFile);

  return createFileSystem(arquivo, blocos, pFile);
}


indice_fs_t vopenfs(char * arquivo) {
  index_fs_t pos = checkForFileSystemOnLSF(arquivo);
  if(pos == FAIL) return FAIL;

  openFileSystemOnLSF(pos);

  return pos +1;
}

// TODO
void vclosefs(indice_fs_t handler){
  handler = handler;
}


indice_arquivo_t vopen(indice_fs_t fs, char * nome,  int acesso, int version) {
  version = version;
  charzao_t *name = charToCharzao(nome);
  if (isFileOpenTAA(getFileIndexTAA(name, fs)) == 0) {
    if (acesso == READ || acesso == READ_AND_WRITE) {
      return FAIL;
    }
    if (createFileDescriptorFS(fs, name) == FAIL) return FAIL;
  }
  int id = getFileDescriptorIndexFS(fs, name);
  return openFileTAA(name, fs, acesso, id);
}

int vclose(indice_arquivo_t arquivo) {
  return closeFileTAA(arquivo);
}

uint32_t vread(indice_arquivo_t arquivo, uint32_t tamanho, char *buffer){
  arquivo = arquivo;
  tamanho = tamanho;
  buffer = buffer;
  return (uint32_t)0;
}

int vwrite(indice_arquivo_t arquivo, uint32_t tamanho, char *buffer){
  if (isFileOpenTAA(arquivo) == FAIL) return FAIL; // Can't write if file is not opened.
  if (getFileMode(arquivo) == READ) return FAIL; // Can't write if it's read only.
  index_fs_t fs = getFileFSTAA(arquivo);
  index_descriptor_t fdId = getFileDescriptorIndexTAA(arquivo);
  return writeFileContent(fs, fdId, tamanho, buffer);
}

int vdelete(indice_arquivo_t arquivo){
  index_fs_t fs = getFileFSTAA(arquivo);
  index_descriptor_t fdId = getFileDescriptorIndexTAA(arquivo);
  if (deleteFileContentBlock(fs, fdId) == FAIL) return FAIL; // If we failed to delete the blocks we failed to delete the file itself.
  if (deleteFileDescriptorFS(fs, fdId) == FAIL) return FAIL; // If we failed to delete the file descriptor we failed to delete the file itself.
  // If deleting the file descriptor and the file content was successful can close the file.
  vclose(arquivo);
  return SUCCESS;
}

int vseek(indice_arquivo_t arquivo, uint32_t seek){
  arquivo = arquivo;
  seek = seek;
  return 0;
}

time_t vcreation(indice_arquivo_t arquivo, int versao){
  arquivo = arquivo;
  versao = versao;
  return (time_t)0;
}

time_t vaccessed(indice_arquivo_t arquivo, int versao){
  arquivo = arquivo;
  versao = versao;
  return (time_t)0;
}

time_t vlast_modified(indice_arquivo_t arquivo, int versao){
  arquivo = arquivo;
  versao = versao;
  return (time_t)0;
}
