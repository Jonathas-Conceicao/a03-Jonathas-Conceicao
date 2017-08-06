#ifndef _BLOCK_H
#define _BLOCK_H

#include <stdlib.h>

#include "version.h"
#include "useful.h"

#define MAX_BLOCKS_INDEXER (BLOCK_SIZE / sizeof(int)) - 2

typedef struct block_content_ {
  index_block_t version[MAXVERSIONS];
}block_content_t;

typedef struct block_ {
  block_content_t metaData;
  char data[BLOCK_SIZE - sizeof(block_content_t)];
}block_t;

typedef struct indexer_ {
  index_block_t nextIndexer;
  index_block_t emptyBlock;
  index_block_t data[MAX_BLOCKS_INDEXER];
}indexer_t;

/**
 * Initializes a indexer that can index the next MAX_BLOCKS_INDEXER.
 * @method initIndexerBlock
 * @param  blockList        Reference to the first free block after the directories.
 * @param  pos              Position on the memory to store the indexer.
 * @return                  SUCCESS or FAIL.
 */
int initIndexerBlock(indexer_t *blockList, index_block_t pos);

/**
 * Sets the indexer at src position to point to the indexer at nxt position as it's next indexer.
 *
 * @param  blockList           Reference to the first free block after the directories.
 * @param  src                 Source index position that will point to nxt.
 * @param  nxt                 Indexer position to be pointed by src.
 */
void setNextIndexerBlock(indexer_t *blockList, index_block_t src, index_block_t nxt);

/**
 * Deletes the content of a file.
 *
 * @param  fs   File's File System identifier.
 * @param  fdId File's File Descriptor identifier inside the FS.
 * @return      SUCCESS OR FAIL.
 */
int deleteFileContentBlock(index_fs_t fs, index_descriptor_t fdId);
#endif
