#include <bfd.h>
#include <string.h>
#include <dis-asm.h>

#include "opcode.h"
#include "printf.h"

static void callback_find_max_sectionhdr_name(bfd *f ATTRIBUTE_UNUSED, asection *s, void *p) {
  /* Ignore linker created section. */
  if (s->flags & SEC_LINKER_CREATED) return;

  size_t *max_name_size = (size_t *) p;
  const char* name = bfd_section_name(s);
  size_t name_size = strlen(name);
  if (name_size > *max_name_size) {
    *max_name_size = name_size;
  }
}

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
  return ismode(p, MODE_OPCODE);
}

int isopsection(handle_t p) {
  return ismode(p, MODE_OPSECTION);
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
  } else if (ismode(p, mode)) {
    if (MODE_OPSECTION == mode || MODE_OPWRAP == mode) {
      popwrap_t p0 = CAST(popwrap_t, p);
      return p0->item;
    }
  }

  return NULL;
}

size_t ocget_maxsectionnamesize(handle_t p) {
  size_t maxsize = 0;
  bfd* p0 = ocget(p, OPCODE_BFD);
  if (p0) {
    bfd_map_over_sections(p0, callback_find_max_sectionhdr_name, &maxsize);
  }

  return maxsize;
}

uint64_t ocget_flags(handle_t p) {
  bfd* p0 = ocget(p, OPCODE_BFD);
  if (p0) {
    return bfd_get_file_flags(p0);
  } else if (isopsection(p)) {
    asection* s = ocget(p, MODE_OPSECTION);
    return s ? s->flags : 0;
  }

  return 0;
}

uint64_t ocget_size(handle_t p) {
  if (isopsection(p)) {
    return bfd_section_size(ocget(p, MODE_OPSECTION));
  }

  return 0;
}

uint64_t ocget_positon(handle_t p) {
  if (isopsection(p)) {
    asection* s = ocget(p, MODE_OPSECTION);
    return s ? s->filepos : 0;
  }

  return 0;
}

uint64_t ocget_alignment(handle_t p) {
  if (isopsection(p)) {
    return bfd_section_alignment(ocget(p, MODE_OPSECTION));
  }

  return 0;
}

uint64_t ocget_lmaddress(handle_t p) {
  if (isopsection(p)) {
    return bfd_section_lma(ocget(p, MODE_OPSECTION));
  }

  return 0;
}

uint64_t ocget_saddress(handle_t p) {
  bfd* p0 = ocget(p, OPCODE_BFD);
  if (p0) {
    return bfd_get_start_address(p0);
  }

  return 0;
}

uint64_t ocget_vmaddress(handle_t p) {
  if (isopsection(p)) {
    return bfd_section_vma(ocget(p, MODE_OPSECTION));
  }

  return 0;
}

uint64_t ocget_opb(handle_t p, handle_t s) {
  uint64_t opb = 1;

  if (isopcode(p) && isopsection(s)) {
    opb = bfd_octets_per_byte(ocget(p, OPCODE_BFD), ocget(p, MODE_OPSECTION));
  }

  return opb;
}

uint64_t ocget_soffset(handle_t p, handle_t s) {
  uint64_t soffset = 0;

  if (isopcode(p) && isopsection(s)) {
    popcode_t p0 = CAST(popcode_t, p);

    if (p0->saddress == CAST(uint64_t, -1) || p0->saddress < ocget_vmaddress(s)) soffset = 0;
    else soffset = p0->saddress - ocget_vmaddress(s);
  }

  return soffset;
}

uint64_t ocget_eoffset(handle_t p, handle_t s) {
  uint64_t eoffset = 0;

  if (isopcode(p) && isopsection(s)) {
    popcode_t p0 = CAST(popcode_t, p);

    uint64_t size = ocget_size(s);
    if (0 != size) {
      /* Compute the address range to display. */
      uint64_t opb = ocget_opb(p, s);

      if (p0->eaddress == CAST(uint64_t, -1)) eoffset = size / opb;
      else {
        if (p0->eaddress < ocget_vmaddress(s)) eoffset = 0;
        else eoffset = p0->eaddress - ocget_vmaddress(s);

        if (eoffset > size / opb) eoffset = size / opb;
      }
    }
  }

  return eoffset;
}

const char* ocget_name(handle_t p) {
  bfd* p0 = ocget(p, OPCODE_BFD);
  if (p0) {
    return bfd_get_filename(p0);
  } else if (isopsection(p)) {
    return bfd_section_name(ocget(p, MODE_OPSECTION));
  }

  return NULL;
}

const char* ocget_fileformat(handle_t p) {
  bfd* p0 = ocget(p, OPCODE_BFD);
  if (p0 && p0->xvec) {
    return p0->xvec->name;
  }

  return NULL;
}

handle_t ocmalloc() {
  popcode_t p = mallocx(sizeof(opcode_t));
  if (p) {
    p->saddress = CAST(uint64_t, -1); /* --start-address */
    p->eaddress = CAST(uint64_t, -1); /* --stop-address */
  }

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

static void callback_section(bfd *f, asection *s, void *p) {
  popfunc_t pcb = CAST(popfunc_t, p);
  if (pcb && pcb->cbfunc) {
    MALLOCSMODE(opwrap_t, sec, MODE_OPSECTION);
    psec->item = s;
    pcb->cbfunc(pcb->handle, psec, pcb->param);
  }
}

int ocdo_sections(handle_t p, opcbfunc_t cbfunc, unknown_t param) {
  if (isopcode(p)) {
    bfd* p0 = ocget(p, OPCODE_BFD);
    if (p0) {
      MALLOCSMODE(opfunc_t, cb, MODE_OPCBFUNC);
      pcb->param = param;
      pcb->cbfunc = cbfunc;
      pcb->handle = p;

      bfd_map_over_sections(p0, callback_section, pcb);
      return 0;
    }
  }

  return -1;
}

