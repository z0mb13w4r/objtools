#include <bfd.h>
#include <stdio.h>
#include <malloc.h>

#include "buffer.h"
#include "readelf.h"
#include "options.h"

static char* get_format(bfd *p) {
  if (bfd_check_format(p, bfd_archive))       return "BFD_ARCHIVE";
  else if (bfd_check_format(p, bfd_object))   return "BFD_OBJECT";
  else if (bfd_check_format(p, bfd_core))     return "BFD_CORE";

  return "BFD_UNK";
}

static void do_section(bfd *abfd, const char *name) {
  printf("-- %s\n", name);
  asection *osec = bfd_get_section_by_name(abfd, ".data");
  if (osec) {
    printf("got section\n");
    if ((bfd_section_flags (osec) & SEC_HAS_CONTENTS) == 0) {
      printf("no contents\n");
    } else {
      bfd_size_type size = bfd_section_size (osec);
      printf("section size: %lu\n", size);
    }
  }
}

int main(int argc, char* argv[]) {
  bfd_vma start;
  enum bfd_architecture iarch;
  unsigned int imach;

  options_t o;

  if (0 == get_options(&o, argc, argv)) {
    pbuffer_t p = create(o.inpname);
    if (p) {
      if (OPT_READELF == o.option) {
        readelf(p, &o);
      }
    }

    destroy(p);
  }

  bfd* ibfd = bfd_openr("example", NULL);
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
  }

  return 0;
}

