#include <bfd.h>
#include <string.h>
#include <dis-asm.h>

#include "printf.h"
#include "objdump.h"

#include "static/has_flags.ci"
#include "static/sectionhdr_flags.ci"

typedef struct objdump_info_s {
  int  action;

} objdump_info_t, *pobjdump_info_t;

static bfd_vma saddress = (bfd_vma) -1; /* --start-address */
static bfd_vma eaddress = (bfd_vma) -1; /* --stop-address */

static pbuffer_t get_symbols(const pbuffer_t p, bfd *f, const int mode) {
  pbuffer_t ps = createx(p, mode);
  if (ps) {
    if (0 == ps->size) {
      if (ismode(ps, MODE_SYMBOLS)) {
        if (bfd_get_file_flags(f) & HAS_SYMS) {
          long size = bfd_get_symtab_upper_bound(f);
          if (size > 0) {
            asymbol **sy = (asymbol **) mallocx(size);
            long count = bfd_canonicalize_symtab(f, sy);
            ps->data = sy;
            ps->size = count;
          }
        }
      } else if (ismode(ps, MODE_SYMBOLS_DYNAMIC)) {
        if (bfd_get_file_flags(f) & DYNAMIC) {
          long size = bfd_get_dynamic_symtab_upper_bound(f);
          if (size > 0) {
            asymbol **sy = (asymbol **) mallocx(size);
            long count = bfd_canonicalize_dynamic_symtab(f, sy);
            ps->data = sy;
            ps->size = count;
          }
        }
      }
    }

    return ps;
  }

  return NULL;
}

static void callback_disassemble(bfd *f, asection *s, void *p) {
  struct disassemble_info *pdi = (struct disassemble_info *)p;
  pobjdump_info_t poi = pdi->application_data;

  /* Sections that do not contain machine code are not normally disassembled. */
  if ((s->flags & SEC_HAS_CONTENTS) == 0) return;
  if (0 == (poi->action & OPTOBJDUMP_DISADDEMBLEALL) && 0 == (s->flags & SEC_CODE)) return;

  bfd_size_type dsize;
  if ((dsize = bfd_section_size(s)) == 0) return;

  /* Compute the address range to display. */
  size_t opb = bfd_octets_per_byte(f, s);

  bfd_vma soffset, eoffset;
  if (saddress == (bfd_vma) -1 || saddress < s->vma) soffset = 0;
  else soffset = saddress - s->vma;

  if (eaddress == (bfd_vma) -1) eoffset = dsize / opb;
  else {
    if (eaddress < s->vma) eoffset = 0;
    else eoffset = eaddress - s->vma;

    if (eoffset > dsize / opb) eoffset = dsize / opb;
  }

  if (soffset >= eoffset) return;

  printf("Disassembly of section %s:\n", s->name);

  printf("\n");
}

static void callback_section_data(bfd *f, asection *s, void *p) {
  if ((s->flags & SEC_HAS_CONTENTS) == 0) return;

  bfd_size_type dsize;
  if ((dsize = bfd_section_size(s)) == 0) return;

  /* Compute the address range to display. */
  size_t opb = bfd_octets_per_byte(f, s);

  bfd_vma soffset, eoffset;
  if (saddress == (bfd_vma) -1 || saddress < s->vma) soffset = 0;
  else soffset = saddress - s->vma;

  if (eaddress == (bfd_vma) -1) eoffset = dsize / opb;
  else {
    if (eaddress < s->vma) eoffset = 0;
    else eoffset = eaddress - s->vma;

    if (eoffset > dsize / opb) eoffset = dsize / opb;
  }

  if (soffset >= eoffset) return;

  printf_text("Contexts of section", USE_LT | USE_SPACE);
  printf_text(s->name, USE_LT | USE_SPACE | USE_SQ);
  printf_text("at offset", USE_LT | USE_SPACE);
  printf_nice(s->filepos + soffset, USE_FHEX16 | USE_COLON | USE_EOL);

  bfd_byte *data = NULL;
  if (bfd_get_full_section_contents(f, s, &data)) {
    printf_data(data, dsize, soffset + s->vma, USE_HEXDUMP);
  } else {
    printf("WARNING: reading section %s failed because: %s\n", s->name, bfd_errmsg(bfd_get_error()));
  }

  printf_eol();
}

