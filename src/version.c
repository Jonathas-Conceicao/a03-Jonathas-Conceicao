#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "version.h"
#include "useful.h"
#include "block.h"
#include "lfs.h"
#include "taa.h"

int initfs(char * arquivo, int blocos) {
  if (fopen(arquivo, "rb")) return FAIL; // Fails if fs already exists
  if (blocos < getNumBlocksMetaData()) return FAIL;
  FILE *pFile;
  pFile = fopen(arquivo, "wb+");
  return createFileSystem(arquivo, blocos, pFile);
}


indice_fs_t vopenfs(char * arquivo) {
  return openFileSystem(arquivo);
}


indice_arquivo_t vopen(indice_fs_t fs, char * nome,  int acesso, int version) {
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
