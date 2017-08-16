#ifndef _BLOCK_H
#define _BLOCK_H

#include <stdlib.h>

#include "version.h"
#include "useful.h"

#define MAX_BLOCKS_INDEXER (BLOCK_SIZE / sizeof(int)) - 3
#define BLOCK_DATA_SIZE (sizeof(block_t) - sizeof(block_content_t))

typedef struct block_content_ {
  index_block_t next[MAXVERSIONS];
}block_content_t;

typedef struct block_ {
  char data[BLOCK_SIZE - sizeof(block_content_t)];
  block_content_t metaData;
}block_t;

typedef struct indexer_ {
  int maxRange;
  index_block_t nextIndexer;
  index_block_t emptyBlock;
  index_block_t data[MAX_BLOCKS_INDEXER];
}indexer_t;

/**
 * Initializes a indexer that can index the next MAX_BLOCKS_INDEXER.
 * @method initIndexerBlock
 * @param  blockList        Reference to the first free block after the directories.
 * @param  pos              Position on the memory to store the indexer.
 * @param  limit            The real max number of blocks the indexer may index, for the SO my have less blocks that a index can index.
 * @return                  SUCCESS or FAIL.
 */
int initIndexerBlock(indexer_t *blockList, index_block_t pos, int limit);

/**
 * Sets the indexer at src position to point to the indexer at nxt position as it's next indexer.
 *
 * @param  blockList           Reference to the first free block after the directories.
 * @param  src                 Source index position that will point to nxt.
 * @param  nxt                 Indexer position to be pointed by src.
 */
void setNextIndexerBlock(indexer_t *blockList, index_block_t src, index_block_t nxt);

/**
 * Writes data from the buffer to the file.
 *
 * @param  fs               File's File System identifier.
 * @param  fdId             File's File Descriptor identifier inside the FS.
 * @param  size             Quantity of bytes to be written.
 * @param  buffer           Reference to the first bit to write.
 * @param  version          File's version to be written.
 * @return                  FAIL or file's new size.
 */
uint32_t writeFileContent(index_fs_t fs, index_descriptor_t fdId, uint32_t size, char *buffer, int version);

/**
 * Reads file's content and write's it buffer. Also returns the amount of bytes read.
 * @method readFileContent
 * @param  fs              File's File System identifier.
 * @param  fdId            File's File Descriptor identifier inside the FS.
 * @param  size            Number of bytes to bee read.
 * @param  buffer          Buffer to store data read.
 * @param  version          File's version to be read.
 * @return                 Number of bytes read.
 */
uint32_t readFileContent(index_fs_t fs, index_descriptor_t fdId, uint32_t size, char *buffer, int version);

/**
 * Deletes the content of a file.
 *
 * @param  fs   File's File System identifier.
 * @param  fdId File's File Descriptor identifier inside the FS.
 * @return      SUCCESS OR FAIL.
 */
int deleteFileContentBlock(index_fs_t fs, index_descriptor_t fdId);
#endif
