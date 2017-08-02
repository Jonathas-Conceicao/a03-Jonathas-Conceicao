#ifndef _TAA_H
#define _TAA_H

#include "version.h"
#include "block.h"
#include <time.h>
#include <stdint.h>

typedef char charzao_t;
typedef index_fs_t indice_fs_t;

typedef struct file_ {
  charzao_t name[255];
  index_fs_t fsIndex;
  int mode;
  int descriptorIndex;
}file_h;

typedef struct file_descriptor_ {
  charzao name[255];
  time_t creation;
  time_t modification;
  time_t access;
  index_block_t firstBlock[MAXVERSIONS];
  uint32_t fileSize;
  int numVersion;
} file_descriptor_t;

#endif
