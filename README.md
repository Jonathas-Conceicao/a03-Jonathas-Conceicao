# Trabalho 3 - Sistemas Operacionais

## Declaração

Declaramos que o presente trabalho contém código desenvolvido exclusivamente por nós e que não foi compartilhado de nenhuma forma com terceiros a não ser o professor da disciplina. Compreendo que qualquer violação destes princípios será punida rigorosamente de acordo com o Regimento da UFPEL.

(Preencha com seus dados)

- Nome completo: Lucas Eduardo Bretana
- Username do Github: lucasbretana
- Email @inf: lebretana@inf.ufpel.edu.br

(Em caso de dupla)

- Nome completo: Jonathas Augusto de Oliveira Conceição
- Username do Github: Jonathas-Conceicao
- Email @inf: jadoliveira@inf.ufpel.edu.br

## Descrição

Para o trabalho 3 um sistema de arquivos com versionamento será implementado em nível de usuário para sistemas de acordo com POSIX, especialmente Linux. 

Normalmente, para implementar um sistema de arquivos é necessário ter acesso direito aos dispositivos. Para simplificar a implementação e depuração, um arquivo grande sobre um outro sistema de arquivos será usado. Este arquivo será usado para armazenar os descritores de arquivos e os blocos que os compõem.

O tamanho de bloco a ser usado é 4 KiB.

O gerenciamento de blocos livres deve utilizar a tabela de alocação de arquivos. O índice do primeiro bloco livre estará armazenado nos primeiros 32 bits do primeiro bloco. 

A implementação deverá prover um diretório de um nível com 256 entradas, armazenado nos primeiros blocos do sistema de arquivos implementado. Os descritores deverão estar armazenados em formato binário, i.e., um descritor pode estar dividido em dois blocos. Cada descritor de arquivo deverá conter:

- O nome do arquivo com no máximo 255 caracteres UTF-8 (todos em zero se a entrada não estiver sendo usada)
- O tempo de criação do arquivo
- O tempo da última modificação
- O tempo do último acesso
- O índice do primeiro bloco
- O tamanho em bytes
- O número de versões

Os índices que compõem todos os arquivos serão armazenados no bloco seguinte aos  blocos do diretório como um vetor de índices de 32 bits. Cada entrada indica qual o próximo bloco que pertence ao arquivo. O índice _0_ é reservado para indicar que não há próximo bloco.  A última entrada do bloco de índices é reservada para dizer qual é o próximo bloco de índices para a próxima versão, se existir.

## Versionamento

Cada vez que um arquivo é aberto para escrita, gera uma nova versão do mesmo. A primeira versão é "0". Para cada nova versão, um bloco de índice é alocado. Apenas blocos modificados são armazenados. Os blocos que não foram modificados ou que não existem são marcados com _0_.

Por exemplo, se a versão _0_ possuir apenas os dois primeiros blocos:

        1 5 0 ...

E a versão _1_ modificar o primeiro bloco, terá um arquivo de índice com:

        9 0 0 ...

Se a versão _2_ adicionar um bloco ao fim do arquivo:

        0 0 11 0 ...

Qualquer versão pode ser aberta para leitura, mas qualquer abertura para escrita gera uma nova versão.


## Funções a serem implementadas

Detalhes dos parâmetros e da semântica das funções estão descritas na documentação do arquivo crypfs.h. Outras funções podem ser implementadas, mas não devem ser visíveis aos usuários finais. O arquivo crypfs.h **NÃO** deve ser modificado.

### initfs()

Função para inicializar um arquivo no sistema de arquivos hospedeiro com os blocos necessários.

Aloca os blocos, incluindo os blocos de metadados da estrutura do diretório.

### vopenfs()

Função usada para abrir um sistema de arquivos já inicializado por initfs().


### vopen()

Abre um arquivo. Deve ser chamada antes de qualquer outra operação com o arquivo e especificar se o arquivo é aberto para leitura, escrita ou ambos. Em caso de leitura apenas, deve ser especificada a versão.

Retorna um file handler, NULL se houve erro.

### vread()

Lê **n** bytes de um arquivo. Os bytes são entregues decriptografados com a chave usada para abrir o arquivo. Retorna o número de bytes lidos.

### vwrite()

Escreve **n** bytes. Retorna **1** se sucesso, **0** se erro.

### vdelete()

Remove o arquivo, com todas suas versões, e o fecha.  Retorna **1** se sucesso, **0** se erro.

### vclose()

Fecha o arquivo.  Retorna o número da versão. 

## vseek()

Muda a posição atual no acesso ao arquivo. Retorna **1** se sucesso, **0** se erro.

## vcreation(), vaccessed(), vlast_modified()

Retornam o tempo em que o arquivo foi criado, acessado e escrito pela última vez, respectivamente. 

## Casos de Teste

O caso de teste deve criar um sistema de arquivos de nome **versioning**

Os casos de teste deverão demonstrar todas as funcionalidades implementadas. Para tanto, deve ser usado a biblioteca **simplegrade**.

As flags **-std=c99 -Wall -Werror -Wextrea** são obrigatórias. O compilador GCC deve ser usado. O código deve compilar e executar no Travis.

## Avaliação

Serão pontuados:

- Implementação: **8 pontos**
- Casos de teste dos alunos: **2 pontos**


## Cronograma

- Entrega no github: **04/08/2017**


