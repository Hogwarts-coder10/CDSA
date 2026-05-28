#include "CDSA/kstring.h"
#include <stdio.h>

int main(void) {

  KString *str = create_kstring();

  printf("Size: %zu\n", size_kstring(str));

  printf("String: '%s'\n", c_str_kstring(str));

  append_kstring(str, "Hello");
  append_kstring(str, " ");
  append_kstring(str, "World");

  printf("%s\n", c_str_kstring(str));

  free_kstring(str);

  return 0;
}
