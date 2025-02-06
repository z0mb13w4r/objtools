#include <bfd.h>

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

int objdump(const pbuffer_t p, const poptions_t o) {
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

  return -1;
}

