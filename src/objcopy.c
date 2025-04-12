#include <bfd.h>
#include <errno.h>
#include <string.h>
#include <dis-asm.h>

#include "printf.h"
#include "objcopy.h"

static void callback_dumpcreate(bfd *f, asection *s, void *p) {
  poptions_t po = CAST(poptions_t, p);

  /* Ignore linker created section.  See elfNN_ia64_object_p in bfd/elfxx-ia64.c.  */
  if (s->flags & SEC_LINKER_CREATED) return;

  paction_t pp = amalloc();
  if (p) {
    strncpy(pp->outname, po->inpname, NELEMENTS(pp->outname));
    strncat(pp->outname, bfd_section_name(s), NELEMENTS(pp->outname));
    strncpy(pp->secname, bfd_section_name(s), NELEMENTS(pp->secname));

    pp->action = ACT_DUMPSECTION;
    pp->actions = po->actions;
    po->actions = pp;
  }
}

static int dump_create(const pbuffer_t p, const poptions_t o, bfd *f) {
  bfd_map_over_sections(f, callback_dumpcreate, o);
  return 0;
}

static int dump_actions(const pbuffer_t p, const poptions_t o, bfd *f) {
  paction_t x = o->actions;
  while (x) {
    if (ACT_DUMPSECTION == x->action) {
      printf("DUMPING: %s -> %s\n", x->secname, x->outname);

      asection *osec = bfd_get_section_by_name(f, x->secname);
      if (NULL == osec) {
        printf_w("can't dump section '%s' - it does not exist.", x->secname);
      } else if (0 == (bfd_section_flags(osec) & SEC_HAS_CONTENTS)) {
        printf_w("can't dump section '%s' - it has no contents.", x->secname);
      } else {
        bfd_size_type size = bfd_section_size(osec);

        FILE *fp = fopen(x->outname, "wb");
        if (NULL == fp) {
          printf_w("could not create section '%s' dump file '%s'.", x->secname, o->outname);
        } else {
          bfd_byte *data = NULL;
	  if (bfd_malloc_and_get_section(f, osec, &data)) {
            if (size != fwrite(data, 1, size, fp)) {
              printf_e("writing section '%s' contents to '%s' (%s)", x->secname, o->outname, strerror(errno));
            }
          } else {
            printf_w("could not retrieve section '%s' contents.", x->secname);
          }

          fclose(fp);
          free(data);
        }
      }
    }

    x = x->actions;
  }

  return 0;
}

static int do_object(const pbuffer_t p, const poptions_t o, bfd *f) {
  if (o->action & OPTOBJCOPY_DUMP_SECTIONS_ALL)  dump_create(p, o, f);
  if (o->actions)                                dump_actions(p, o, f);
  return 0;
}

static int do_coredump(const pbuffer_t p, const poptions_t o, bfd *f) {
  printf("%s is an core dump\n", o->inpname);
  return -1;
}

static int do_archive(const pbuffer_t p, const poptions_t o, bfd *f) {
  printf("%s is an archive\n", o->inpname);
  return -1;
}

int objcopy(const pbuffer_t p, const poptions_t o) {
  int r = 0;

  bfd_set_error_program_name(o->prgname);

  if (BFD_INIT_MAGIC != bfd_init()) {
    printf_x("libbfd ABI mismatch.");
  }

  const char *target = "x86_64-pc-linux-gnu";

  if (!bfd_set_default_target(target)) {
    printf_x("can't set BFD default target to '%s': %s.", target, bfd_errmsg(bfd_get_error()));
  }

  bfd *f = bfd_openr(o->inpname, NULL);
  if (f) {
    if (bfd_check_format(f, bfd_archive)) {
      r = do_archive(p, o, f);
    } else if (bfd_check_format(f, bfd_object)) {
      r = do_object(p, o, f);
    } else if (bfd_check_format(f, bfd_core)) {
      r = do_coredump(p, o, f);
    } else {
      printf_w("%s is an unknown format!", o->inpname);
    }
    bfd_close(f);
  }

  return r;
}

