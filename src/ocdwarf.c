#include <bfd.h>
#include <dwarf.h>
#include <libdwarf.h>

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
  if (isopshdr(s)) {
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

bool_t ocdwarf_isneeded(handle_t p, handle_t s, handle_t o) {
  if (isopshdr(s) && isoptions(o)) {
    if (0 == (ocget_flags(s) & SEC_HAS_CONTENTS)) return FALSE;
    else if (0 == ocget_size(s)) return FALSE;

    uint64_t soffset = ocget_soffset(p, s);
    uint64_t eoffset = ocget_eoffset(p, s);
    if (soffset >= eoffset) return FALSE;

    poptions_t op = CAST(poptions_t, o);
    pdwarf_display_t p = ocdwarf_get(s);
    return p && MODE_ISSET(p->action, op->ocdump);
  }

  return FALSE;
}

int ocdwarf_open(handle_t p, handle_t o) {
  return 0;
}

int ocdwarf_close(handle_t p) {
  return 0;
}

int ocdwarf_run(handle_t p, handle_t s) {
  int n = 0;
  if (isopcode(p) && isopshdr(s)) {

    Dwarf_Debug dbg = 0;
    Dwarf_Error error = 0;
    //int res = dwarf_object_init_b(&dw_interface, 0, 0, DW_GROUPNUMBER_ANY, &dbg, &error);

const char *path = "samples/exampled-64";
char *true_path = 0;
unsigned int true_pathlen = 0;
Dwarf_Handler errhand = 0;
Dwarf_Ptr errarg = 0;
int r = dwarf_init_path(path,true_path,true_pathlen, DW_GROUPNUMBER_ANY,errhand,errarg,&dbg,&error);

    pdwarf_display_t d = ocdwarf_get(s);
    n = d && d->func ? d->func(p, s, &d->section) : -1;

    dwarf_object_finish(dbg);
  }

  return n;
}

