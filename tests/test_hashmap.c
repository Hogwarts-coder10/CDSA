#include "CDSA/hashmap.h"
#include <stdio.h>

int main() {
  HashMap *map = create_hashmap(16);
  free_hashmap(map);
  return 0;
}
