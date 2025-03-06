#include <string.h>

#include "opcode.h"
#include "printf.h"
#include "options.h"

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

uint64_t ocget_position(handle_t p) {
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
    bfree(p0->items[OPCODE_DISASSEMBLER]);
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

#define DEFAULT_SKIP_ZEROES            (8)
#define DEFAULT_SKIP_ZEROES_AT_END     (3)

int ocdisassemble_default(handle_t p, handle_t o) {
  if (isopcode(p)) {
    popcode_t p1 = CAST(popcode_t, p);
    if (isoptions(o)) {
      poptions_t p5 = CAST(poptions_t, o);
      p1->action = p5->action;
    }

    bfd* p0 = ocget(p, OPCODE_BFD);
    if (p0) {
      struct disassemble_info* p2 = ocget(p, OPCODE_DISASSEMBLER);
      if (NULL == p2) {
        p2 = mallocx(sizeof(struct disassemble_info));
        p1->items[OPCODE_DISASSEMBLER] = p2;
      }
      if (p2) {
        /* Construct and configure the disassembler_info class using stdout */
        init_disassemble_info(p2, stdout, (fprintf_ftype)fprintf);

        p2->application_data = p1;
        p2->arch = bfd_get_arch(p0);
        p2->mach = bfd_get_mach(p0);
        p2->flavour = bfd_get_flavour(p0);
        p2->octets_per_byte = bfd_octets_per_byte(p0, NULL);
        p2->skip_zeroes = DEFAULT_SKIP_ZEROES;
        p2->skip_zeroes_at_end = DEFAULT_SKIP_ZEROES_AT_END;
        p2->disassembler_options = NULL;  // -M options, --disassembler-options=options

        if (bfd_big_endian(p0))          p2->endian_code = p2->display_endian = p2->endian = BFD_ENDIAN_BIG;
        else if (bfd_little_endian(p0))  p2->endian_code = p2->display_endian = p2->endian = BFD_ENDIAN_LITTLE;
        else                             p2->endian_code = p2->endian = BFD_ENDIAN_UNKNOWN;

        /* Construct disassembler_ftype class */
        p1->disfunc = disassembler(p2->arch, bfd_big_endian(p0), p2->mach, p0);

        disassemble_init_for_target(p2);
      }
    }
  }

  return 0;
}

int ocdisassemble_run(handle_t p, handle_t s) {
  if (isopcode(p) && isopsection(s)) {
//    uint64_t soffset = ocget_soffset(p, s);
//    uint64_t eoffset = ocget_eoffset(p, s);

//    uint64_t soffset = ocget_saddress(p);
//    uint64_t eoffset = ocget_saddress(p) + ocget_size(s);

    uint64_t soffset = ocget_vmaddress(s);
    uint64_t eoffset = ocget_vmaddress(s) + ocget_size(s);

    popcode_t p0 = CAST(popcode_t, p);
    struct disassemble_info* p1 = p0->items[OPCODE_DISASSEMBLER];
    if (p1) {
      p1->buffer_vma    = ocget_vmaddress(s);
      p1->buffer_length = ocget_size(s);
      p1->section       = ocget(s, MODE_OPSECTION);
      p1->buffer        = mallocx(p1->buffer_length);

      if (bfd_get_section_contents(p0->items[OPCODE_BFD], p1->section, p1->buffer, 0, p1->buffer_length)) {
        while (soffset < eoffset) {
          printf("%4lx: ", soffset);
          int siz = p0->disfunc(soffset, p1);
          if (siz <= 0) return -1;
          soffset += siz;
          printf("\n");
        }
      }

      free(p1->buffer);
      p1->buffer = NULL;

      return 0;
    }
  }

  return -1;
}

