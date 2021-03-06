#ifndef _USEFUL_H
#define _USEFUL_H

#include "version.h"

typedef indice_fs_t index_fs_t;
typedef indice_arquivo_t index_file_t;

typedef int index_block_t;
typedef int index_descriptor_t;

#define SUCCESS SUCESSO
#define FAIL FALHA

#define READ LEITURA
#define WRITE ESCRITA
#define READ_AND_WRITE LEITURAESCRITA

#define BLOCK_SIZE BLOCO

#define MAX_DIR 256 // Max number of files in each File System.

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

/**
 * Copies the string pointed to by src to the buffer pointed to by dest.
 *
 * @param  dest        Destiny string.
 * @param  src         Source string.
 */
void copyCharzao(char *dest, char *src);

/**
 * Converts between char* and charzao*.
 * @method charToCharzao
 * @param  s1            Orginal String
 * @return               Referece to new charzao.
 */
charzao_t *charToCharzao(char *s1);

#endif //_USEFUL_H
