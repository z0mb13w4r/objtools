#include <bfd.h>
#include <string.h>

#include "printf.h"
#include "objdump.h"

static char* get_format(bfd *p) {
  if (bfd_check_format(p, bfd_archive))       return "BFD_ARCHIVE";
  else if (bfd_check_format(p, bfd_object))   return "BFD_OBJECT";
  else if (bfd_check_format(p, bfd_core))     return "BFD_CORE";

  return "BFD_UNK";
}

static void do_section(bfd *abfd, const char *name) {
  printf("-- %s\n", name);
  asection *osec = bfd_get_section_by_name(abfd, name);
  if (osec) {
    printf("got section\n");
    if ((bfd_section_flags(osec) & SEC_HAS_CONTENTS) == 0) {
      printf("no contents\n");
    } else {
      bfd_size_type size = bfd_section_size(osec);
      printf("section size: %lu\n", size);
    }
  }
}

static int dump_header(const pbuffer_t p, const poptions_t o, bfd *f) {
  printf("Architecture:   %s\n",
    bfd_printable_arch_mach (bfd_get_arch(f), bfd_get_mach(f)));

  printf("Flags:         ");
  printf_nice(f->flags & ~BFD_FLAGS_FOR_BFD_USE_MASK, USE_FHEX32);
  printf(": ");
  if (f->flags & HAS_RELOC)         printf(" HAS_RELOC");
  if (f->flags & EXEC_P)            printf(" EXEC_P");
  if (f->flags & HAS_LINENO)        printf(" HAS_LINENO");
  if (f->flags & HAS_DEBUG)         printf(" HAS_DEBUG");
  if (f->flags & HAS_SYMS)          printf(" HAS_SYMS");
  if (f->flags & HAS_LOCALS)        printf(" HAS_LOCALS");
  if (f->flags & DYNAMIC)           printf(" DYNAMIC");
  if (f->flags & WP_TEXT)           printf(" WP_TEXT");
  if (f->flags & D_PAGED)           printf(" D_PAGED");
  if (f->flags & BFD_IS_RELAXABLE)  printf(" BFD_IS_RELAXABLE");
  printf("\n");
  printf("Start Address: ");
  printf_nice(f->start_address, USE_FHEX64);
  printf("\n\n");

  return 0;
}

static int dump_privatehdr(const pbuffer_t p, const poptions_t o, bfd *f) {
  if (!bfd_print_private_bfd_data(f, stdout)) {
    printf("WARNING: private Headers incomplete: %s", bfd_errmsg(bfd_get_error()));
    return 1;
  }

  return 0;
}

static void callback_section_header(bfd *f, asection *s, void *p) {
  size_t name_size = *((size_t *) p);

  /* Ignore linker created section.  See elfNN_ia64_object_p in bfd/elfxx-ia64.c.  */
  if (s->flags & SEC_LINKER_CREATED) return;

  printf("%3d %-*s", s->index, (int)name_size, bfd_section_name(s));
  printf_nice(bfd_section_size(s) / bfd_octets_per_byte(f, s), USE_LHEX32);
  printf_nice(bfd_section_vma(s), USE_LHEX64);
  printf_nice(s->lma, USE_LHEX64);
  printf_nice(s->filepos, USE_LHEX32);
  printf(" 2**%u", bfd_section_alignment(s));

  if (s->flags & SEC_HAS_CONTENTS)     printf(" CONTENTS");
  if (s->flags & SEC_ALLOC)            printf(" ALLOC");
  if (s->flags & SEC_CONSTRUCTOR)      printf(" CONSTRUCTOR");
  if (s->flags & SEC_LOAD)             printf(" LOAD");
  if (s->flags & SEC_RELOC)            printf(" RELOC");
  if (s->flags & SEC_READONLY)         printf(" READONLY");
  if (s->flags & SEC_CODE)             printf(" CODE");
  if (s->flags & SEC_DATA)             printf(" DATA");
  if (s->flags & SEC_ROM)              printf(" ROM");
  if (s->flags & SEC_DEBUGGING)        printf(" DEBUGGING");
  if (s->flags & SEC_NEVER_LOAD)       printf(" NEVER_LOAD");
  if (s->flags & SEC_EXCLUDE)          printf(" EXCLUDE");
  if (s->flags & SEC_SORT_ENTRIES)     printf(" SORT_ENTRIES");
  if (bfd_get_arch(f) == bfd_arch_tic54x) {
    if (s->flags & SEC_TIC54X_BLOCK)   printf(" BLOCK");
    if (s->flags & SEC_TIC54X_CLINK)   printf(" CLINK");
  }
  if (s->flags & SEC_SMALL_DATA)       printf(" SMALL_DATA");
  if (bfd_get_flavour(f) == bfd_target_coff_flavour) {
    if (s->flags & SEC_COFF_SHARED)    printf(" SHARED");
    if (s->flags & SEC_COFF_NOREAD)    printf(" NOREAD");
  } else if (bfd_get_flavour(f) == bfd_target_elf_flavour) {
    if (s->flags & SEC_ELF_OCTETS)     printf(" OCTETS");
    if (s->flags & SEC_ELF_PURECODE)   printf(" PURECODE");
  }
  if (s->flags & SEC_THREAD_LOCAL)     printf(" THREAD_LOCAL");
  if (s->flags & SEC_GROUP)            printf(" GROUP");
  if (bfd_get_arch(f) == bfd_arch_mep) {
    if (s->flags & SEC_MEP_VLIW)       printf(" VLIW");
  }
  if (s->flags & SEC_LINK_ONCE) {
    if ((s->flags & SEC_LINK_DUPLICATES) == SEC_LINK_DUPLICATES_DISCARD)        printf(" LINK_ONCE_DISCARD");
    if ((s->flags & SEC_LINK_DUPLICATES) == SEC_LINK_DUPLICATES_ONE_ONLY)       printf(" LINK_ONCE_ONE_ONLY");
    if ((s->flags & SEC_LINK_DUPLICATES) == SEC_LINK_DUPLICATES_SAME_SIZE)      printf(" LINK_ONCE_SAME_SIZE");
    if ((s->flags & SEC_LINK_DUPLICATES) == SEC_LINK_DUPLICATES_SAME_CONTENTS)  printf(" LINK_ONCE_SAME_CONTENTS");

    // TBD
    //struct coff_comdat_info* c = bfd_coff_get_comdat_section(f, s);
    //if (c) {
    //  printf(" (COMDAT %s %ld)", c->name, c->symbol);
    //}
  }
  printf("\n");
}

