#include "version.h"
#include "simplegrade.h"
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>



// tenta ler um número de caracteres iguais de um arquivo
int read_chars(indice_fs_t vfs, char * filename, int v, char c, int num){
    char * buffer;
    indice_arquivo_t f;
    int i;
    int ok = 0; 

    f = vopen(vfs, filename, LEITURA, v);
    if (f){
        buffer = (char *) calloc(num, sizeof(char));
        assert(buffer);
        if (vread(f, num, buffer)){
            for(i=0; i<num; i++){
                if (buffer[i] != c)
                    break;
            }        
            if (i == num)
                ok = 1;
        }
        free(buffer);
        vclose(f);
    }
    return ok;
}


// tenta escrever um número de caracteres iguais de um arquivo
int write_chars(indice_fs_t vfs, char * filename, int v, char c, int num){
    char * buffer;
    indice_arquivo_t f;
    int i;
    int ok = 0; 

    f = vopen(vfs, filename, ESCRITA, v);
    if (f){
        buffer = (char *) calloc(num, sizeof(char));
        for(i=0; i<num; i++){
                buffer[i] = c;
        }        
        assert(buffer);
        if (vwrite(f, num, buffer))
            ok = 1;
        free(buffer);
        vclose(f);
    }
    return ok;
}

void test_too_few_blocks(){
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
        isGreaterThan(vopen(vfs, "äßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäßäß", ESCRITA, 0),0, 1);
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
    if (!vopen(vfs, "a.txt", LEITURA, 0))
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


int main(){
    test_too_few_blocks();
    test_enough_blocks();
    test_vfs_exists();
    test_utf_8();
    test_multiple_fs();
    test_one_file_one_block();
    test_multiple_versions();
    test_multiple_blocks();
    test_seek();
    test_delete();
    test_date();
	printf("Grading \n");
	GRADEME();

	if (grade==maxgrade)
		return 0;
	else return grade;
}