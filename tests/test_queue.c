#include "CDSA/queue.h"
#include <stdio.h>

int main() {
  printf("🎬 Starting Queue (FIFO) Tests...\n\n");

  // Create a queue with a maximum capacity of 3 integers
  Queue *q = create_queue(3, sizeof(int));

  // 1. Test Enqueue
  printf("[*] Testing Enqueue...\n");
  int val1 = 100, val2 = 200, val3 = 300;

  printf("Enqueue 100: %s\n", enqueue(q, &val1) ? "Success" : "Failed");
  printf("Enqueue 200: %s\n", enqueue(q, &val2) ? "Success" : "Failed");
  printf("Enqueue 300: %s\n", enqueue(q, &val3) ? "Success" : "Failed");

  printf("Queue size: %zu (Expected: 3)\n", size_queue(q));
  printf("Is full? %s (Expected: Yes)\n\n", is_full_queue(q) ? "Yes" : "No");

  // 2. Test Overfill Prevention
  printf("[*] Testing Overfill Prevention...\n");
  int val4 = 400;
  printf("Enqueue 400: %s (Expected: Failed)\n\n",
         enqueue(q, &val4) ? "Success" : "Failed");

  // 3. Test Front Inspection & Dequeue
  printf("[*] Testing FIFO Order...\n");
  printf("Front item: %d (Expected: 100)\n", *(int *)front_queue(q));

  dequeue(q);
  printf("Dequeued front element. New size: %zu\n", size_queue(q));
  printf("New front item: %d (Expected: 200)\n\n", *(int *)front_queue(q));

  // 4. Test Draining the entire Queue
  printf("[*] Draining remaining Queue items...\n");
  while (!is_empty_queue(q)) {
    printf("Processing front: %d\n", *(int *)front_queue(q));
    dequeue(q);
  }

  printf("Is queue empty? %s (Expected: Yes)\n",
         is_empty_queue(q) ? "Yes" : "No");

  // Clean up
  free_queue(q);
  printf("\n✅ Queue Tests successfully completed!\n");

  return 0;
}
