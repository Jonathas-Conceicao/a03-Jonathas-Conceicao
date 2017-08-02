#ifndef _BLOCK_H
#define _BLOCK_H

#include "version.h"
#include <stdlib.h>

#define LIMIT (BLOCO / sizeof(int)) - 2

typedef indice_arquivo_t index_block_t;

typedef struct block_ {
  int indexVersion[MAXVERSIONS];
}block_t;

typedef struct indexer_ {
  index_block_t nextIndexer;
  index_block_t emptyBlock;
  index_block_t data[LIMIT];
}indexer_t;


#endif
