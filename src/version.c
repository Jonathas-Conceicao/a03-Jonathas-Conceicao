#include <assert.h>

#include "version.h"
#include "useful.h"
#include "block.h"
#include "lfs.h"
#include "taa.h"

int initfs(char * arquivo, int blocos) {
  if (fopen(arquivo, "rb")) return FAIL; // Fails if fs already exists

  if(checkForFileSystemOnLSF(arquivo) == FAIL)

  if (blocos < getNumBlocksMetaData()) return FAIL;
  FILE *pFile;
  pFile = fopen(arquivo, "wb+");
  assert(pFile);

  void *blockList = calloc(blocos, BLOCK_SIZE);
  assert(blockList);

  fwrite(blockList, BLOCK_SIZE, blocos, pFile);

  free(blockList);
  fclose(pFile);

  return SUCCESS;
}


indice_fs_t vopenfs(char * arquivo) {
  assert(arquivo);
  FILE *disk = fopen(arquivo, "wb+");
  assert(disk);

  

  //index_fs_t createFileSystem(char *name, int size, FILE *pFile){
  return openFileSystem(arquivo);
}

void vclosefs(indice_fs_t handler){
  handler = handler;
}


indice_arquivo_t vopen(indice_fs_t fs, char * nome,  int acesso, int version) {
  version = version;
  charzao_t *name = charToCharzao(nome);
  if (isFileOpenTAA(getFileIndexTAA(name, fs)) == 0) {
    if (acesso == READ) {
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
