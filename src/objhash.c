#include <bfd.h>
#include <errno.h>
#include <string.h>

#include "printf.h"
#include "objhash.h"

static void callback_hashsections(bfd *f, asection *s, void *p) {
  pbuffer_t pp = CAST(pbuffer_t, p);

  /* Ignore linker created section.  See elfNN_ia64_object_p in bfd/elfxx-ia64.c.  */
  if (s->flags & SEC_LINKER_CREATED) return;

  bfd_byte *data = NULL;
  if (bfd_malloc_and_get_section(f, s, &data)) {
    unsigned char md[SHA256_DIGEST_LENGTH]; // 32 bytes
    if (!sha256(data, bfd_section_size(s), md)) {
      printf_data(md, SHA256_DIGEST_LENGTH, 0, USE_HEX);
    }
  } else {
    printf("WARNING: could not retrieve section '%s' contents from '%s'.\n", bfd_section_name(s), pp->note);
  }

  free(data);


  size_t max_name_size = 20;

  printf_nice(s->index, USE_TAB | USE_DEC2);
  printf_text(bfd_section_name(s), USE_LT | USE_SPACE | SET_PAD(max_name_size));
  printf_nice(bfd_section_size(s) / bfd_octets_per_byte(f, s), USE_LHEX32);
  printf_nice(bfd_section_vma(s), USE_LHEX64);
  printf_nice(s->lma, USE_LHEX64);
  printf_nice(s->filepos, USE_LHEX32);
  printf_nice(bfd_section_alignment(s), USE_DEC2);
  printf_nice(s->flags, USE_LHEX32);

  printf_text(pp->note, USE_LT | USE_SPACE | USE_EOL);
}

static int dump_create(const pbuffer_t p, const poptions_t o, bfd *f) {
  bfd_map_over_sections(f, callback_hashsections, p);
  return 0;
}

int objhash(const pbuffer_t p0, const poptions_t o) {
  const char *target = "x86_64-pc-linux-gnu";

  int r = 0;
  pbuffer_t p1 = NULL;
  if (o->inpname1[0]) {
    p1 = open(o->inpname1);
    if (p1) {
    } else {
      printf("%s: ERROR: '%s': no such file.\n", o->prgname, o->inpname1);
      goto objhash_die;
    }
  }

  if (p0) {
    bfd_set_error_program_name(o->prgname);

    if (BFD_INIT_MAGIC != bfd_init()) {
      printf("%s: FATAL: libbfd ABI mismatch", o->prgname);
      goto objhash_die;
    }

    if (!bfd_set_default_target(target)) {
      printf("%s: FATAL: can't set BFD default target to '%s': %s", o->prgname, target, bfd_errmsg(bfd_get_error()));
      goto objhash_die;
    }
  }

  bfd *f0 = NULL;
  if (p0) {
    f0 = bfd_openr(o->inpname0, NULL);
    if (NULL == f0) {
      printf("%s: FATAL: BFD can't load into memory '%s': %s", o->prgname, o->inpname0, bfd_errmsg(bfd_get_error()));
      goto objhash_die;
    }

    if (!bfd_check_format(f0, bfd_archive) &&
        !bfd_check_format(f0, bfd_object) &&
        !bfd_check_format(f0, bfd_core)) {
      printf("%s: FATAL: %s is an unknown format!\n", o->prgname, o->inpname0);
      goto objhash_die0;
    }
  }

  bfd *f1 = NULL;
  if (p1) {
    f1 = bfd_openr(o->inpname1, NULL);
    if (NULL == f1) {
      printf("%s: FATAL: BFD can't load into memory '%s': %s", o->prgname, o->inpname1, bfd_errmsg(bfd_get_error()));
      goto objhash_die0;
    }

    if (!bfd_check_format(f1, bfd_archive) &&
        !bfd_check_format(f1, bfd_object) &&
        !bfd_check_format(f1, bfd_core)) {
      printf("%s: FATAL: %s is an unknown format!\n", o->prgname, o->inpname1);
      goto objhash_die1;
    }
  }

  if (p0) r = dump_create(p0, o, f0);
  if (p1) r = dump_create(p1, o, f1);

  if (f0) bfd_close(f0);
  if (f1) bfd_close(f1);
  destroy(p1);

  return r;

objhash_die1:
  if (f1) bfd_close(f1);

objhash_die0:
  if (f0) bfd_close(f0);

objhash_die:
  destroy(p1);

  return -1;
}

