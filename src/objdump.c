#include <bfd.h>
#include <string.h>
#include <dis-asm.h>

#include "printf.h"
#include "objdump.h"

#include "static/has_flags.ci"
#include "static/sectionhdr_flags.ci"

#define DEFAULT_SKIP_ZEROES            (8)
#define DEFAULT_SKIP_ZEROES_AT_END     (3)

typedef struct objdump_info_s {
  imode_t  action;

} objdump_info_t, *pobjdump_info_t;

static bfd_vma get_signadjustment(bfd *f) {
  /* If the target used signed addresses then we must make
     sure that we sign extend the value that we calculate. */
  bfd_vma sign_adjust = 0;
  if (1 == bfd_get_sign_extend_vma(f)) {
    sign_adjust = (bfd_vma) 1 << (bfd_get_arch_size(f) - 1);
  }

  return sign_adjust;
}

static bfd_vma fix_signadjustment(bfd_vma addr, bfd_vma sign_adjust) {
  return ((addr & ((sign_adjust << 1) - 1)) ^ sign_adjust) - sign_adjust;
}

static void callback_disassemble(handle_t p, handle_t section, unknown_t param) {
//  struct disassemble_info *pdi = CAST(struct disassemble_info *, p);
  const poptions_t o = CAST(poptions_t, param);

  /* Sections that do not contain machine code are not normally disassembled. */
  if ((ocget_flags(section) & SEC_HAS_CONTENTS) == 0) return;
  if (o && 0 == (o->action & OPTOBJDUMP_DISASSEMBLE_ALL) && 0 == (ocget_flags(section) & SEC_CODE)) return;
  if (0 == ocget_size(section)) return;

  uint64_t soffset = ocget_soffset(p, section);
  uint64_t eoffset = ocget_eoffset(p, section);
  if (soffset >= eoffset) return;

//  bfd_vma sign_adjust = get_signadjustment(f);

  printf_text("Disassembly of section", USE_LT);
  printf_text(ocget_name(section), USE_LT | USE_SPACE | USE_SQ);
  printf_text("at offset", USE_LT | USE_SPACE);
  printf_nice(ocget_positon(section) + soffset, USE_FHEX16 | USE_COLON | USE_EOL);

//  while (soffset < eoffset) {
//    bfd_vma addr = fix_signadjustment(s->vma + soffset, sign_adjust);
//
//    bfd_vma noffset = eoffset;
//    if (sym && bfd_asymbol_value(sym) > addr)      noffset = bfd_asymbol_value(sym) - s->vma;
//    else if (NULL == nextsym)                      noffset = eoffset;
//    else                                           noffset = bfd_asymbol_value(nextsym) - s->vma;
//
//    if (noffset > eoffset || noffset <= soffset)   noffset = eoffset;
//
//    soffset = noffset;
//  }

  printf_eol();
}

static void callback_sections(handle_t p, handle_t section, unknown_t param) {
  if ((ocget_flags(section) & SEC_HAS_CONTENTS) == 0) return;

  bfd_size_type dsize = ocget_size(section);
  if (0 == dsize) return;

  uint64_t soffset = ocget_soffset(p, section);
  uint64_t eoffset = ocget_eoffset(p, section);
  if (soffset >= eoffset) return;

  printf_text("Contexts of section", USE_LT | USE_SPACE);
  printf_text(ocget_name(section), USE_LT | USE_SPACE | USE_SQ);
  printf_text("at offset", USE_LT | USE_SPACE);
  printf_nice(ocget_positon(section) + soffset, USE_FHEX16 | USE_COLON | USE_EOL);

  bfd_byte *data = NULL;
  if (bfd_get_full_section_contents(ocgetbfd(p), ocgetsec(section), &data)) {
    printf_data(data, dsize, soffset + ocget_vmaddress(section), USE_HEXDUMP);
  } else {
    printf_w("reading section %s failed because: %s.", ocget_name(section), bfd_errmsg(bfd_get_error()));
  }

  printf_eol();
}

