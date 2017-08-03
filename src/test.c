#include "version.h"
#include "simplegrade.h"

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
        isGreaterThan(vopen(vfs, "teste", ESCRITA, 1),0, 1);

        IF("Nome do arquivo tem .");
        THEN("Deve criar");
        isGreaterThan(vopen(vfs, "teste.txt", ESCRITA, 1),0, 1);

        IF("Nome do arquivo contém UTF-8");
        THEN("Deve criar");
        isGreaterThan(vopen(vfs, "äß", ESCRITA, 1),0, 1);

        IF("Nome do arquivo contém muitos UTF-8");
        THEN("Deve criar");
        isGreaterThan(vopen(vfs, "TODO", ESCRITA, 1),0, 1);
    }
    else isEqual(0,1,4);

}


void test_multiple_fs(){
    WHEN("Criando vários VFS");
    THEN("Devo conseguir cria-los e acessá-los");
    remove("grademe.bin");
    remove("grademe2.bin");
    initfs("grademe.bin",500);
    initfs("grademe2.bin",500);
    indice_fs_t vfs = vopenfs("grademe.bin");
    vfs = vfs;
    indice_fs_t vfs2 = vopenfs("grademe2.bin");
    isNotEqual(vfs2, FALHA, 1);
}




void test_one_file(){

}

void test_multiple_files(){
	int ok = 1;
    remove("grademe.bin");
    initfs("grademe.bin",500);
    indice_fs_t vfs = vopenfs("grademe.bin");


	IF("Crio 255 arquivos");
	THEN("Devo conseguir criá-los e acessá-los");
	for(int i=0; (i<255) && ok; i++){
		char buffer[10];
		snprintf(buffer, sizeof(int), "%d", i);
		ok = ok && vopen(vfs, buffer, ESCRITA, 1);
	}

	isEqual(ok, 1, 2);

}





void test_versions(){

	
}


int main(){
	 test_too_few_blocks();
	 test_enough_blocks();
	 test_vfs_exists();
	 test_utf_8();
	 test_multiple_fs();
	 test_one_file();
	 test_multiple_files();
	 test_versions();

    GRADEME();
    return 0;
}
