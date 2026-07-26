#include "CDSA/kstring.h"
#include <stdio.h>

int main(void) {

  cdsa_kstring *str = cdsa_create_kstring();

  printf("Size: %zu\n", cdsa_size_kstring(str));

  printf("String: '%s'\n", c_str_kstring(str));

  append_kstring(str, "Hello");
  append_kstring(str, " ");
  append_kstring(str, "World");

  printf("%s\n", c_str_kstring(str));

  cdsa_free_kstring(str);

  return 0;
}