static void callback_find_longest_section_name(bfd *f ATTRIBUTE_UNUSED, asection *s, void *p) {
  /* Ignore linker created section. */
  if (s->flags & SEC_LINKER_CREATED) return;

  size_t *max_name_size = (size_t *) p;
  const char* name = bfd_section_name(s);
  size_t name_size = strlen(name);
  if (name_size > *max_name_size) {
    *max_name_size = name_size;
  }
}

static int dump_sectionhdr(const pbuffer_t p, const poptions_t o, bfd *f) {
  size_t max_name_size = 20;

  printf("Sections:\n");
  bfd_map_over_sections(f, callback_find_longest_section_name, &max_name_size);

  printf("Idx %-*s Size     VMA              LMA              File Off Algn Flags\n", (int)max_name_size, "Name");

  bfd_map_over_sections(f, callback_section_header, &max_name_size);

  return 0;
}

static int do_object(const pbuffer_t p, const poptions_t o, bfd *f) {
  printf("File Name:      %s\n", bfd_get_filename(f));
  printf("File Format:    %s\n", f->xvec->name);
  printf("\n");

  if (o->action & OPTOBJDUMP_FILEHEADER) {
    dump_header(p, o, f);
  }
  if (o->action & OPTOBJDUMP_PRIVATEHEADER) {
    dump_privatehdr(p, o, f);
  }
  if (o->action & (OPTOBJDUMP_SYMBOLS | OPTOBJDUMP_DISASSEMBLE | OPTOBJDUMP_DEBUGGING)) {

  }
  if (o->action & OPTOBJDUMP_SECTIONHEADER) {
    dump_sectionhdr(p, o, f);
  }

  return -1;
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
        printf("%s\n", bfd_errmsg(bfd_get_error()));
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
#if 1
  int r = 0;

  bfd_set_error_program_name(o->prgname);

  if (BFD_INIT_MAGIC != bfd_init()) {
    printf("FATAL: libbfd ABI mismatch");
    exit(1);
  }

  const char *target = "x86_64-pc-linux-gnu";

  if (!bfd_set_default_target(target)) {
    printf("FATAL: can't set BFD default target to '%s': %s", target, bfd_errmsg(bfd_get_error()));
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
      printf("%s is an unknown format!\n", o->inpname);
    }
    bfd_close(f);
  }
#else
  bfd_vma start;
  enum bfd_architecture iarch;
  unsigned int imach;

  if (o) {
    bfd* ibfd = bfd_openr(o->inpname, NULL);
    if (ibfd) {
      printf("size: %lu\n", bfd_get_size(ibfd));
      printf("format: %s\n", get_format(ibfd));
      printf("format: %s\n", bfd_format_string(ibfd->format));
      if (ibfd->sections == NULL) {
        printf("missing sections\n");
      }
      start = bfd_get_start_address (ibfd);
      printf("start: %lu\n", start);

      iarch = bfd_get_arch (ibfd);
      imach = bfd_get_mach (ibfd);
      printf("%d %d\n", iarch, imach);

      do_section(ibfd, ".data");
      do_section(ibfd, ".symtab");
      do_section(ibfd, ".strtab");
      do_section(ibfd, ".shstrtab");

      bfd_close(ibfd);

      return 0;
    }
  }
#endif
  return r;
}

