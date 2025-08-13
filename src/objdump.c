#include <bfd.h>
#include <string.h>
#include <dis-asm.h>

#include "printf.h"
#include "opcode.h"
#include "ocdwarf.h"
#include "objdump.h"

static void callback_disassemble(handle_t p, handle_t section, unknown_t param) {
  const poptions_t o = CAST(poptions_t, param);

  /* Sections that do not contain machine code are not normally disassembled. */
  if ((ocget_flags(section) & SEC_HAS_CONTENTS) == 0) return;
  if (o && 0 == (o->action & OPTOBJDUMP_DISASSEMBLE_ALL) && 0 == (ocget_flags(section) & SEC_CODE)) return;
  if (0 == ocget_size(section)) return;

  uint64_t soffset = ocget_soffset(p, section);
  uint64_t eoffset = ocget_eoffset(p, section);
  if (soffset >= eoffset) return;

  printf_text("Disassembly of section", USE_LT);
  printf_text(ocget_name(section), USE_LT | USE_SPACE | USE_SQ);
  printf_text("at offset", USE_LT | USE_SPACE);
  printf_nice(ocget_position(section) + soffset, USE_FHEX16 | USE_COLON | USE_EOL);

  ocdisassemble_run(p, section);

  printf_eol();
}

static void callback_dwarf(handle_t p, handle_t section, unknown_t param) {
  if (!ocdwarf_isneeded(p, section)) return;

  uint64_t soffset = ocget_soffset(p, section);

  printf_text("Contents of section", USE_LT);
  printf_text(ocget_name(section), USE_LT | USE_SPACE | USE_SQ);
  printf_text("at offset", USE_LT | USE_SPACE);
  printf_nice(ocget_position(section) + soffset, USE_FHEX16 | USE_COLON | USE_EOL);

  ocdwarf_run(p, section);

  printf_eol();
}

static void callback_reloc(handle_t p, handle_t section, unknown_t param) {
  if ((ocget_flags(section) & SEC_HAS_CONTENTS) == 0) return;
  if ((ocget_flags(section) & SEC_RELOC) == 0) return;

  printf_text("RELOCATION RECORDS FOR", USE_LT);
  printf_text(ocget_name(section), USE_LT | USE_SPACE | USE_SB | USE_COLON | USE_EOL);
  printf_text("OFFSET TYPE VALUE", USE_LT | USE_EOL);
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
  printf_nice(ocget_position(section) + soffset, USE_FHEX16 | USE_COLON | USE_EOL);

  bfd_byte *data = NULL;
  if (bfd_get_full_section_contents(ocgetbfd(p), ocgetshdr(section), &data)) {
    printf_data(data, dsize, soffset + ocget_vmaddress(section), USE_HEXDUMP);
  } else {
    printf_w("reading section %s failed because: %s.", ocget_name(section), bfd_errmsg(bfd_get_error()));
  }

  printf_eol();
}

static void callback_programhdr(handle_t p, handle_t phdr, unknown_t param) {
  size_t name_size = *CAST(size_t*, param);
  int n = 0;
  n += printf_pick(zPHDRTYPE, ocget_type(phdr), USE_LT | USE_TAB | SET_PAD(name_size));

  n += printf_nice(ocget_offset(phdr), USE_LHEX64);
  n += printf_nice(ocget_vmaddress(phdr), USE_LHEX64);
  n += printf_nice(ocget_paddress(phdr), USE_LHEX64);
  n += printf_nice(ocget_alignment(phdr), USE_POWER2);
  n += printf_nice(ocget_size(phdr), USE_LHEX64);
  n += printf_nice(ocget_memsize(phdr), USE_LHEX64);

  n += printf_nice(ocget_flags(phdr) & PF_R ? 'r' : '-', USE_SPACE | USE_CHAR);
  n += printf_nice(ocget_flags(phdr) & PF_W ? 'w' : '-', USE_CHAR);
  n += printf_nice(ocget_flags(phdr) & PF_X ? 'x' : '-', USE_CHAR);

  n += printf_eol();
}