static void callback_sectionhdr(handle_t p, handle_t section, unknown_t param) {
  size_t name_size = *CAST(size_t*, param);
  asection* s = ocget(section, MODE_OPSECTION);

  uint64_t flags = ocget_flags(section);

  /* Ignore linker created section.  See elfNN_ia64_object_p in bfd/elfxx-ia64.c.  */
  if (flags & SEC_LINKER_CREATED) return;

  printf_nice(s->index, USE_TAB | USE_DEC2);
  printf_text(ocget_name(section), USE_LT | USE_SPACE | SET_PAD(name_size));
  printf_nice(ocget_size(section) / ocget_opb(p, section), USE_LHEX32);
  printf_nice(ocget_vmaddress(section), USE_LHEX64);
  printf_nice(ocget_lmaddress(section), USE_LHEX64);
  printf_nice(ocget_positon(section), USE_LHEX32);
  printf(" 2**%lu", ocget_alignment(section));

  printf_maskmute(zSECTIONHDR1_FLAGS, flags, USE_LT);

  if (bfd_get_arch(ocgetbfd(p)) == bfd_arch_tic54x) {
    printf_maskmute(zSECTIONHDRTIC54X_FLAGS, flags, USE_LT);
  }

  printf_maskmute(zSECTIONHDR2_FLAGS, flags, USE_LT);

  if (bfd_get_flavour(ocgetbfd(p)) == bfd_target_coff_flavour) {
    printf_maskmute(zSECTIONHDRCOFF_FLAGS, flags, USE_LT);
  } else if (bfd_get_flavour(ocgetbfd(p)) == bfd_target_elf_flavour) {
    printf_maskmute(zSECTIONHDRELF_FLAGS, flags, USE_LT);
  }

  printf_maskmute(zSECTIONHDR3_FLAGS, flags, USE_LT);

  if (bfd_get_arch(ocgetbfd(p)) == bfd_arch_mep) {
    printf_maskmute(zSECTIONHDRARCHMEP_FLAGS, flags, USE_LT);
  }

  if (flags & SEC_LINK_ONCE) {
    printf_pick(zSECTIONHDRLINKDUP, flags & SEC_LINK_DUPLICATES, USE_LT);

    // TBD
    //struct coff_comdat_info* c = bfd_coff_get_comdat_section(f, s);
    //if (c) {
    //  printf(" (COMDAT %s %ld)", c->name, c->symbol);
    //}
  }
  printf_eol();
}

static int dump_header(const handle_t p, const poptions_t o) {
  printf_text("FILE HEADER", USE_LT | USE_COLON | USE_EOL);

  printf_text("Architecture", USE_LT | USE_TAB | USE_COLON | SET_PAD(18));
  printf_text(bfd_printable_arch_mach(bfd_get_arch(ocgetbfd(p)), bfd_get_mach(ocgetbfd(p))), USE_SPACE | USE_EOL);

  printf_text("Flags", USE_LT | USE_TAB | USE_COLON | SET_PAD(18));
  printf_nice(ocget_flags(p) & ~BFD_FLAGS_FOR_BFD_USE_MASK, USE_FHEX32 | USE_COLON);
  printf_mask(zHAS_FLAGS, ocget_flags(p), USE_EOL);
  printf_text("Start Address", USE_LT | USE_TAB | USE_COLON | SET_PAD(18));
  printf_nice(ocget_saddress(p), USE_FHEX64 | USE_EOL);

  return 0;
}

static int dump_privatehdr(const handle_t p, const poptions_t o) {
  if (!bfd_print_private_bfd_data(ocgetbfd(p), stdout)) {
    printf_w("private Headers incomplete: %s.", bfd_errmsg(bfd_get_error()));
    return 1;
  }
  printf_eol();

  return 0;
}

static int dump_sectionhdr(const handle_t p, const poptions_t o) {
  size_t max_name_size = ocget_maxsectionnamesize(p) + 2;

  printf_text("SECTIONS", USE_LT | USE_COLON | USE_EOL);
  printf_text("Idx", USE_LT | USE_SPACE);
  printf_text("Name", USE_LT | USE_SPACE | SET_PAD(max_name_size));
  printf_text("Size", USE_LT | USE_SPACE | SET_PAD(9));
  printf_text("VMA", USE_LT | USE_SPACE | SET_PAD(17));
  printf_text("LMA", USE_LT | USE_SPACE | SET_PAD(17));
  printf_text("File Off", USE_LT | USE_SPACE);
  printf_text("Algn", USE_LT | USE_SPACE);
  printf_text("Flags", USE_LT | USE_SPACE | USE_EOL);

  ocdo_sections(p, callback_sectionhdr, &max_name_size);

  printf_eol();
  return 0;
}

