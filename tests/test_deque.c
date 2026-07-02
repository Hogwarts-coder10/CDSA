#include "CDSA/deque.h"
#include <stdio.h>

int main() {
  printf("🎢 Starting Deque (Double-Ended) Tests...\n\n");

  // Capacity of 3 to force wrap-arounds
  Deque *dq = create_deque(3, sizeof(int));

  // 1. Test Push Front (Forces a backwards wrap immediately!)
  printf("[*] Testing Push Front (Reverse Wrap)...\n");
  int a = 10, b = 20;

  printf("Push Front 10: %s\n",
         push_front_deque(dq, &a) ? "Success" : "Failed");

  printf("Push Front 20: %s\n",
         push_front_deque(dq, &b) ? "Success" : "Failed");

  printf("Front item: %d (Expected: 20)\n", *(int *)front_deque(dq));
  printf("Back item: %d (Expected: 10)\n\n", *(int *)back_deque(dq));

  // 2. Test Push Back
  printf("[*] Testing Push Back...\n");
  int c = 30;
  printf("Push Back 30: %s\n", push_back_deque(dq, &c) ? "Success" : "Failed");

  printf("Is Full? %s (Expected: Yes)\n", is_full_deque(dq) ? "Yes" : "No");
  printf("Back item is now: %d (Expected: 30)\n\n", *(int *)back_deque(dq));

  // 3. Test Popping from both ends
  printf("[*] Testing Pops...\n");
  pop_front_deque(dq); // Removes 20
  printf("Popped Front. New Front: %d (Expected: 10)\n",
         *(int *)front_deque(dq));

  pop_back_deque(dq); // Removes 30
  printf("Popped Back. New Back: %d (Expected: 10)\n\n",
         *(int *)back_deque(dq));

  // Clean up
  free_deque(dq);
  printf("✅ Deque Tests successfully completed!\n");

  return 0;
}