static void callback_sectionhdr(handle_t p, handle_t shdr, unknown_t param) {
  size_t name_size = *CAST(size_t*, param);
  asection* s = ocget(shdr, MODE_OCSHDR);

  uint64_t flags = ocget_flags(shdr);

  /* Ignore linker created section.  See elfNN_ia64_object_p in bfd/elfxx-ia64.c.  */
  if (flags & SEC_LINKER_CREATED) return;

  printf_nice(s->index, USE_TAB | USE_DEC2);
  printf_text(ocget_name(shdr), USE_LT | USE_SPACE | SET_PAD(name_size));
  printf_nice(ocget_size(shdr) / ocget_opb(p, shdr), USE_LHEX32);
  printf_nice(ocget_vmaddress(shdr), USE_LHEX64);
  printf_nice(ocget_lmaddress(shdr), USE_LHEX64);
  printf_nice(ocget_position(shdr), USE_LHEX32);
  printf_nice(ocget_alignment(shdr), USE_POWER2);

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
  printf_nice(ocget_flags(p) & ~BFD_FLAGS_FOR_BFD_USE_MASK, USE_FHEX32);
  printf_mask(zHAS_FLAGS, ocget_flags(p), USE_EOL);
  printf_text("Start Address", USE_LT | USE_TAB | USE_COLON | SET_PAD(18));
  printf_nice(ocget_saddress(p), USE_FHEX64 | USE_EOL);

  return 0;
}

