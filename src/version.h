#ifndef _VERSION_H_
#define _VERSION_H_

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
//#include "simplegrade.h"

#define SUCESSO 1
#define FALHA 0

#define LEITURA 0
#define ESCRITA 1
#define LEITURAESCRITA 2

#define MAXVERSIONS 8 // máximo de versões que um arquivo pode ter
#define MAXBLOCKSVERSION 256 // máximo de blocos que uma versão pode mudar


#define BLOCO 4096


typedef int indice_arquivo_t;
typedef int indice_fs_t;

/** Inicializa o arquivo no sistema de arquivos hospedeiro.
 *
 *  Deve ser chamada apenas uma vez para um determinado sistema de arquivos.
 *  Caso o nome de arquivo já exista, deve retornar erro.
 *  Também deve retornar erro caso o número de blocos não seja o suficiente
 *  para armazenar os metadados pelo menos.
 *
 *  @param  arquivo   nome do arquivo no sistema de arquivos hospedeiro
 *  @param  blocos    número de blocos do arquivo
 *  @return           SUCCESSO ou FALHA
 */
int initfs(char * arquivo, int blocos);


/** Abre um sistema de arquivos.
 *
 *  @param  arquivo   nome do arquivo no sistema de arquivos hospedeiro
 *  @return           file handle do descritor de sistema de arquivos ou FALHA
 */
indice_fs_t vopenfs(char * arquivo);

/*
 * Fecha um sistema de arquivos.
 *
 * @param handler   o handler do descritor de sistema de arquivo
 */
void vclosefs(indice_fs_t handler);

/** Abre um arquivo versionado.
 *
 *
 * @param   fs        o file handle do sistema de arquivos
 * @param   nome      o arquivo a ser aberto
 * @param   acesso    LEITURA, ESCRITA ou LEITURAESCRITA
 * @param   versao    versão a ser aberta
 * @return            índice do arquivo aberto, FALHA se não abrir
 */
indice_arquivo_t vopen(indice_fs_t fs, char * nome,  int acesso, int version);

/** Fecha um arquivo versionado.
 *
 * @param   arquivo   índice para a tabela de arquivos abertos
 * @return            SUCESSO OU FALHA
 */
int vclose(indice_arquivo_t arquivo);

/** Lê bytes de um arquivo versionado aberto.
 *
 * @param   arquivo   índice para a tabela de arquivos abertos
 * @param   tamanho   número de bytes a serem lidos
 * @param   buffer    ponteiro para buffer onde serão armazenados os bytes lidos
 * @return            número de bytes lidos
 */
uint32_t vread(indice_arquivo_t arquivo, uint32_t tamanho, char *buffer);

/** Escreve bytes em um arquivo versionado aberto.
 *
 * A escrita é sempre realizada no modo append, ou seja, no fim do arquivo.
 *
 * @param   arquivo   índice para a tabela de arquivos abertos
 * @param   tamanho   número de bytes a serem escritos
 * @param   buffer    ponteiro para conteúdo a ser escrito
 * @return            SUCESSO ou FALHA
 */
int vwrite(indice_arquivo_t arquivo, uint32_t tamanho, char *buffer);

/** Apaga um arquivo e o fecha.
 *
 * @param   arquivo   índice para a tabela de arquivos abertos
 * @return            SUCESSO ou FALHA
 */

int vdelete(indice_arquivo_t arquivo);

/** Modifica a posição atual de leitura ou escrita do arquivo
 *
 * @param   arquivo   índice para a tabela de arquivos abertos
 * @param   seek      deslocamento em relação ao início do arquivo
 * @return            SUCESSO ou FALHA
 */
int vseek(indice_arquivo_t arquivo, uint32_t seek);

/** Retorna o tempo em que uma versão do arquivo foi criado
 *
 * @param   arquivo   índice para a tabela de arquivos abertos
 * @param   versao    versão a ser retornada
 * @return            tempo
 */
time_t vcreation(indice_arquivo_t arquivo, int versao);

/** Retorna o tempo em que a versão do arquivo foi acessado
 *
 * @param   arquivo   índice para a tabela de arquivos abertos
 * @return            tempo
 */
time_t vaccessed(indice_arquivo_t arquivo, int versao);

/** Retorna o tempo em que o arquivo foi modificado
 *
 * @param   arquivo   índice para a tabela de arquivos abertos
 * @param   versao    versão a ser retornada
 * @return            tempo
 */
time_t vlast_modified(indice_arquivo_t arquivo, int versao);

#endif
