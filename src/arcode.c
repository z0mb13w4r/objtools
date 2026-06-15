#include "arcode.h"
#include "memfind.h"
#include "objutils.h"

bool_t isAR(const pbuffer_t p) {
  if (issafe(p)) {
    const bool_t isarch = 'a' == getb(p, 2) && 'r' == getb(p, 3) && 'c' == getb(p, 4) && 'h' == getb(p, 5);
    const bool_t isthin = 't' == getb(p, 2) && 'h' == getb(p, 3) && 'i' == getb(p, 4) && 'n' == getb(p, 5);
    return '!' == getb(p, 0) && '<' == getb(p, 1) && (isarch || isthin) && '>' == getb(p, 6) && '\n' == getb(p, 7) ? TRUE : FALSE;
  }

  return FALSE;
}

bool_t isARarch(const pbuffer_t p) {
  if (issafe(p)) {
    const bool_t isarch = 'a' == getb(p, 2) && 'r' == getb(p, 3) && 'c' == getb(p, 4) && 'h' == getb(p, 5);
    return '!' == getb(p, 0) && '<' == getb(p, 1) && isarch && '>' == getb(p, 6) && '\n' == getb(p, 7) ? TRUE : FALSE;
  }

  return FALSE;
}

bool_t isARthin(const pbuffer_t p) {
  if (issafe(p)) {
    const bool_t isthin = 't' == getb(p, 2) && 'h' == getb(p, 3) && 'i' == getb(p, 4) && 'n' == getb(p, 5);
    return '!' == getb(p, 0) && '<' == getb(p, 1) && isthin && '>' == getb(p, 6) && '\n' == getb(p, 7) ? TRUE : FALSE;
  }

  return FALSE;
}

unknown_t ecget_ahdr(const pbuffer_t p, const int index) {
  if (isAR(p)) {
    if (0 == index) {
      return getp(p, SARMAG, sizeof(struct ar_hdr));
    }

    size_t size = bgetsize(p) - SARMAG;
    handle_t p0 = fmalloc(getp(p, SARMAG, size), size, MEMFIND_NOBLOCKSIZE);
    if (p0) {
      struct ar_hdr* p1 = fgetp(p0, sizeof(struct ar_hdr));
      int i = 1;
      while (i < index && p1) {
//printf("size = %ld\n", decb(p1->ar_size, sizeof(p1->ar_size)));
        fstep(p0, decb(p1->ar_size, sizeof(p1->ar_size)));
        p1 = fgetp(p0, sizeof(struct ar_hdr));
        ++i;
      }

      ffree(p0);
      return p1 && (index == i) ? p1 : NULL;
    }
  }

  return NULL;
}

handle_t ecget_archive(const pbuffer_t p, const int index, const size_t blocksize) {
  const size_t HDRSIZE = sizeof(struct ar_hdr);

  if (isAR(p)) {
    if (0 == index) {
      struct ar_hdr* p0 = getp(p, SARMAG, HDRSIZE);
      if (p0) {
        size_t size1 = decb(p0->ar_size, sizeof(p0->ar_size));
        if (ISSIZE(size1)) {
          unknown_t p1 = getp(p, SARMAG, HDRSIZE + size1);
          if (p1) {
            return fmalloc(p1, HDRSIZE + size1, blocksize | MEMFIND_BIGENDIAN | MEMFIND_NOBLOCKSIZE);
          }
        }
      }
    } else {
      size_t size = bgetsize(p) - SARMAG;
      handle_t p0 = fmalloc(getp(p, SARMAG, size), size, MEMFIND_NOBLOCKSIZE);
      if (p0) {
        size_t cpos = fgetcpos(p0);
        struct ar_hdr* p1 = fgetp(p0, HDRSIZE);
        int i = 1;
        while (i < index && p1) {
//printf("size = %ld\n", decb(p1->ar_size, sizeof(p1->ar_size)));
          fstep(p0, decb(p1->ar_size, sizeof(p1->ar_size)));

          cpos = fgetcpos(p0);
          p1 = fgetp(p0, HDRSIZE);
          ++i;
        }

        handle_t p2 = NULL;
        if (p1 && (index == i)) {
          size_t size1 = decb(p1->ar_size, sizeof(p1->ar_size));
          if (ISSIZE(size1)) {
            unknown_t p3 = fmove(p0, cpos);
            if (p3) {
//printf("size = %d:%ld\n", index, size1);
              p2 = fmalloc(p3, HDRSIZE + size1, blocksize | MEMFIND_BIGENDIAN | MEMFIND_NOBLOCKSIZE);
            }
          }
        }

        ffree(p0);
        return p2;
      }
    }
  }

  return NULL;
}