static int dump_symbols(const handle_t p, const poptions_t o, const imode_t mode) {
  if (OPCODE_SYMBOLS_DYNAMIC == mode) {
    printf_text("DYNAMIC SYMBOL TABLE", USE_LT | USE_COLON | USE_EOL);
  } else if (OPCODE_SYMBOLS == mode) {
    printf_text("SYMBOL TABLE", USE_LT | USE_COLON | USE_EOL);
  } else {
    return -1;
  }

  pbuffer_t ps = ocget(p, mode);
  if (NULL == ps || 0 == ps->size) {
    printf_text("no symbols", USE_LT | USE_EOL);
  } else {
    asymbol **cs = ps->data;
    for (size_t i = 0; i < ps->size; ++i) {
      bfd *cf = NULL;

      if (NULL == *cs) {
        printf_w("no information for symbol number %ld.", i);
      } else if ((cf = bfd_asymbol_bfd(*cs)) == NULL) {
        printf_w("could not determine the type of symbol number %ld.", i);
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

static int dump_sections(const handle_t p, const poptions_t o) {
  printf_text("SECTION CONTENTS", USE_LT | USE_COLON | USE_EOL);
  ocdo_sections(p, callback_sections, NULL);
  return 0;
}

static int dump_disassemble(const handle_t p, const poptions_t o) {
//  MALLOCS(struct disassemble_info, di);
//  MALLOCS(objdump_info_t, oi);

//  oi.action = o->action;

//  di.application_data = &oi;
//  di.arch = bfd_get_arch(f);
//  di.mach = bfd_get_mach(f);
//  di.flavour = bfd_get_flavour(f);
//  di.octets_per_byte = bfd_octets_per_byte(f, NULL);
//  di.skip_zeroes = DEFAULT_SKIP_ZEROES;
//  di.skip_zeroes_at_end = DEFAULT_SKIP_ZEROES_AT_END;
//  di.disassembler_options = NULL;  // -M options, --disassembler-options=options
//  di.disassembler_needs_relocs = FALSE;

//  if (bfd_big_endian(f))               di.endian_code = di.display_endian = di.endian = BFD_ENDIAN_BIG;
//  else if (bfd_little_endian(f))       di.endian_code = di.display_endian = di.endian = BFD_ENDIAN_LITTLE;
//  else                                 di.endian_code = di.endian = BFD_ENDIAN_UNKNOWN;

  ocdo_sections(p, callback_disassemble, o);
  return 0;
}

static int dump_debugging(const handle_t p, const poptions_t o) {
  return 0;
}

static int do_object(const handle_t p, const poptions_t o) {
  printf_text("FILE DETAILS", USE_LT | USE_COLON | USE_EOL);
  printf_text("File Name", USE_LT | USE_TAB | USE_COLON | SET_PAD(18));
  printf_text(ocget_name(p), USE_SPACE | USE_EOL);
  printf_text("File Format", USE_LT | USE_TAB | USE_COLON | SET_PAD(18));
  printf_text(ocget_fileformat(p), USE_SPACE | USE_EOL);
  printf_eol();

  if (o->action & OPTOBJDUMP_FILE_HEADER)        dump_header(p, o);
  if (o->action & OPTOBJDUMP_PRIVATE_HEADER)     dump_privatehdr(p, o);
  if (o->action & OPTOBJDUMP_SECTION_HEADER)     dump_sectionhdr(p, o);
  if (o->action & OPTOBJDUMP_SYMBOLS)            dump_symbols(p, o, OPCODE_SYMBOLS);
  if (o->action & OPTOBJDUMP_DYNAMIC_SYMBOLS)    dump_symbols(p, o, OPCODE_SYMBOLS_DYNAMIC);
  if (o->action & OPTOBJDUMP_SECTIONS)           dump_sections(p, o);
  if (o->action & OPTOBJDUMP_DISASSEMBLE)        dump_disassemble(p, o);
  if (o->action & OPTOBJDUMP_DEBUGGING)          dump_debugging(p, o);

  return 0;
}

static int do_coredump(const handle_t p, const poptions_t o) {
  printf("%s is an core dump\n", o->inpname);
  return -1;
}

static int do_archive(const handle_t p, const poptions_t o) {
  printf("%s is an archive\n", o->inpname);
  bfd *lf = NULL;

  for ( ; ; ) {
    bfd *nf = bfd_openr_next_archived_file(ocgetbfd(p), nf);
    if (nf) {
      printf("%s\n", bfd_get_filename(nf));
    } else {
      bfd_close(lf);

      if (bfd_get_error() != bfd_error_no_more_archived_files) {
        printf_e("%s", bfd_errmsg(bfd_get_error()));
        return -1;
      }

      return 0;
    }

    bfd_close(lf);
    lf = nf;
  }

  return -1;
}

int objdump(const handle_t p, const poptions_t o) {
  int r = 0;

  /* Decompress sections unless dumping the section contents. */
  //if (0 == (o->action & OPTOBJDUMP_SECTIONS)) {
  //  f->flags |= BFD_DECOMPRESS;
  //}

  if (isarchive(p)) {
    r = do_archive(p, o);
  } else if (isobject(p)) {
    r = do_object(p, o);
  } else if (iscoredump(p)) {
    r = do_coredump(p, o);
  } else {
    printf_w("%s is an unknown format!", o->inpname);
  }

  return r;
}

