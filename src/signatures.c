#include "pecode.h"
#include "printf.h"
#include "memfind.h"
#include "options.h"
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
    handle_t p1 = fmalloc(bget(p), bgetsize(p), MEMFIND_NOBLOCKSIZE);
    if (p1) {
      int maxcount = 0;
      if (MODE_ISANY(mode, SIGNATURE_EP_ONLY)) {
        if (fmove(p1, peget_addressofentrypoint(p))) {
          maxcount = signature_scan(p1, data, datasize, mode);
        }
      } else {
        for (int i = 0; i < (filesize - safesize); ++i) {
          if (fmove(p1, i)) {
            maxcount = MAX(maxcount, signature_scan(p1, data, datasize, mode));
          }
        }
      }

      ffree(p1);

      return maxcount;
    }
  }

  return -1;
}

int signature_pedump(handle_t p, handle_t q, const imode_t mode) {
  int n = 0;
  handle_t s1 = fmalloc(bget(q), bgetsize(q), MEMFIND_NOBLOCKSIZE);
  if (s1) {
    size_t  size = 0;
    nmode_t role = 0;

    role = fgetchunk(s1, &size);
    if (SIGNATURE_MAGIC0 == role) {
      fstep(s1, size);

      char*  signature = NULL;
      size_t signaturesize = 0;
      char*  signaturename = NULL;
      while (!fiseof(s1)) {
        role = fgetchunk(s1, &size);
        if (SIGNATURE_NAME == role) {
          signaturename = fgetp(s1, size);
          if (MODE_ISANY(mode, OPTPROGRAM_VERBOSE)) {
            n += printf_text("NAME", USE_LT | USE_COLON);
            n += printf_sore(signaturename, size, USE_STR | USE_SPACE | USE_SQ | USE_EOL);
          }
        } else if (SIGNATURE_SIGNATURE == role) {
          signature = fgetp(s1, size);
          signaturesize = size - 1;
          if (MODE_ISANY(mode, OPTPROGRAM_VERBOSE)) {
            n += printf_text("SIGNATURE", USE_LT | USE_COLON);
            n += printf_sore(signature, signaturesize, USE_STR | USE_SPACE | USE_EOL);
          }
        } else if (SIGNATURE_FLAG == role) {
          const uint32_t mode = fgetu32(s1);
          if (MODE_ISANY(mode, OPTPROGRAM_VERBOSE)) {
            n += printf_text("FLAG", USE_LT | USE_COLON);
            n += printf_nice(mode, USE_LHEX | USE_EOL);
          }

          const bool_t isok = MODE_ISNOT(mode, OPTDETECT_EPONLY) || MODE_ISANY(mode, SIGNATURE_EP_ONLY);
          if (isok && (signaturesize == signature_pecode(p, signature, signaturesize, mode))) {
            n += printf_text("MATCHED", USE_LT | USE_TAB | USE_COLON);
            n += printf_text(signaturename, USE_LT | USE_SPACE | USE_SQ | USE_EOL);
            n += printf_text("SIGNATURE", USE_LT | USE_TAB | USE_COLON);
            n += printf_sore(signature, signaturesize, USE_STR | USE_SPACE | USE_EOL);
            if (MODE_ISANY(mode, OPTDETECT_MATCHONCE)) break;
          }
        } else {
          printf_e("bad magic %08x.", role);
          break;
        }
      }
    }

    ffree(s1);
  }

  return n;
}

