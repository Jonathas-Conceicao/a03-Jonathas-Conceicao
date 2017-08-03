#include <stdio.h>
// #include <stdio.h>
#include "src/taa.h"

int main(void) {
  taa_t *pTable;
  pTable = initTAA(3);
  int ret, ret2;
  ret = openFileTAA(*pTable, "lol.txt", 0, WRITE, 0);
  printf("Adding lol returned:%i\n",ret);
  ret = openFileTAA(*pTable, "lol.txt", 0, WRITE, 0);
  printf("Adding lol again returned:%i\n",ret);
  ret = openFileTAA(*pTable, "dota.txt", 0, WRITE, 0);
  printf("Adding dota returned:%i\n",ret);
  ret = openFileTAA(*pTable, "f1.txt", 0, WRITE, 0);
  ret2 = openFileTAA(*pTable, "f2.txt", 0, WRITE, 0);
  printf("Adding 2 files (exceeds the limit on the second)\nf1:%i\nf2:%i\n", ret, ret2);


  return 0;
}
