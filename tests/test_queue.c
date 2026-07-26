#include "CDSA/queue.h"
#include <stdio.h>

int main() {
  printf("🎬 Starting cdsa_queue (FIFO) Tests...\n\n");

  // Create a queue with a maximum capacity of 3 integers
  cdsa_queue *q = cdsa_create_queue(3, sizeof(int));

  // 1. Test Enqueue
  printf("[*] Testing Enqueue...\n");
  int val1 = 100, val2 = 200, val3 = 300;

  printf("Enqueue 100: %s\n", cdsa_enqueue(q, &val1) ? "Success" : "Failed");
  printf("Enqueue 200: %s\n", cdsa_enqueue(q, &val2) ? "Success" : "Failed");
  printf("Enqueue 300: %s\n", cdsa_enqueue(q, &val3) ? "Success" : "Failed");

  printf("cdsa_queue size: %zu (Expected: 3)\n", cdsa_size_queue(q));
  printf("Is full? %s (Expected: Yes)\n\n", cdsa_is_full_queue(q) ? "Yes" : "No");

  // 2. Test Overfill Prevention
  printf("[*] Testing Overfill Prevention...\n");
  int val4 = 400;
  printf("Enqueue 400: %s (Expected: Failed)\n\n",
         cdsa_enqueue(q, &val4) ? "Success" : "Failed");

  // 3. Test Front Inspection & Dequeue
  printf("[*] Testing FIFO Order...\n");
  printf("Front item: %d (Expected: 100)\n", *(int *)cdsa_front_queue(q));

  cdsa_dequeue(q);
  printf("Dequeued front element. New size: %zu\n", cdsa_size_queue(q));
  printf("New front item: %d (Expected: 200)\n\n", *(int *)cdsa_front_queue(q));

  // 4. Test Draining the entire cdsa_queue
  printf("[*] Draining remaining cdsa_queue items...\n");
  while (!cdsa_is_empty_queue(q)) {
    printf("Processing front: %d\n", *(int *)cdsa_front_queue(q));
    cdsa_dequeue(q);
  }

  printf("Is queue empty? %s (Expected: Yes)\n",
         cdsa_is_empty_queue(q) ? "Yes" : "No");

  // Clean up
  cdsa_free_queue(q);
  printf("\n✅ cdsa_queue Tests successfully completed!\n");

  return 0;
}
