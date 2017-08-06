#ifndef _LFS_H
#define _LFS_H

#include "version.h"
#include "useful.h"
#include "block.h"

#define FS_MAX 15

typedef struct file_descriptor_ {
  char vBit; //Indicates rather the descriptor is free.
  charzao_t name[255];
  time_t creation;
  time_t modification;
  time_t access;
  index_block_t firstBlock[MAXVERSIONS];
  uint32_t fileSize;
  int numVersion;
} file_descriptor_t;

typedef struct fs_ {
  char vBit; // Indicates rather the position is being used.
  char open; // Indicates rather the FS is opened.
  char name[255];
  int numBlock;
  FILE *disk;
  void *blockList;
}fs_t;

typedef struct fsList_ {
  fs_t *list;
  int size;
}fsList_t;

/**
 * Initializes the list of file systems.
 *
 * @param  size    Number of fs allowed to be opened.
 * @return         reference to the LFS.
 */
fsList_t *initLFS(int size);

/**
 * Frees (destenta) the allocated memory for the List of File Systems.
 */
void destryLFS();

/**
 * Returns the number of blocks need for the meta data.
 *
 * @return Number of blocks.
 */
int getNumBlocksMetaData();

/**
 * Creates a new file in the FS.
 *
 * @param  fs           Index of file's FS.
 * @param  name         File's name.
 * @return              SUCCESS OR FAIL.
 */
int createFileDescriptorFS(index_fs_t fs, charzao_t *name);

/** TODO: Bretana implenta isso aqui. Tahnks.
 * Deletes a file descriptor from the FS.
 * @param  fs   File's File System.
 * @param  fdId File's file descriptor ID.
 * @return      SUCCESS OR FAIL.
 */
int deleteFileDescriptorFS(index_fs_t fs, index_descriptor_t fdId);

/**
 * Find the index of file's descriptor in the File System.
 * @method getFileDescriptorIndexFS
 * @param  fs                       Index of the file's FS.
 * @param  name                     File's name.
 * @return                          File's descriptor index or FAIL if file was not found.
 */
int getFileDescriptorIndexFS(index_fs_t fs, charzao_t *name);

/**
 * Returns a pointer to the list of first blocks that the file has.
 *
 * @param  fs                Index of the file's FS.
 * @param  fdId              File's descriptor ID.
 * @return                   Reference to file's first block list.
 */
index_block_t *getFirstBlockList(index_fs_t fs, index_descriptor_t fdId);

/** TODO: Bretana implementa isso aqui também. Thanks.
 * Returns the referece for the block list (AFTER the directories) of the File System.
 *
 * @param  fs             Index of the FS.
 * @return                referece to the blocks (first block after the directories).
 */
indexer_t *getBlockListFS(index_fs_t fs);

/** TODO: Bretana Implemente essas funções para lidar com a versão do arquivo.
 * Returns the current number of versions that the file has.
 *
 * @param  fs                Index of file's FS.
 * @param  fdId              Index of file's File Descriptor.
 * @return                   Number of versions.
 */
int getNumVersionFile(index_fs_t fs, index_descriptor_t fdId);

/** TODO: Bretana Implemente essas funções para lidar com a versão do arquivo.
 * Increments file's version.
 *
 * @param  fs                Index of file's FS.
 * @param  fdId              Index of file's File Descriptor.
 * @param  newFirstBlock     File's first block for new version.
 * @return                   SUCCESS or FAIL.
 */
void incNumVersionFile(index_fs_t fs, index_descriptor_t fdId, index_block_t newFirstBlock);

/**
 * Creates the file system based on it's name and returns it's file handler.
 *
 * @param  name           File Systems's name.
 * @param  size           size is the number of blocks for this file system.
 * @param  pFile          pFile it the disk of this file system.
 * @return                FS's handler or FAIL.
 */
index_fs_t createFileSystem(char *name, int size, FILE *pFile);

/**
 * Check for a given file system's name on the LFS
 *
 * @param name            the name of the file system to look for
 * @return                the index of the file system +1 or FAIL
 * */
index_fs_t checkForFileSystemOnLSF(char *name);

/**
 * Mark the file system as closed
 *
 * @param  index          index of the file system to be closed
 */
void closeFileSystemOnLSF(index_fs_t index);

/**
 * Mark the file system as open
 *
 * @param  index          index of the file system to be open
 */
void openFileSystemOnLSF(index_fs_t index);

#endif