static int dump_privatehdr(const handle_t p, const poptions_t o) {
  int n = 0;
  size_t max_name_size = 20;
// bfd/elf.c:1648:_bfd_elf_print_private_bfd_data (bfd *abfd, void *farg)
  if (MODE_ISANY(o->action, OPTPROGRAM_VERBOSE)) {
    if (!bfd_print_private_bfd_data(ocgetbfd(p), stdout)) {
      printf_w("private Headers incomplete: %s.", bfd_errmsg(bfd_get_error()));
      return ECODE_BFD;
    }
    n += printf_eol();
  } else {
    n += printf_text("PROGRAM HEADER", USE_LT | USE_COLON | USE_EOL);
    n += printf_text("Type", USE_LT | USE_TAB | SET_PAD(max_name_size));
    n += printf_text("Offset", USE_LT | USE_SPACE | SET_PAD(17));
    n += printf_text("VirtAddr", USE_LT | USE_SPACE | SET_PAD(17));
    n += printf_text("PhysAddr", USE_LT | USE_SPACE | SET_PAD(17));
    n += printf_text("Align", USE_LT | USE_SPACE | SET_PAD(6));
    n += printf_text("FileSiz", USE_LT | USE_SPACE | SET_PAD(17));
    n += printf_text("MemSiz", USE_LT | USE_SPACE | SET_PAD(17));
    n += printf_text("Flg", USE_LT | USE_SPACE);
    n += printf_eol();

    ocdo_programs(p, callback_programhdr, &max_name_size);
    n += printf_eol();

    n += printf_text("DYNAMIC SECTION", USE_LT | USE_COLON | USE_EOL);
    n += printf_eol();

    n += printf_text("VERSION REFERENCES", USE_LT | USE_COLON | USE_EOL);
    n += printf_eol();
  }

  return n;
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
  printf_text("Algn", USE_LT | USE_SPACE | SET_PAD(6));
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

static int dump_relocdynamic(const handle_t p, const poptions_t o) {
  const int MAXSIZE1 = 17;
  const int MAXSIZE2 = 24;

  printf_text("DYNAMIC RELOCATION RECORDS", USE_LT | USE_EOL);

  pbuffer_t ps = ocget(p, OPCODE_SYMBOLS_DYNAMICRELOC);
  if (NULL == ps || 0 == ps->size) {
    printf_text("no symbols", USE_LT | USE_EOL);
  } else {
    printf_text("OFFSET", USE_LT | SET_PAD(MAXSIZE1));
    printf_text("TYPE", USE_LT | SET_PAD(MAXSIZE2));
    printf_text("VALUE", USE_LT | USE_EOL);

    arelent **p0 = CAST(arelent **, ps->data);
    for (size_t i = 0; i < ps->size; ++i, ++p0) {
      arelent *p1 = *p0;
      if (p1) {
        printf_nice(p1->address, USE_LHEX64 | USE_NOSPACE);
        if (p1->howto) {
          if (p1->howto->name) {
            printf_text(p1->howto->name, USE_LT | USE_SPACE | SET_PAD(MAXSIZE2));
          } else {
            printf_nice(p1->howto->type, USE_DEC | SET_PAD(MAXSIZE2));
          }
        } else {
          printf_text("*unknown*", USE_LT | USE_SPACE | SET_PAD(MAXSIZE2));
        }

        const char *symname = NULL;
        const char *secname = NULL;
        if (p1->sym_ptr_ptr && *p1->sym_ptr_ptr) {
          symname = (*(p1->sym_ptr_ptr))->name;
          secname = (*(p1->sym_ptr_ptr))->section->name;
        }

        if (symname) {
          printf_text(symname, USE_LT | USE_SPACE);
        } else if (secname) {
          printf_text(secname, USE_LT | USE_SPACE | USE_SB);
        } else {
          printf_text("*unknown*", USE_LT | USE_SPACE | USE_SB);
        }

        if (p1->addend) {
          printf_nice(p1->addend, USE_SFHEX64 | USE_NOSPACE);
        }
        printf_eol();
      }
    }
  }

  printf_eol();
  return 0;
}

static int dump_reloc(const handle_t p, const poptions_t o) {
  ocdo_sections(p, callback_reloc, NULL);
  return 0;
}

static int dump_sections(const handle_t p, const poptions_t o) {
  printf_text("SECTION CONTENTS", USE_LT | USE_COLON | USE_EOL);
  ocdo_sections(p, callback_sections, NULL);
  return 0;
}

static int dump_disassemble(const handle_t p, const poptions_t o) {
  ocdisassemble_open(p, o);
  ocdo_sections(p, callback_disassemble, o);
  ocdisassemble_close(p);
  return 0;
}

static int dump_dwarf(const handle_t p, const poptions_t o) {
  ocdwarf_open(p, o);
  ocdo_sections(p, callback_dwarf, o);
  ocdwarf_close(p);
  return 0;
}

static int do_object(const handle_t p, const poptions_t o) {
  printf_text("FILE DETAILS", USE_LT | USE_COLON | USE_EOL);
  printf_text("File Name", USE_LT | USE_TAB | USE_COLON | SET_PAD(18));
  printf_text(ocget_name(p), USE_SPACE | USE_EOL);
  printf_text("File Format", USE_LT | USE_TAB | USE_COLON | SET_PAD(18));
  printf_text(ocget_fileformat(p), USE_SPACE | USE_EOL);
  printf_eol();

  if (MODE_ISANY(o->action, OPTOBJDUMP_FILE_HEADER))        dump_header(p, o);
  if (MODE_ISANY(o->action, OPTOBJDUMP_PRIVATE_HEADER))     dump_privatehdr(p, o);
  if (MODE_ISANY(o->action, OPTOBJDUMP_SECTION_HEADER))     dump_sectionhdr(p, o);
  if (MODE_ISANY(o->action, OPTOBJDUMP_SYMBOLS))            dump_symbols(p, o, OPCODE_SYMBOLS);
  if (MODE_ISANY(o->action, OPTOBJDUMP_DYNAMIC_SYMBOLS))    dump_symbols(p, o, OPCODE_SYMBOLS_DYNAMIC);
  if (MODE_ISANY(o->action, OPTOBJDUMP_RELOC))              dump_reloc(p, o);
  if (MODE_ISANY(o->action, OPTOBJDUMP_DYNAMIC_RELOC))      dump_relocdynamic(p, o);
  if (MODE_ISANY(o->action, OPTOBJDUMP_SECTIONS))           dump_sections(p, o);
  if (MODE_ISANY(o->action, OPTPROGRAM_DISASSEMBLE))        dump_disassemble(p, o);
  if (MODE_ISANY(o->action, OPTDEBUGELF_DEBUGGING))         dump_dwarf(p, o);

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

