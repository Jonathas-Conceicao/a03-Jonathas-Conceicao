#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "block.h"
#include "lfs.h"

#ifndef DEBUGBLOCK
#define DEBUGBLOCK 0
#endif

static void addEmptyBlock(indexer_t *pIndexer, index_block_t id);
// static int fileHasIndexerBlock(indexer_t *blockList);
static uint32_t countFreeBlocksOnIndexerBlock(indexer_t *pIndexer);
static index_block_t removeEmptyBlock(indexer_t *pIndexer);
static void printBlock(block_t *, int);

static int min(int a, int b) {
  return (a >= b) ? b : a;
}
static int max(int a, int b) {
  return (a >= b) ? a : b;
}

static void printBlock(block_t *b, int printContent) {
  fprintf(stderr, " --- PRINTING BLOCK ---\n");
  fprintf(stderr, "Block's pointer: %p\n", b);
  fprintf(stderr, "Block's version indexes:\n");
  for (unsigned i = 0; i < MAXVERSIONS; ++i) {
    fprintf(stderr, "%i: %i\n", i+1, b->metaData.next[i]);
  }
  if (printContent) {
    fprintf(stderr, "Block's content:\n------\n");
    for (long unsigned i = 0; i < BLOCK_DATA_SIZE; ++i) {
      if (!b->data[i]) fprintf(stderr, "-");
      else fprintf(stderr, "%c",b->data[i]);
    }
    fprintf(stderr, "\n------\n");
  }
  return;
}

int initIndexerBlock(indexer_t *blockList, index_block_t pos, int limit) {
  blockList[pos].nextIndexer = -1; // Indexer has no next indexer yet.
  blockList[pos].emptyBlock = 1; // Set the next empty block as the first block after the index
  blockList[pos].maxRange = max(MAX_BLOCKS_INDEXER, limit); // Set the maxRange of the indexer, for the system my have less blocks then it can index.
  block_t *aux = (block_t *) blockList; // First position after the indexer, i.e. first data block
  for (int i = 1; i < limit; ++i) {
    aux[i].metaData.next[0] = i+1; // Sets the next empty block to be used as the next block after this one.
    // printBlock(&aux[i], 0);
  }
  return SUCCESS;
}

void setNextIndexerBlock(indexer_t *blockList, index_block_t src, index_block_t nxt) {
  blockList[src].nextIndexer = nxt;
  return;
}

uint32_t readFileContent(index_fs_t fs, index_descriptor_t fdId, uint32_t size, char *buffer, int version) {
  indexer_t *indexer = getBlockListFS(fs);
  block_t *blockList = (block_t *) indexer;
  index_block_t *firstIDList = getFirstBlockList(fs, fdId);
  // int version = getNumVersionFile(fs, fdId);
  index_block_t eye = firstIDList[version];
  uint32_t read = 0;
  uint32_t toRead;

  int seekByte = getSeekByteFile(fs, fdId); // Offset insde the block.
  int seekBlock = seekByte / BLOCK_DATA_SIZE; // Blocks of Offset.
  seekByte = seekByte % BLOCK_DATA_SIZE; // Offset inside the block.

  for (int i = 0; i < seekBlock; ++i) { // Walk the seekBlock offset in blocks
    eye = blockList[eye].metaData.next[version];
  }

  toRead = min(BLOCK_DATA_SIZE, size);
  while (toRead > 0) {
    if (eye == 0) break; // Finish before reading everthing if next index is out of bound.
    strncpy((buffer + read), (const char *) (blockList[eye].data + seekByte), toRead);
    seekByte = 0; // Set the byte offset after it's used once.
    read += toRead;
    toRead = min(BLOCK_DATA_SIZE, size - read); // New size to read from block next block.
    eye = blockList[eye].metaData.next[version]; // Gets next pointer.
  }
  return read;
}

