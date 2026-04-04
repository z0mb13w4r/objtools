#include "pecode.h"
#include "memfind.h"
#include "objutils.h"
#include "signatures.h"

#define FLAG_NONE            U32MASK_NONE
#define FLAG_ISHI            U32MASK(0)
#define FLAG_ISLO            U32MASK(1)

int signature_pecode(handle_t p, const char* data, const size_t datasize, const uint32_t mode) {
  if (isPE(p) && data && datasize) {
    handle_t p1 = fcalloc(bget(p), bgetsize(p), MEMFIND_NOBLOCKSIZE);
    if (p1) {
      fmove(p1, peget_addressofentrypoint(p));

      size_t i = 0;
      for (i = 0; i < datasize; i += 2) {
        uint32_t state1 = FLAG_NONE;

        uint8_t cx = fgetu8(p);
        if (ishex8(data[i + 0])) {
          uint8_t v0 = hex8(data[i + 0]) << 4;
          if (MODE_MASK4HI(cx) == v0) state1 |= FLAG_ISHI;
        } else {
          state1 |= FLAG_ISHI;
        }

        if (ishex8(data[i + 1])) {
          uint8_t v1 = hex8(data[i + 1]);
          if (MODE_MASK4LO(cx) == v1) state1 |= FLAG_ISLO;
        } else {
          state1 |= FLAG_ISLO;
        }

        if (!MODE_ISSET(state1, FLAG_ISHI | FLAG_ISLO)) break;
      }

      ffree(p1);

      return i == datasize;
    }
  }

  return 0;
}
