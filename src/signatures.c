#include "pecode.h"
#include "signatures.h"

int signature_pecode(handle_t p, const char* data, const size_t datasize, const uint32_t mode) {
  if (isPE(p) && data && datasize) {
    DWORD ep = peget_addressofentrypoint(p);
printf("ep = %x\n", ep);
  }

  return 0;
}
