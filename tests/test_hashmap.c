#include "CDSA/hashmap.h"
#include <stdio.h>

int main() {
  HashMap *map = create_hashmap(16);
  int a = 10;
  int b = 20;

  insert_hashmap(map, "apple", &a);
  insert_hashmap(map, "banana", &b);

  free_hashmap(map);
  return 0;
}
