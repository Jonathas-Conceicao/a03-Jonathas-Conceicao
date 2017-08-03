#ifndef _USEFUL_H
#define _USEFUL_H

#include "version.h"

typedef indice_fs_t index_fs_t;
typedef indice_arquivo_t index_block_t;

#define SUCESS SUCESSO
#define FAIL FALHA

#define READ LEITURA
#define WRITE ESCRITA
#define READ_AND_WRITE LEITURAESCRITA

#define BLOCK_SIZE BLOCO

typedef char charzao_t;

/**
 * Compares the two strings.
 *
 * @param  s1             First string.
 * @param  s2             Second string.
 * @return                It returns an integer less than, equal to, or greater than zero if s1 is found,
 *                        respectively, to be less than, to match, or be greater than s2.
 */
int compareCharzao(charzao_t *s1, charzao_t *s2);

#endif //_USEFUL_H
