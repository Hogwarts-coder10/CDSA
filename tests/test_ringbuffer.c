#include "CDSA/ringbuffer.h"
#include <stdio.h>

int main() {
  printf("🔄 Starting Ring Buffer Tests...\n\n");

  // Create a tiny buffer of capacity 3 to force wrap-arounds quickly
  RingBuffer *rb = create_ringbuffer(3, sizeof(int));

  // 1. Fill the buffer
  printf("[*] Testing Push (Filling Buffer)...\n");
  int a = 10, b = 20, c = 30;

  printf("Push 10: %s\n", push_back_ringbuffer(rb, &a) ? "Success" : "Failed");
  printf("Push 20: %s\n", push_back_ringbuffer(rb, &b) ? "Success" : "Failed");
  printf("Push 30: %s\n", push_back_ringbuffer(rb, &c) ? "Success" : "Failed");

  printf("Is Full? %s (Expected: Yes)\n\n",
         is_full_ringbuffer(rb) ? "Yes" : "No");

  // 2. Overfill guard
  printf("[*] Testing Overfill Guard...\n");
  int d = 40;
  printf("Push 40: %s (Expected: Failed)\n\n",
         push_back_ringbuffer(rb, &d) ? "Success" : "Failed");

  // 3. The Wrap-Around Magic
  printf("[*] Testing Wrap-Around (Pop 1, Push 1)...\n");
  printf("Front item: %d (Expected: 10)\n", *(int *)front_ringbuffer(rb));

  pop_front_ringbuffer(rb); // Removes 10, frees up a slot
  printf("Popped front. Size is now: %zu\n", size_ringbuffer(rb));

  printf("Push 40: %s (Expected: Success. This wraps to index 0!)\n",
         push_back_ringbuffer(rb, &d) ? "Success" : "Failed");

  printf("Front item is now: %d (Expected: 20)\n\n",
         *(int *)front_ringbuffer(rb));

  // 4. Drain the buffer
  printf("[*] Draining Buffer...\n");
  while (!is_empty_ringbuffer(rb)) {
    printf("Popping: %d\n", *(int *)front_ringbuffer(rb));
    pop_front_ringbuffer(rb);
  }

  printf("Is Empty? %s (Expected: Yes)\n",
         is_empty_ringbuffer(rb) ? "Yes" : "No");

  free_ringbuffer(rb);
  printf("\n✅ Ring Buffer Tests complete!\n");

  return 0;
}
