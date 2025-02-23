#include <bfd.h>
#include <errno.h>
#include <string.h>
#include <dis-asm.h>

#include "printf.h"
#include "objhash.h"

static int dump_create(const pbuffer_t p, const poptions_t o, bfd *f) {
  return 0;
}

int objhash(const pbuffer_t p0, const poptions_t o) {
  printf("'%s' == '%s'\n", o->inpname0, o->inpname1);

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
      goto objhash_die;
    }
  }

  bfd *f1 = NULL;
  if (p1) {
    f1 = bfd_openr(o->inpname1, NULL);
    if (NULL == f1) {
      printf("%s: FATAL: BFD can't load into memory '%s': %s", o->prgname, o->inpname1, bfd_errmsg(bfd_get_error()));
      goto objhash_die;
    }

    if (!bfd_check_format(f1, bfd_archive) &&
        !bfd_check_format(f1, bfd_object) &&
        !bfd_check_format(f1, bfd_core)) {
      printf("%s: FATAL: %s is an unknown format!\n", o->prgname, o->inpname1);
      goto objhash_die;
    }
  }

  if (p0) r = dump_create(p0, o, f0);
  if (p1) r = dump_create(p1, o, f1);

  if (f0) bfd_close(f0);
  if (f1) bfd_close(f1);
  destroy(p1);

  return r;

objhash_die:

  if (f0) bfd_close(f0);
  if (f1) bfd_close(f1);
  destroy(p1);

  return -1;
}

