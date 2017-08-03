#ifndef _TAA_H
#define _TAA_H

#include "version.h"
#include "useful.h"
#include "block.h"
#include "lfs.h"
#include <time.h>
#include <stdint.h>

typedef struct file_ {
  char vBit;
  charzao_t name[255];
  index_fs_t fsIndex;
  int mode;
  int descriptorIndex;
}file_t;

typedef struct file_descriptor_ {
  charzao_t name[255];
  time_t creation;
  time_t modification;
  time_t access;
  index_block_t firstBlock[MAXVERSIONS];
  uint32_t fileSize;
  int numVersion;
} file_descriptor_t;

// typedef file_t* taa_t;
typedef struct taa_ {
  file_t *file;
  int     size;
}taa_t;

/**
 * Initialize the Table of Open Files.
 *
 * @param  size    Max number of files in TAA.
 * @return         Reference to the TAA.
 */
taa_t *initTAA(int size);

/**
 * Adds a file to the TAA returning the index in the TAA's list.
 * If file is already opened the mode is updated.
 *
 * @param  table             System's table to add the file.
 * @param  name              File's name.
 * @param  fsIndex           Index of files's FS.
 * @param  mode              READ, WRITE or READ_AND_WRITE
 * @param  descriptorIndex   Index of file's descriptor in the directory.
 * @return                   File's index, or -1 if failed.
 */
index_file_t openFileTAA(taa_t talbe, charzao_t *name, index_fs_t fsIndex, int mode, int descriptorIndex);

/**
 * Remove a opened file from TAA.
 * @method closeFileTAA
 * @param  table        System's table.
 * @param  id           index of file.
 * @return              SUCCESS OR FAIL.
 */
int closeFileTAA(taa_t table, index_file_t id);

/**
 * Finds file in TAA and returns it's index.
 *
 * @param  table        System's TAA.
 * @param  name         File's name.
 * @param  fsIndex      Index of files's FS.
 * @return              File's index, or -1 if failed.
 */
index_file_t getFileIndex(taa_t table, charzao_t *name, index_fs_t fsIndex);

/**
 * Finds file's mode.
 *
 * @param  table       System's TAA.
 * @param  id          File's index in table.
 * @return             file's mode, -1 if position is invalid.
 */
int getFileMode(taa_t table, index_file_t id);

/**
 * Finds file's descriptor's index.
 *
 * @param  table                  System's TAA.
 * @param  id                     File's index in table.
 * @return                        File's descriptor's index, or -1 if falied.
 */
int getFileDescriptorIndex(taa_t table, index_file_t id);
#endif
