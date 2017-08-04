#ifndef _TAA_H
#define _TAA_H

#include <time.h>
#include <stdint.h>

#include "version.h"
#include "useful.h"
#include "block.h"
#include "lfs.h"

#define MAX_FILES 500 // Number max of files that can be opened in the SO

typedef struct file_ {
  char vBit;
  charzao_t name[255];
  index_fs_t fsIndex;
  int mode;
  index_descriptor_t descriptorIndex;
}file_t;

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
 * Frees (destenta) the allocated memory for the Table of Open Files.
 */
void destryTAA();

/**
 * Adds a file to the TAA returning the index in the TAA's list.
 * If file is already opened the mode is updated.
 *
 * @param  name              File's name.
 * @param  fsIndex           Index of files's FS.
 * @param  mode              READ, WRITE or READ_AND_WRITE
 * @param  descriptorIndex   Index of file's descriptor in the directory.
 * @return                   File's index, or -1 if failed.
 */
index_file_t openFileTAA(charzao_t *name, index_fs_t fsIndex, int mode, index_descriptor_t descriptorIndex);

/**
 * Checks if a file is already opened on TAA.
 * @param  id                File's index.
 * @return                   SUCCESS if file is opened, otherwise FAIL.
 */
int isFileOpenTAA(index_file_t id);

/**
 * Remove a opened file from TAA.
 * @method closeFileTAA
 * @param  id           index of file.
 * @return              SUCCESS OR FAIL.
 */
int closeFileTAA(index_file_t id);

/**
 * Closes all files from the File System.
 *
 * @param  fs File system's index.
 */
void closeAllFilesFromFSTAA(index_fs_t fs);

/**
 * Finds file in TAA and returns it's index.
 *
 * @param  name         File's name.
 * @param  fsIndex      Index of files's FS.
 * @return              File's index, or -1 if failed.
 */
index_file_t getFileIndexTAA(charzao_t *name, index_fs_t fsIndex);

/**
 * Returns file's File System.
 * @param  id File's identifier.
 * @return    File's FS.
 */
index_fs_t getFileFSTAA(index_file_t id);

/**
 * Finds file's mode.
 *
 * @param  id          File's index in table.
 * @return             file's mode, -1 if position is invalid.
 */
int getFileMode(index_file_t id);

/**
 * Finds file's descriptor's index.
 *
 * @param  id                     File's index in table.
 * @return                        File's descriptor's index, or -1 if falied.
 */
index_descriptor_t getFileDescriptorIndexTAA(index_file_t id);

#endif
