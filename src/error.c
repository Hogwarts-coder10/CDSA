#include "CDSA/error.h"

const char *cdsa_strerror(CDSA_STATUS status) {
  switch (status) {
  case CDSA_OK:
    return "OK";
  case CDSA_ERR_OOM:
    return "out of memory";
  case CDSA_ERR_NOT_FOUND:
    return "not found";
  case CDSA_ERR_EXISTS:
    return "key already exists";
  case CDSA_ERR_INVALID:
    return "invalid argument";
  case CDSA_ERR_FULL:
    return "structure is full";
  case CDSA_ERR_EMPTY:
    return "structure is empty";
  default:
    return "unknown error";
  }
}
