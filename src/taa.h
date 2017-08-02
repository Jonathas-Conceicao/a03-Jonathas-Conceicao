#ifndef _TAA_H
#define _TAA_H

#include "version.h"
#include "useful.h"
#include "block.h"
#include "lfs.h"
#include <time.h>
#include <stdint.h>

typedef char charzao_t;

typedef struct file_ {
  charzao_t name[255];
  index_fs_t fsIndex;
  int mode;
  int descriptorIndex;
}file_t;

typedef struct file_descriptor_ {
  charzao name[255];
  time_t creation;
  time_t modification;
  time_t access;
  index_block_t firstBlock[MAXVERSIONS];
  uint32_t fileSize;
  int numVersion;
} file_descriptor_t;

typedef file_t* taa_t;

/**
 * Initialize the Table of Open Files.
 *
 * @param  size    Max number of files in TAA.
 * @return         Reference to the TAA.
 */
taa_t initTAA(int size);

/**
 * Adds a file to the TAA returning the index of its descriptor.
 *
 * @param  name              File's name.
 * @param  fsIndex           Index of files's FS.
 * @param  mode              READ, WRITE or READ_AND_WRITE
 * @param  descriptorIndex   Index of file's descriptor in the directory.
 * @return               Indice para o descritor do arquivo ou FALHA.
 */
int openFileTAA(charzao_t *name, index_fs_t fsIndex, int mode, int descriptorIndex);
#endif