uint32_t writeFileContent(index_fs_t fs, index_descriptor_t fdId, uint32_t size, char *buffer, int version) {
  indexer_t *indexer = getBlockListFS(fs);
  block_t *blockList = (block_t *) indexer;

  uint32_t numBlocks = ceil ((float) size / (float) BLOCK_DATA_SIZE); // Count and round up the number of blocks needed.
  uint32_t freeBlocks = countFreeBlocksOnIndexerBlock(indexer);
  if (numBlocks > freeBlocks) return FAIL; // Can't write on this FAT when it's full.

  index_block_t *firstIDList = getFirstBlockList(fs, fdId);
  // int version = getNumVersionFile(fs, fdId);

  index_block_t writingBlock;
  index_block_t *prevBlock; // Hold the position for the block that will point to the written block.
  int seekByte = getSeekByteFile(fs, fdId);
  int seekBlock = seekByte / BLOCK_DATA_SIZE; // Blocks of Offset.
  seekByte = seekByte % BLOCK_DATA_SIZE; // Offset inside the block.
  uint32_t fSize = getSizeFile(fs, fdId); // New size to be returned.

  prevBlock = &firstIDList[version];
  if (seekBlock > 0) { // If we have a seek in block we won't write at the end of the file.
    for (int i = 0; i < seekBlock; ++i) {
      writingBlock = *prevBlock;
      prevBlock = &blockList[*prevBlock].metaData.next[version];
    }
  } else { // Else we go to the last block.
    while (*prevBlock != 0) {
      writingBlock = *prevBlock;
      prevBlock = &blockList[*prevBlock].metaData.next[version];
    }
  }

  int written = 0;
  int toWrite;
  for (unsigned i = 0; i < numBlocks; ++i) {
    toWrite = min(BLOCK_DATA_SIZE - seekByte, size - written);
    if (!seekByte) { // If seekByte is zero we get a newBlock.
      writingBlock = removeEmptyBlock(indexer); // Gets an empty block.
    }
    *prevBlock = writingBlock;
    assert(writingBlock != 0 && "Can't write on block 0");
    strncpy((blockList[writingBlock].data + seekByte), (buffer + written), toWrite);
    if (seekByte) { // If I had a seekByte I can set it to zero after the first write. Because it's only used once.
      seekByte = 0;
    } else { // If I did not had a seekByte then I've written a new block and now we need to link it.
      prevBlock = &blockList[writingBlock].metaData.next[version];
    }
    seekByte = 0; // Only uses the seekByte for the first write.
    written += toWrite;
  }
  blockList[writingBlock].metaData.next[version] = 0; // Set the last block as last block of the file.
  if (DEBUGBLOCK == 1) printBlock(&blockList[firstIDList[version]], 1);

  return fSize + size;
}

int deleteFileContentBlock(index_fs_t fs, index_descriptor_t fdId) {
  indexer_t *indexer = getBlockListFS(fs);
  index_block_t *firstIDList = getFirstBlockList(fs, fdId);
  block_t *blockList = (block_t *) indexer; // Pointer to the first block after the indexer.
  index_block_t thisIndex;
  index_block_t nextIndex;
  for (unsigned int i = 0; firstIDList[i] != 0 && i < MAXVERSIONS; ++i) { // Stops of first index of version is 0 or after all versions.
    thisIndex = firstIDList[i];
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

// static int fileHasIndexerBlock(indexer_t *blockList) {
//   return (blockList[0].nextIndexer != 0) ? SUCCESS : FAIL;
// }

static index_block_t removeEmptyBlock(indexer_t *pIndexer) {
  block_t *blockList = (block_t *) pIndexer;
  index_block_t auxIndex = pIndexer[0].emptyBlock; // Index of first free block.
  block_t *auxBlock = &blockList[auxIndex]; // Reference to first block.
  index_block_t nextIndex = auxBlock->metaData.next[0]; // Index of the second free block.
  pIndexer[0].emptyBlock = nextIndex; // The second free block is now the first free block.
  return auxIndex; // The old free block is returned.
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