static void callback_sectionhdr(bfd *f, asection *s, void *p) {
  size_t name_size = *((size_t *) p);

  /* Ignore linker created section.  See elfNN_ia64_object_p in bfd/elfxx-ia64.c.  */
  if (s->flags & SEC_LINKER_CREATED) return;

  printf_nice(s->index, USE_TAB | USE_DEC2);
  printf_text(bfd_section_name(s), USE_LT | USE_SPACE | SET_PAD(name_size));
  printf_nice(bfd_section_size(s) / bfd_octets_per_byte(f, s), USE_LHEX32);
  printf_nice(bfd_section_vma(s), USE_LHEX64);
  printf_nice(s->lma, USE_LHEX64);
  printf_nice(s->filepos, USE_LHEX32);
  printf(" 2**%u", bfd_section_alignment(s));

  printf_maskmute(zSECTIONHDR1_FLAGS, s->flags, USE_NONE);

  if (bfd_get_arch(f) == bfd_arch_tic54x) {
    printf_maskmute(zSECTIONHDRTIC54X_FLAGS, s->flags, USE_NONE);
  }

  printf_maskmute(zSECTIONHDR2_FLAGS, s->flags, USE_NONE);

  if (bfd_get_flavour(f) == bfd_target_coff_flavour) {
    printf_maskmute(zSECTIONHDRCOFF_FLAGS, s->flags, USE_NONE);
  } else if (bfd_get_flavour(f) == bfd_target_elf_flavour) {
    printf_maskmute(zSECTIONHDRELF_FLAGS, s->flags, USE_NONE);
  }

  printf_maskmute(zSECTIONHDR3_FLAGS, s->flags, USE_NONE);

  if (bfd_get_arch(f) == bfd_arch_mep) {
    printf_maskmute(zSECTIONHDRARCHMEP_FLAGS, s->flags, USE_NONE);
  }

  if (s->flags & SEC_LINK_ONCE) {
    printf_pick(zSECTIONHDRLINKDUP, s->flags & SEC_LINK_DUPLICATES, USE_NONE);

    // TBD
    //struct coff_comdat_info* c = bfd_coff_get_comdat_section(f, s);
    //if (c) {
    //  printf(" (COMDAT %s %ld)", c->name, c->symbol);
    //}
  }
  printf_eol();
}

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

static int dump_header(const pbuffer_t p, const poptions_t o, bfd *f) {
  printf_text("FILE HEADER", USE_LT | USE_COLON | USE_EOL);

  printf_text("Architecture", USE_LT | USE_TAB | USE_COLON | SET_PAD(18));
  printf_text(bfd_printable_arch_mach (bfd_get_arch(f), bfd_get_mach(f)), USE_SPACE | USE_EOL);

  printf_text("Flags", USE_LT | USE_TAB | USE_COLON | SET_PAD(18));
  printf_nice(f->flags & ~BFD_FLAGS_FOR_BFD_USE_MASK, USE_FHEX32 | USE_COLON);
  printf_mask(zHAS_FLAGS, f->flags, USE_EOL);
  printf_text("Start Address", USE_LT | USE_TAB | USE_COLON | SET_PAD(18));
  printf_nice(f->start_address, USE_FHEX64 | USE_EOL);

  return 0;
}

static int dump_privatehdr(const pbuffer_t p, const poptions_t o, bfd *f) {
  if (!bfd_print_private_bfd_data(f, stdout)) {
    printf("%s: WARNING: private Headers incomplete: %s", o->prgname, bfd_errmsg(bfd_get_error()));
    return 1;
  }
  printf_eol();

  return 0;
}

static int dump_sectionhdr(const pbuffer_t p, const poptions_t o, bfd *f) {
  size_t max_name_size = 20;

  printf_text("SECTIONS", USE_LT | USE_COLON | USE_EOL);
  bfd_map_over_sections(f, callback_find_max_sectionhdr_name, &max_name_size);

  printf_text("Idx", USE_LT | USE_SPACE);
  printf_text("Name", USE_LT | USE_SPACE | SET_PAD(max_name_size));
  printf_text("Size", USE_LT | USE_SPACE | SET_PAD(9));
  printf_text("VMA", USE_LT | USE_SPACE | SET_PAD(17));
  printf_text("LMA", USE_LT | USE_SPACE | SET_PAD(17));
  printf_text("File Off", USE_LT | USE_SPACE);
  printf_text("Algn", USE_LT | USE_SPACE);
  printf_text("Flags", USE_LT | USE_SPACE | USE_EOL);

  bfd_map_over_sections(f, callback_sectionhdr, &max_name_size);

  printf_eol();
  return 0;
}

static int dump_symbols(const pbuffer_t p, const poptions_t o, bfd *f, const int mode) {
  if (MODE_SYMBOLS_DYNAMIC == mode) {
    printf_text("DYNAMIC SYMBOL TABLE", USE_LT | USE_COLON | USE_EOL);
  } else if (MODE_SYMBOLS == mode) {
    printf_text("SYMBOL TABLE", USE_LT | USE_COLON | USE_EOL);
  } else {
    return -1;
  }

  pbuffer_t ps = get_symbols(p,  f, mode);
  if (NULL == ps || 0 == ps->size) {
    printf_text("no symbols", USE_LT | USE_EOL);
  } else {
    asymbol **cs = ps->data;
    for (size_t i = 0; i < ps->size; ++i) {
      bfd *cf = NULL;

      if (NULL == *cs) {
        printf("%s: WARNING: no information for symbol number %ld\n", o->prgname, i);
      } else if ((cf = bfd_asymbol_bfd(*cs)) == NULL) {
        printf("%s: WARNING: could not determine the type of symbol number %ld\n", o->prgname, i);
      } else {
        bfd_print_symbol(cf, stdout, *cs, bfd_print_symbol_all);
        printf_eol();
      }

      ++cs;
    }
  }

  printf_eol();
  return 0;
}

