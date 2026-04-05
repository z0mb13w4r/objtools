#include "pecode.h"
#include "memfind.h"
#include "objutils.h"
#include "signatures.h"

#define FLAG_NONE            U32MASK_NONE
#define FLAG_ISHI            U32MASK(0)
#define FLAG_ISLO            U32MASK(1)

int signature_scan(handle_t p, const char* data, const size_t datasize, const uint32_t mode) {
  const size_t safesize = (datasize + 1) / 2;
  if (isfind(p) && data && safesize < fgetsize(p)) {
    for (size_t i = 0; i < datasize; i += 2) {
      uint32_t state = FLAG_NONE;

      uint8_t cx = fgetu8(p);
      if (ishex8(data[i + 0])) {
        uint8_t v0 = hex8(data[i + 0]) << 4;
        if (MODE_MASK4HI(cx) == v0) state |= FLAG_ISHI;
      } else {
        state |= FLAG_ISHI;
      }

      if (ishex8(data[i + 1])) {
        uint8_t v1 = hex8(data[i + 1]);
        if (MODE_MASK4LO(cx) == v1) state |= FLAG_ISLO;
      } else {
        state |= FLAG_ISLO;
      }

      if (!MODE_ISSET(state, FLAG_ISHI | FLAG_ISLO)) {
        return i;
      }
    }

    return datasize;
  }

  return -1;
}

int signature_pecode(handle_t p, const char* data, const size_t datasize, const uint32_t mode) {
  const size_t filesize = bgetsize(p);
  const size_t safesize = (datasize + 1) / 2;
  if (isPE(p) && data && safesize < bgetsize(p)) {
    handle_t p1 = fcalloc(bget(p), bgetsize(p), MEMFIND_NOBLOCKSIZE);
    if (p1) {
      int maxcount = 0;
      if (MODE_ISANY(mode, SIGNATURE_EP_ONLY)) {
        fmove(p1, peget_addressofentrypoint(p));
        maxcount = signature_scan(p1, data, datasize, mode);
      } else {
        for (int i = 0; i < (filesize - safesize); ++i) {
          fmove(p1, i);
          maxcount = MAX(maxcount, signature_scan(p1, data, datasize, mode));
        }
      }

      ffree(p1);

      return maxcount;
    }
  }

  return -1;
}
