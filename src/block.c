#include <stdio.h>

#include "block.h"
#include "lfs.h"

static void addEmptyBlock(indexer_t *pIndexer, index_block_t id);
static int fileHasIndexerBlock(indexer_t *blockList);

int initIndexerBlock(indexer_t *blockList, index_block_t pos) {
  blockList[pos].nextIndexer = -1; // Indexer has no next indexer yet.
  blockList[pos].emptyBlock = 0; // Set the next empty block as the first block after the index
  block_t *aux = (block_t *) &blockList[1]; // First position after the indexer, i.e. first data block
  for (unsigned int i = 0; i < MAX_BLOCKS_INDEXER - 1; ++i) {
    aux[i].metaData.next[0] = i+1; // Sets the next empty block to be used as the next block after this one.
  }
  return SUCCESS;
}

void setNextIndexerBlock(indexer_t *blockList, index_block_t src, index_block_t nxt) {
  blockList[src].nextIndexer = nxt;
  return;
}

int deleteFileContentBlock(index_fs_t fs, index_descriptor_t fdId) {
  indexer_t *indexer = getBlockListFS(fs);
  index_block_t *firstIDList = getFirstBlockList(fs, fdId);
  block_t *blockList = (block_t *) indexer; // Pointer to the first block after the indexer.
  index_block_t thisIndex;
  index_block_t nextIndex;
  for (unsigned int i = 0; firstIDList[i] != 0 && i < MAXVERSIONS; ++i) { // Stops of first index of version is 0 or after all versions.
    thisIndex = firstIDList[i];;
    nextIndex = blockList[thisIndex].metaData.next[i];
    while (nextIndex != 0) {
      addEmptyBlock(indexer, thisIndex); // Set the current block to be an empty block.
      thisIndex = nextIndex;
      nextIndex = blockList[thisIndex].metaData.next[i];
    }
  }
  return SUCCESS;
}

static int fileHasIndexerBlock(indexer_t *blockList) {
  return (blockList[0].nextIndexer != 0) ? SUCCESS : FAIL;
}

static void addEmptyBlock(indexer_t *pIndexer, index_block_t id) {
  block_t *blockList = (block_t *) pIndexer;
  index_block_t auxIndex = pIndexer[0].emptyBlock; // Index of first free block.
  block_t *auxBlock;
  do {
    if (auxIndex == id) return; // Return if id is already an empty block.
    auxBlock = &blockList[auxIndex]; // Points to the next free block.
    auxIndex = auxBlock->metaData.next[0]; // Sets auxIndex to the next empty block.
  } while(auxIndex != 0);
  auxBlock->metaData.next[0] = id;
  auxBlock = &blockList[id];
  auxBlock->metaData.next[0] = 0;
  return;
}
