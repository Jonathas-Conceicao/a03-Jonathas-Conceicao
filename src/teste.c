#include "version.h"
#include "simplegrade.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

/*void test_too_few_blocks(){
    WHEN("Criando novo VFS");
    IF("Menos blocos que necessário");
    THEN("Deve retornar FALHA");

    remove("grademe.bin");
    isEqual(initfs("grademe.bin",5),FALHA,1);

}

void test_enough_blocks(){
    WHEN("Criando novo VFS");
    IF("Número de blocos razoável");
    THEN("Deve retornar SUCESSO");

    remove("grademe.bin");
    isEqual(initfs("grademe.bin",500),SUCESSO,1);
}


void test_vfs_exists(){
    remove("grademe.bin");
    WHEN("Criando arquivos");
    IF("Crio duas vezes com mesmo nome");
    THEN("Deve dar erro");
    isNotEqual(initfs("grademe.bin", 500), initfs("grademe.bin",500), 1);
}

void test_utf_8(){
    WHEN("Criando arquivos");
    remove("grademe.bin");
    initfs("grademe.bin",500);
    indice_fs_t vfs = vopenfs("grademe.bin");
    if (vfs){
        IF("Nome do arquivo pequeno e simples");
        THEN("Deve criar");
        isGreaterThan(vopen(vfs, "teste", ESCRITA, 0),0, 1);

        IF("Nome do arquivo tem .");
        THEN("Deve criar");
        isGreaterThan(vopen(vfs, "teste.txt", ESCRITA, 0),0, 1);

        IF("Nome do arquivo contém UTF-8");
        THEN("Deve criar");
        isGreaterThan(vopen(vfs, "äß", ESCRITA, 0),0, 1);

        IF("Nome do arquivo contém muitos UTF-8");
        THEN("Deve criar");
        isGreaterThan(vopen(vfs, "TODO", ESCRITA, 0),0, 1);
    }
    else isEqual(0,1,4);
    vclosefs(vfs);

}


void test_multiple_fs(){
    WHEN("Criando vários VFS");
    THEN("Devo conseguir cria-los e acessá-los");
    remove("grademe.bin");
    remove("grademe2.bin");
    initfs("grademe.bin",500);
    initfs("grademe2.bin",500);
    indice_fs_t vfs = vopenfs("grademe.bin");
    indice_fs_t vfs2 = vopenfs("grademe2.bin");
    isNotEqual(vfs2, FALHA, 1);
    vclosefs(vfs);
    vclosefs(vfs2);
}


void test_one_file_one_block(){
    int ok=0;
    remove("grademe.bin");
    initfs("grademe.bin",500);
    indice_fs_t vfs = vopenfs("grademe.bin");
    WHEN("Crio um arquivo com um bloco e duas versões");
    THEN("Conteúdo lido deve ser igual ao escrito");

    if (write_chars(vfs, "a.txt", 0, 'a', 4096)){
        if (write_chars(vfs, "a.txt", 1, 'b', 4096)){
            if (read_chars(vfs, "a.txt", 0, 'a', 4096)){
                if (read_chars(vfs, "a.txt", 1, 'b', 4096))
                    ok = 1;
            }
        }
    }
    isEqual(ok, 1, 1);
    vclosefs(vfs);
}

void test_multiple_versions(){
    int ok=0;
    remove("grademe.bin");
    initfs("grademe.bin",500);
    indice_fs_t vfs = vopenfs("grademe.bin");

    WHEN("Crio um arquivo com um bloco e duas versões");
    THEN("Conteúdo lido deve ser igual ao escrito em cada versão");

    if (write_chars(vfs, "a.txt", 0, 'a', 4096)){
        if (write_chars(vfs, "a.txt", 1, 'b', 4096)){
            if (read_chars(vfs, "a.txt", 0, 'a', 4096)){
                if (read_chars(vfs, "a.txt", 1, 'b', 4096))
                    ok = 1;
            }
        }
    }
    isEqual(ok, 1, 1);
    vclosefs(vfs);

}

void test_multiple_blocks(){
    int ok=0;
    remove("grademe.bin");
    initfs("grademe.bin",500);
    indice_fs_t vfs = vopenfs("grademe.bin");

    WHEN("Crio um arquivo com vários blocos");
    THEN("Conteúdo lido deve ser igual ao escrito");

    if (write_chars(vfs, "a.txt", 0, 'a', 40960)){
        if (read_chars(vfs, "a.txt", 0, 'a', 40960))
            ok = 1;
    }
    isEqual(ok, 1, 1);
    vclosefs(vfs);
}

void test_seek(){
     int ok=0;
    remove("grademe.bin");
    initfs("grademe.bin",500);
    indice_fs_t vfs = vopenfs("grademe.bin");

    WHEN("Faço um seek em um arquivo");
    THEN("Deve corrigir o ponteiro de lido/escrito");

    char buffer[10];
    char s[] =  "123456789";
    indice_arquivo_t f;
    int i;

    f = vopen(vfs, "teste.txt", LEITURAESCRITA, 0);
    if (f){
        vwrite(f, 10, s);
        vseek(f,0);
        if (vread(f, 10, buffer)){
            for(i=0; i<10; i++){
                if (buffer[i] != s[i])
                    break;
            }
            if (i == 10)
                ok = 1;
        }
        vclose(f);
    }


    isEqual(ok, 1, 2);
    vclosefs(vfs);
}


void test_delete(){
    int ok=0;
    remove("grademe.bin");
    initfs("grademe.bin",500);
    indice_fs_t vfs = vopenfs("grademe.bin");
    WHEN("Removo um arquivo");
    THEN("Deve remover todas versões");

    if (write_chars(vfs, "a.txt", 0, 'a', 4096)){
        write_chars(vfs, "a.txt", 1, 'b', 4096);
    }
    indice_arquivo_t  f = vopen(vfs, "a.txt", LEITURA, 1);
    vdelete(f);
    if (!vopen(vfs, "a.txt", 0, LEITURA))
        ok = 1;

    isEqual(ok, 1, 1);
    vclosefs(vfs);
}

void test_date(){
    int ok=0;
    remove("grademe.bin");
    initfs("grademe.bin",500);
    indice_fs_t vfs = vopenfs("grademe.bin");
    WHEN("Verifico datas em um arquivo");
    THEN("Ordenamento deve ser preservado");

    if (write_chars(vfs, "a.txt", 0, 'a', 4096)){
        time_t created, accessed, last_mod;
        sleep(2);
        indice_arquivo_t f = vopen(vfs, "a.txt", LEITURAESCRITA, 1);
        created = vcreation(f, 0);
        accessed = vaccessed(f, 0);
        sleep(2);

        vwrite(f, 1, "a");
        last_mod = vlast_modified(f, 1);
        if (created < accessed)
            ok++;
        if (accessed < last_mod)
            ok++;
        vclose(f);

    }
    isEqual(ok, 2, 3);
    vclosefs(vfs);
}
*/

