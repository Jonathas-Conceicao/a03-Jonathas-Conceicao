#include <stdio.h>
#include <math.h>

#include "block.h"
#include "lfs.h"

static void addEmptyBlock(indexer_t *pIndexer, index_block_t id);
static int fileHasIndexerBlock(indexer_t *blockList);
static uint32_t countFreeBlocksOnIndexerBlock(indexer_t *pIndexer);
static void printBlock(block_t *);
static int max(int a, int b);

static int max(int a, int b) {
  return (a >= b) ? a : b;
}

static void printBlock(block_t *b) {
  fprintf(stderr, " --- PRINTING BLOCK ---\n");
  fprintf(stderr, "Block's pointer: %p\n", b);
  fprintf(stderr, "Block's version indexes:\n");
  for (unsigned i = 0; i < MAXVERSIONS; ++i) {
    fprintf(stderr, "%i: %i\n", i+1, b->metaData.next[i]);
  }
  fprintf(stderr, "Block's content:\n------\n%s\n------\n", b->data);
  return;
}

int initIndexerBlock(indexer_t *blockList, index_block_t pos, int limit) {
  blockList[pos].nextIndexer = -1; // Indexer has no next indexer yet.
  blockList[pos].emptyBlock = 1; // Set the next empty block as the first block after the index
  blockList[pos].maxRange = max(MAX_BLOCKS_INDEXER, limit); // Set the maxRange of the indexer, for the system my have less blocks then it can index.
  block_t *aux = (block_t *) blockList; // First position after the indexer, i.e. first data block
  for (int i = 1; i < limit; ++i) {
    aux[i].metaData.next[0] = i+1; // Sets the next empty block to be used as the next block after this one.
  }
  return SUCCESS;
}

void setNextIndexerBlock(indexer_t *blockList, index_block_t src, index_block_t nxt) {
  blockList[src].nextIndexer = nxt;
  return;
}

int writeFileContent(index_fs_t fs, index_descriptor_t fdId, uint32_t size, char *buffer) {
  uint32_t numBlocks = ceil ((float) size / (float) BLOCK_DATA_SIZE); // Count and round up the number of blocks needed.
  indexer_t *indexer = getBlockListFS(fs);
  uint32_t freeBlocks = countFreeBlocksOnIndexerBlock(indexer);
  // printf("DEBUG: Num of free blocks %i\n", freeBlocks);
  if (numBlocks > freeBlocks) {
    return FAIL; //TODO FIX THIS.
  }
  return FAIL;
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

static uint32_t countFreeBlocksOnIndexerBlock(indexer_t *pIndexer) {
  index_block_t freeIndex = pIndexer[0].emptyBlock;
  block_t *freeBlock;
  uint32_t numFreeBlocks = 0;
  while (freeIndex != 0) { // While we still have free blocks to check.
    freeBlock = (block_t *) &pIndexer[freeIndex]; // Get reference to next free block;
    freeIndex = freeBlock->metaData.next[0]; // Check if theres another free block.
    numFreeBlocks += 1; // Count the free block found.
  }
  return numFreeBlocks;
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
