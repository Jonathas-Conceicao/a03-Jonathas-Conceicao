#ifndef _BLOCK_H
#define _BLOCK_H

#include <stdlib.h>

#include "version.h"
#include "useful.h"

#define LIMIT (BLOCK_SIZE / sizeof(int)) - 2

typedef struct block_ {
  int indexVersion[MAXVERSIONS];
}block_t;

typedef struct indexer_ {
  index_block_t nextIndexer;
  index_block_t emptyBlock;
  index_block_t data[LIMIT];
}indexer_t;

/**
 * Deletes the content of a file.
 *
 * @param  fs   File's File System identifier.
 * @param  fdId File's File Descriptor identifier inside the FS.
 * @return      SUCCESS OR FAIL.
 */
int deleteFileContentBlock(index_fs_t fs, index_descriptor_t fdId);
#endif
