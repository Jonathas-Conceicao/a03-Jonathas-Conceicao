#include "useful.h"

#include <string.h>

int compareCharzao(charzao_t *s1, charzao_t *s2) {
  return strcmp(s1,s2);
}

void copyCharzao(char *dest, char *src) {
  strcpy(dest, src);
  return;
}
