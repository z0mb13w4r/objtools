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

unknown_t ecget_ahdr(const pbuffer_t p, const int index) {
  if (0 == index) {
    return getp(p, SARMAG, sizeof(struct ar_hdr));
  }

  size_t size = bgetsize(p) - SARMAG;
  handle_t p0 = fmalloc(getp(p, SARMAG, size), size, MEMFIND_NOBLOCKSIZE);
  if (p0) {
    struct ar_hdr* p1 = fgetp(p0, sizeof(struct ar_hdr));
    int i = 1;
    while (i < index && p1) {
//      printf("size = %ld\n", decb(p1->ar_size, sizeof(p1->ar_size)));
      fstep(p0, decb(p1->ar_size, sizeof(p1->ar_size)));
      p1 = fgetp(p0, sizeof(struct ar_hdr));
      ++i;
    }

    ffree(p0);
    return p1 && (index == i) ? p1 : NULL;
  }

  return NULL;
}

handle_t ecget_archive(const pbuffer_t p, const int index) {
  if (0 == index) {
    size_t size0 = sizeof(struct ar_hdr);
    struct ar_hdr* p0 = getp(p, SARMAG, size0);
    if (p0) {
      size_t size1 = decb(p0->ar_size, sizeof(p0->ar_size));
      return fmalloc(getp(p, SARMAG, size0 + size1), size0 + size1, MEMFIND_NOBLOCKSIZE);
    }
  }

  return NULL;
}

