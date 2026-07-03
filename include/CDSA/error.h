#ifndef CDSA_ERROR_H
#define CDSA_ERROR_H

typedef enum {
  CDSA_OK = 0,             // Operation Sucessful
  CDSA_ERR_OOM = -1,       /* calloc / malloc failure */
  CDSA_ERR_NOT_FOUND = -2, // Key, Element Not found
  CDSA_ERR_EXISTS = -3,    // key already exists (for strict inserts)
  CDSA_ERR_INVALID = -4,   // Invalid argument
  CDSA_ERR_FULL = -5,  // ringbuffer, deque, queue — fixed capacity structures
  CDSA_ERR_EMPTY = -6, // pop/peek on empty structure
} CDSA_STATUS;

// Optional but highly recommended: A helper to translate codes to strings for
// logging
const char *cdsa_strerror(CDSA_STATUS status);
#endif
