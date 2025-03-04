#include <bfd.h>
#include <string.h>
#include <dis-asm.h>

#include "opcode.h"
#include "printf.h"

static handle_t create_symbols(bfd *f) {
  if (f && bfd_get_file_flags(f) & HAS_SYMS) {
    long size = bfd_get_symtab_upper_bound(f);
    if (size > 0) {
      pbuffer_t bsy = bmalloc();
      if (bsy) {
        asymbol **asy = (asymbol **) mallocx(size);
        if (asy) {
          long count = bfd_canonicalize_symtab(f, asy);
          bsy->data = asy;
          bsy->size = count;
        } else {
          bsy = bfree(bsy);
        }
      }
      return bsy;
    }
  }

  return NULL;
}

static handle_t create_symbols_dynamic(bfd *f) {
  if (f && bfd_get_file_flags(f) & DYNAMIC) {
    long size = bfd_get_dynamic_symtab_upper_bound(f);
    if (size > 0) {
      pbuffer_t bsy = bmalloc();
      if (bsy) {
        asymbol **asy = (asymbol **) mallocx(size);
        if (asy) {
          long count = bfd_canonicalize_dynamic_symtab(f, asy);
          bsy->data = asy;
          bsy->size = count;
        } else {
          bsy = bfree(bsy);
        }
      }
      return bsy;
    }
  }

  return NULL;
}

int isopcode(handle_t p) {
  if (p) {
    return ismode(p, MODE_OPCODE);
  }

  return 0;
}

int isobject(handle_t p) {
  if (isopcode(p)) {
    popcode_t p0 = CAST(popcode_t, p);
    return p0->items[OPCODE_BFD] && bfd_check_format(p0->items[OPCODE_BFD], bfd_object);
  }

  return 0;
}

int isarchive(handle_t p) {
  if (isopcode(p)) {
    popcode_t p0 = CAST(popcode_t, p);
    return p0->items[OPCODE_BFD] && bfd_check_format(p0->items[OPCODE_BFD], bfd_archive);
  }

  return 0;
}

int iscoredump(handle_t p) {
  if (isopcode(p)) {
    popcode_t p0 = CAST(popcode_t, p);
    return p0->items[OPCODE_BFD] && bfd_check_format(p0->items[OPCODE_BFD], bfd_core);
  }

  return 0;
}

unknown_t ocget(handle_t p, const imode_t mode) {
  if (isopcode(p) && mode < OPCODE_MAXITEMS) {
    popcode_t p0 = CAST(popcode_t, p);
    if (OPCODE_SYMBOLS == mode && NULL == p0->items[mode]) {
      p0->items[mode] = create_symbols(p0->items[OPCODE_BFD]);
    } else if (OPCODE_SYMBOLS_DYNAMIC == mode && NULL == p0->items[mode]) {
      p0->items[mode] = create_symbols_dynamic(p0->items[OPCODE_BFD]);
    }
    return p0->items[mode];
  }

  return NULL;
}

handle_t ocmalloc() {
  popcode_t p = mallocx(sizeof(opcode_t));
  return setmode(p, MODE_OPCODE);
}

handle_t ocfree(handle_t p) {
  if (isopcode(p)) {
    popcode_t p0 = CAST(popcode_t, p);
    bfree(p0->data);
    bfree(p0->items[OPCODE_SYMBOLS]);
    bfree(p0->items[OPCODE_SYMBOLS_DYNAMIC]);
    free(p);
    return NULL;
  }

  return p;
}

handle_t ocopen(const char* name) {
  handle_t p0 = bopen(name);
  if (p0) {
    handle_t p1 = ocmalloc();
    if (p1) {
      popcode_t p2 = CAST(popcode_t, p1);
      p2->data = p0;
      p2->items[OPCODE_BFD] = bfd_openr(name, NULL);
      return p2;
    } else {
      bfree(p0);
    }
  }

  return NULL;
}

int occlose(handle_t p) {
  if (isopcode(p)) {
    popcode_t p0 = CAST(popcode_t, p);
    bfd_close(p0->items[OPCODE_BFD]);
    ocfree(p);
    return 0;
  }

  return -1;
}

void occonfig(const char* name, const char* target) {
  static int isconfigured = 0;

  if (0 == isconfigured) {
    bfd_set_error_program_name(name);

    if (BFD_INIT_MAGIC != bfd_init()) {
      printf_x("libbfd ABI mismatch.");
    }
    isconfigured = -1;
  }

  if (!bfd_set_default_target(target)) {
    printf_x("can't set BFD default target to '%s': %s", target, bfd_errmsg(bfd_get_error()));
  }
}

