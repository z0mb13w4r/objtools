#include <bfd.h>

#include "opcode.h"
#include "ocdwarf.h"
#include "options.h"

static int ocdwarf_dodebug_abbrev(handle_t p, handle_t s, handle_t d);
static int ocdwarf_dodebug_aranges(handle_t p, handle_t s, handle_t d);
static int ocdwarf_dodebug_info(handle_t p, handle_t s, handle_t d);
static int ocdwarf_dodebug_line(handle_t p, handle_t s, handle_t d);
static int ocdwarf_dodebug_macroinfo(handle_t p, handle_t s, handle_t d);
static int ocdwarf_dodebug_str(handle_t p, handle_t s, handle_t d);
static int ocdwarf_dodebug_stroffset(handle_t p, handle_t s, handle_t d);
static int ocdwarf_dodebug_types(handle_t p, handle_t s, handle_t d);
static int ocdwarf_doeh_frame(handle_t p, handle_t s, handle_t d);

#include "static/dwarf.ci"

static int ocdwarf_dodebug_abbrev(handle_t p, handle_t s, handle_t d) { return 0; }
static int ocdwarf_dodebug_aranges(handle_t p, handle_t s, handle_t d) { return 0; }
static int ocdwarf_dodebug_info(handle_t p, handle_t s, handle_t d) { return 0; }
static int ocdwarf_dodebug_line(handle_t p, handle_t s, handle_t d) { return 0; }
static int ocdwarf_dodebug_macroinfo(handle_t p, handle_t s, handle_t d) { return 0; }
static int ocdwarf_dodebug_str(handle_t p, handle_t s, handle_t d) { return 0; }
static int ocdwarf_dodebug_stroffset(handle_t p, handle_t s, handle_t d) { return 0; }
static int ocdwarf_dodebug_types(handle_t p, handle_t s, handle_t d) { return 0; }
static int ocdwarf_doeh_frame(handle_t p, handle_t s, handle_t d) { return 0; }

pdwarf_display_t ocdwarf_get(handle_t s) {
  if (isopsection(s)) {
    const char* name = ocget_name(s);
    pdwarf_display_t p = zDWARFDISPLAY;
    for (size_t i = 0; i < eDWARF_MAX; ++i, ++p) {
      pdwarf_section_t x = &p->section;
      if (x->uncompressed_name && 0 == strcmp(x->uncompressed_name, name)) return p;
      else if (x->compressed_name && 0 == strcmp(x->compressed_name, name)) return p;
      else if (x->xcoff_name && 0 == strcmp(x->xcoff_name, name)) return p;
    }
  }

  return NULL;
}

int ocdwarf_isneeded(handle_t s, handle_t o) {
  if (isopsection(s) && isoptions(o)) {
    if (0 == (ocget_flags(s) & SEC_HAS_CONTENTS)) return 0;
    else if (0 == ocget_size(s)) return 0;

    poptions_t op = CAST(poptions_t, o);
    pdwarf_display_t p = ocdwarf_get(s);
    return p && p->action & op->action;
  }

  return 0;
}

int ocdwarf_open(handle_t p, handle_t o) {
  return 0;
}

int ocdwarf_close(handle_t p) {
  return 0;
}

int ocdwarf_run(handle_t p, handle_t s) {
  if (isopcode(p) && isopsection(s)) {
    pdwarf_display_t d = ocdwarf_get(s);
    return d && d->func ? d->func(p, s, &d->section) : -1;
  }

  return -1;
}

