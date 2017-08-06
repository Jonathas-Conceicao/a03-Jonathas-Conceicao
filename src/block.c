#include <stdio.h>

#include "block.h"
#include "lfs.h"

// static index_block_t nextEmptyPositionBlock(indexer_t *pBL);
static int fileHasIndexerBlock(indexer_t *blockList);

int initIndexerBlock(indexer_t *blockList, index_block_t pos) {
  blockList[pos].nextIndexer = -1; // Indexer has no next indexer yet.
  blockList[pos].emptyBlock = 0; // Set the next empty block as the first block after the index
  block_t *aux = (block_t *) &blockList[1]; // First position after the indexer, i.e. first data block
  for (unsigned int i = 0; i < MAX_BLOCKS_INDEXER; ++i) {
    aux[i].metaData.version[0] = i+1; // Sets the next empty block to be used as the next block afther this one.
  }
  return SUCCESS;
}

void setNextIndexerBlock(indexer_t *blockList, index_block_t src, index_block_t nxt) {
  blockList[src].nextIndexer = nxt;
  return;
}

int deleteFileContentBlock(index_fs_t fs, index_descriptor_t fdId) {
  indexer_t *indexer = getBlockListFS(fs);
  block_t *blockList = (block_t *) &indexer[1]; // Pointer to the first block after the indexer;
  index_block_t *firstIDList = getFirstBlockList(fs, fdId);
  index_block_t auxIndex;
  block_t *auxBlock;
  for (unsigned int i = 0; i < MAXVERSIONS; ++i) {
    auxIndex = firstIDList[i];
    while (auxBlock->metaData.version[i] != 0) {
      auxIndex = auxBlock->metaData.version[i];
      auxBlock = &blockList[auxIndex];
    }
    auxBlock->metaData.version[i] = indexer[0].emptyBlock;
    indexer[0].emptyBlock = auxIndex;
  }
  return SUCCESS;
}

static int fileHasIndexerBlock(indexer_t *blockList) {
  return (blockList[0].nextIndexer != 0) ? SUCCESS : FAIL;
}

// static index_block_t nextEmptyPositionBlock(indexer_t *pBL) {
  // for (unsigned int i = 1; i < MAX_BLOCKS_INDEXER; ++i) {
  //   if (pBL[i].vBit == 0) { // If position is not being used.
  //     return i;
  //   }
  // }
//   return -1;
// }
