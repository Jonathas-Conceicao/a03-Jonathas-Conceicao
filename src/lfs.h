#ifndef _LFS_H
#define _LFS_H

#include "version.h"

typedef struct fs_ {
  char name[255];
  int numBlock;
  FILE *disk;
  void *blockList;
}fs_t;

#endif