static int dump_sections(const pbuffer_t p, const poptions_t o, bfd *f) {
  printf_text("SECTION CONTENTS", USE_LT | USE_COLON | USE_EOL);
  bfd_map_over_sections(f, callback_section_data, NULL);
  return 0;
}

static int dump_disassemble(const pbuffer_t p, const poptions_t o, bfd *f) {
  struct disassemble_info di;
  objdump_info_t oi;

  memset(&di, 0, sizeof(struct disassemble_info));
  memset(&oi, 0, sizeof(objdump_info_t));

  di.application_data = &oi;

  oi.action = o->action;

  bfd_map_over_sections(f, callback_disassemble, &di);
  return 0;
}

static int dump_debugging(const pbuffer_t p, const poptions_t o, bfd *f) {
  return 0;
}

static int do_object(const pbuffer_t p, const poptions_t o, bfd *f) {
  printf_text("FILE DETAILS", USE_LT | USE_COLON | USE_EOL);
  printf_text("File Name", USE_LT | USE_TAB | USE_COLON | SET_PAD(18));
  printf_text(bfd_get_filename(f), USE_SPACE | USE_EOL);
  printf_text("File Format", USE_LT | USE_TAB | USE_COLON | SET_PAD(18));
  printf_text(f->xvec->name, USE_SPACE | USE_EOL);
  printf_eol();

  if (o->action & OPTOBJDUMP_FILEHEADER)        dump_header(p, o, f);
  if (o->action & OPTOBJDUMP_PRIVATEHEADER)     dump_privatehdr(p, o, f);
  if (o->action & OPTOBJDUMP_SECTIONHEADER)     dump_sectionhdr(p, o, f);
  if (o->action & OPTOBJDUMP_SYMBOLS)           dump_symbols(p, o, f, MODE_SYMBOLS);
  if (o->action & OPTOBJDUMP_DYNAMICSYMBOLS)    dump_symbols(p, o, f, MODE_SYMBOLS_DYNAMIC);
  if (o->action & OPTOBJDUMP_SECTIONS)          dump_sections(p, o, f);
  if (o->action & OPTOBJDUMP_DISASSEMBLE)       dump_disassemble(p, o, f);
  if (o->action & OPTOBJDUMP_DEBUGGING)         dump_debugging(p, o, f);

  return 0;
}

static int do_coredump(const pbuffer_t p, const poptions_t o, bfd *f) {
  printf("%s is an core dump\n", o->inpname);
  return -1;
}

static int do_archive(const pbuffer_t p, const poptions_t o, bfd *f) {
  printf("%s is an archive\n", o->inpname);
  bfd *lf = NULL;

  for ( ; ; ) {
    bfd *nf = bfd_openr_next_archived_file(f, nf);
    if (nf) {
      printf("%s\n", bfd_get_filename(nf));
    } else {
      bfd_close(lf);

      if (bfd_get_error() != bfd_error_no_more_archived_files) {
        printf("%s: FATAL: %s\n", o->prgname, bfd_errmsg(bfd_get_error()));
        return -1;
      }

      return 0;
    }

    bfd_close(lf);
    lf = nf;
  }

  return -1;
}

int objdump(const pbuffer_t p, const poptions_t o) {
  int r = 0;

  bfd_set_error_program_name(o->prgname);

  if (BFD_INIT_MAGIC != bfd_init()) {
    printf("%s: FATAL: libbfd ABI mismatch", o->prgname);
    exit(1);
  }

  const char *target = "x86_64-pc-linux-gnu";

  if (!bfd_set_default_target(target)) {
    printf("%s: FATAL: can't set BFD default target to '%s': %s", o->prgname, target, bfd_errmsg(bfd_get_error()));
    exit(1);
  }

  bfd *f = bfd_openr(o->inpname, NULL);
  if (f) {
    /* Decompress sections unless dumping the section contents. */
    if (0 == (o->action & OPTOBJDUMP_SECTIONS)) {
      f->flags |= BFD_DECOMPRESS;
    }

    if (bfd_check_format(f, bfd_archive)) {
      r = do_archive(p, o, f);
    } else if (bfd_check_format(f, bfd_object)) {
      r = do_object(p, o, f);
    } else if (bfd_check_format(f, bfd_core)) {
      r = do_coredump(p, o, f);
    } else {
      printf("%s: WARNING: %s is an unknown format!\n", o->prgname, o->inpname);
    }
    bfd_close(f);
  }

  return r;
}