void test_1(){
  remove("test.bin"); // just to be sure
  int handTest1 = initfs("test.bin", 666);
  indice_fs_t ind = vopenfs("test.bin");

  WHEN("Tento abrir um arquivo pela primeira vez como leitura");
  THEN("Espero falha");
  indice_arquivo_t zeroFile = vopen(handTest1, "firstFile", LEITURA, 0);
  isEqual(zeroFile, FALHA, 1);

  indice_arquivo_t firstFile = vopen(handTest1, "firstFile", ESCRITA, 0);
  indice_arquivo_t firstFileError = vopen(handTest1, "firstFile", LEITURA, 0);
  WHEN("Tento abrir o mesmo arquivo duas vezes: 1 como escrita, 2 como leitura");
  THEN("Espero que não de falaha para o primeiro");
  isNotEqual(firstFile, FALHA, 1);
  THEN("Espero falha pro segundo");
  isEqual(firstFileError, FALHA, 1);
  
  char *name = calloc(7, sizeof(char));
  strcpy(name, "Batman");

  WHEN("Uso o meu arquivo aberto para escrever uma mensagem e depois fecho-o");
  THEN("Dois sucessos");
  int right = 0;
  right += vwrite(firstFile, 7, name);
  right += vclose(firstFile);
  isEqual(right, 2, 2); 

  WHEN("Reabro o arquivo para leitura e leio o mesmo numero de bytes");
  THEN("Espero que não falhe ao abrir");
  indice_arquivo_t firstFileRead = vopen(handTest1, "firstFile", LEITURA, 0);
  isNotEqual(firstFileRead, FALHA, 1);
  THEN("E espero ler o mesmo conteudo escrito");
  char *nameRead = calloc(7, sizeof(char));
  vread(firstFileRead, 7, nameRead);
  isEqual(strcmp(name, nameRead), 0, 1);

  free(name);
  free(nameRead);
  vclosefs(ind);
  remove("test.bin"); // just to be sure
}
void test_2(){
  remove("test2.bin");
  WHEN("Abro um novo file system sem fazer initfs");
  THEN("Espero ter falha");
  indice_fs_t ind = vopenfs("test2.bin");
  isEqual(ind, FALHA, 1);

  WHEN("Feito o initfs");
  THEN("Espero não ter falha");
  int handTest2 = initfs("test2.bin", 777);
  ind = vopenfs("test2.bin");
  isNotEqual(ind, FALHA, 1);

  WHEN("Abro um arquivo para escrita");
  THEN("Espero que não falhe");
  indice_arquivo_t hand = vopen(handTest2, "バットマン", ESCRITA, 0); // translate that
  isNotEqual(hand, FALHA, 1);

  WHEN("Escrevo um arquivo");
  THEN("Espero conseguir");
  char *nananana = calloc(2 * 4 + 1, sizeof(char));
  strcpy(nananana, "NaNaNaNa");
  isEqual(vwrite(hand, 2*4+1, nananana), 1, 1);;
 
  WHEN("Fecho arquivo");
  THEN("Espero ter sucesso");
  isEqual(vclose(hand), SUCESSO, 1);;

  WHEN("Abro o arquivo pra leitura");
  THEN("Espero não falhar");
  hand = vopen(handTest2, "バットマン", LEITURA, 0);
  isNotEqual(hand, FALHA, 1);

  WHEN("Faço um seek");
  THEN("Espero sucesso");
  isEqual(vseek(hand, 2), SUCESSO, 1);

  WHEN("Leio o restante do arquivo");
  THEN("Espero parte da string escrita");
  char *nana = calloc(2 * 2 + 1, sizeof(char));
  vread(hand, 4, nana); 
  isEqual(strcmp("NaNa", nana), 0, 1);

  WHEN("Deleto um arquivo existente");
  THEN("Espero sucesso");
  isEqual(vdelete(hand), SUCESSO, 1);

  WHEN("Tento abrir um arquivo recem apagado");
  IF("For para leitura");
  THEN("Falha");
  isEqual(vopen(handTest2, "バットマン", LEITURA, 0), FALHA, 1);;
  IF("For para escrita");
  THEN("Espero não falhar");
  hand = vopen(handTest2, "バットマン", ESCRITA, 0);
  isNotEqual(hand, FALHA, 1);

  WHEN("Se tento ler desse arquivo");
  THEN("Não devo ler nada, pois ele foi apagado e este é um arquivo novo");
  isEqual(vread(hand, 4, nana), 0, 1);

  free(nananana);
  free(nana);
  vclosefs(ind);
  remove("test2.bin");
}

void test_3(){
}

int main(){
  test_1();
  test_2();
  test_3();
  printf("Grading \n");
  GRADEME();

  if (grade==maxgrade)
    return 0;
  else return grade;
}
