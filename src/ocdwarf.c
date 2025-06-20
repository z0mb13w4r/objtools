#include <bfd.h>

#include "ocdwarf.h"
#include "options.h"
#include "ocdwarf-str.h"
#include "ocdwarf-info.h"
#include "ocdwarf-lines.h"
#include "ocdwarf-macro.h"
#include "ocdwarf-types.h"
#include "ocdwarf-abbrev.h"
#include "ocdwarf-printf.h"
#include "ocdwarf-aranges.h"
#include "ocdwarf-eh-frame.h"
#include "ocdwarf-stroffset.h"

#include <fcntl.h>

static const int MAXSIZE = 23;

static const Dwarf_Sig8 zerosignature;

#include "static/dwarf.ci"

pdwarf_display_t ocdwarf_get(handle_t s) {
  if (isopshdr(s) || isopshdrNN(s)) {
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

bool_t ocdwarf_isneeded(handle_t p, handle_t s) {
  if (0 == ocget_size(s)) return FALSE;

  uint64_t soffset = ocget_soffset(p, s);
  uint64_t eoffset = ocget_eoffset(p, s);
  if (soffset >= eoffset) return FALSE;

  if (isopcode(p)) {
    popcode_t p0 = CAST(popcode_t, p);
    pdwarf_display_t p1 = ocdwarf_get(s);
    return p1 && MODE_ISSET(p1->action, p0->ocdump);
  } else if (isoptions(p)) {
    poptions_t p0 = CAST(poptions_t, p);
    pdwarf_display_t p1 = ocdwarf_get(s);
    return p1 && MODE_ISSET(p1->action, p0->ocdump);
  }

  return FALSE;
}

int ocdwarf_open(handle_t p, handle_t o) {
  if (isopcode(p) && isoptions(o)) {
    popcode_t oc = CAST(popcode_t, p);
    poptions_t op = CAST(poptions_t, o);

    oc->action = op->action;
    oc->ocdump = op->ocdump;
    oc->saddress = op->saddress;
    oc->eaddress = op->eaddress;

    MEMCPYA(oc->inpname0, op->inpname0);
    MEMCPYA(oc->inpname1, op->inpname1);

    oc->items[OPCODE_DWARF1] = xmalloc(sizeof(dwarf_workspace_t));
    if (oc->items[OPCODE_DWARF1]) {
      pdwarf_workspace_t ws = CAST(pdwarf_workspace_t, oc->items[OPCODE_DWARF1]);
      ws->sf = xmalloc(sizeof(pdwarf_srcfiles_t));
      if (ws) {
        ws->sf->status = DW_DLV_ERROR;
      }
    }

    return 0;
  }

  return -1;
}

int ocdwarf_close(handle_t p) {
  if (isopcode(p)) {
    ocdwarf_sfreset(p);
    popcode_t oc = CAST(popcode_t, p);
    if (oc) {
      pdwarf_workspace_t ws = CAST(pdwarf_workspace_t, oc->items[OPCODE_DWARF1]);
      if (ws) {
        xfree(ws->sf);
      }

      zfree(&oc->items[OPCODE_DWARF1]);
    }

    return 0;
  }

  return -1;
}

/* Some valid DWARF2 data */
static Dwarf_Small abbrevbytes[] = {
0x01, 0x11, 0x01, 0x25, 0x0e, 0x13, 0x0b, 0x03, 0x08, 0x1b,
0x0e, 0x11, 0x01, 0x12, 0x01, 0x10, 0x06, 0x00, 0x00, 0x02,
0x2e, 0x01, 0x3f, 0x0c, 0x03, 0x08, 0x3a, 0x0b, 0x3b, 0x0b,
0x39, 0x0b, 0x27, 0x0c, 0x11, 0x01, 0x12, 0x01, 0x40, 0x06,
0x97, 0x42, 0x0c, 0x01, 0x13, 0x00, 0x00, 0x03, 0x34, 0x00,
0x03, 0x08, 0x3a, 0x0b, 0x3b, 0x0b, 0x39, 0x0b, 0x49, 0x13,
0x02, 0x0a, 0x00, 0x00, 0x04, 0x24, 0x00, 0x0b, 0x0b, 0x3e,
0x0b, 0x03, 0x08, 0x00, 0x00, 0x00, };
static Dwarf_Small infobytes[] = {
0x60, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x74, 0x2e, 0x63,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x66, 0x00, 0x01,
0x02, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x01, 0x5c, 0x00, 0x00, 0x00, 0x03, 0x69,
0x00, 0x01, 0x03, 0x08, 0x5c, 0x00, 0x00, 0x00, 0x02, 0x91,
0x6c, 0x00, 0x04, 0x04, 0x05, 0x69, 0x6e, 0x74, 0x00, 0x00, };
static Dwarf_Small strbytes[] = {
0x47, 0x4e, 0x55, 0x20, 0x43, 0x31, 0x37, 0x20, 0x39, 0x2e,
0x33, 0x2e, 0x30, 0x20, 0x2d, 0x6d, 0x74, 0x75, 0x6e, 0x65,
0x3d, 0x67, 0x65, 0x6e, 0x65, 0x72, 0x69, 0x63, 0x20, 0x2d,
0x6d, 0x61, 0x72, 0x63, 0x68, 0x3d, 0x78, 0x38, 0x36, 0x2d,
0x36, 0x34, 0x20, 0x2d, 0x67, 0x64, 0x77, 0x61, 0x72, 0x66,
0x2d, 0x32, 0x20, 0x2d, 0x4f, 0x30, 0x20, 0x2d, 0x66, 0x61,
0x73, 0x79, 0x6e, 0x63, 0x68, 0x72, 0x6f, 0x6e, 0x6f, 0x75,
0x73, 0x2d, 0x75, 0x6e, 0x77, 0x69, 0x6e, 0x64, 0x2d, 0x74,
0x61, 0x62, 0x6c, 0x65, 0x73, 0x20, 0x2d, 0x66, 0x73, 0x74,
0x61, 0x63, 0x6b, 0x2d, 0x70, 0x72, 0x6f, 0x74, 0x65, 0x63,
0x74, 0x6f, 0x72, 0x2d, 0x73, 0x74, 0x72, 0x6f, 0x6e, 0x67,
0x20, 0x2d, 0x66, 0x73, 0x74, 0x61, 0x63, 0x6b, 0x2d, 0x63,
0x6c, 0x61, 0x73, 0x68, 0x2d, 0x70, 0x72, 0x6f, 0x74, 0x65,
0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x2d, 0x66, 0x63, 0x66,
0x2d, 0x70, 0x72, 0x6f, 0x74, 0x65, 0x63, 0x74, 0x69, 0x6f,
0x6e, 0x00, 0x2f, 0x76, 0x61, 0x72, 0x2f, 0x74, 0x6d, 0x70,
0x2f, 0x74, 0x69, 0x6e, 0x79, 0x64, 0x77, 0x61, 0x72, 0x66,
0x00, };

/*  An internals_t , data used elsewhere but
    not directly visible elsewhere. One needs to have one
    of these but  maybe the content here too little
    or useless, this is just an example of sorts.  */
typedef struct sectiondata_s {
  unsigned int   sd_addr;
  unsigned int   sd_objoffsetlen;
  unsigned int   sd_objpointersize;
  Dwarf_Unsigned sd_sectionsize;
  const char    *sd_secname;
  Dwarf_Small   *sd_content;
} sectiondata_t, *psectiondata_t;

/*  The secname must not be 0 , pass "" if
    there is no name. */
static sectiondata_t sectiondata[] = {
{0,0,0,0,"",0},
//{0,32,32,sizeof(abbrevbytes),".debug_abbrev",abbrevbytes},
{0,32,32,sizeof(infobytes),".debug_info",infobytes},
//{0,32,32,sizeof(strbytes),".debug_str",strbytes}
};

typedef struct dwarf_data_s {
  int            f_is_64bit;
  Dwarf_Small    f_object_endian;
  unsigned       f_pointersize;
  unsigned       f_offsetsize;
  Dwarf_Unsigned f_filesize;
  Dwarf_Unsigned f_sectioncount;
  struct sectiondata_s * f_sectarray;
} dwarf_data_t, *pdwarf_data_t;

/*  Use DW_END_little.
    Libdwarf finally sets the file-format-specific
    f_object_endianness field to a DW_END_little or
    DW_END_big (see dwarf.h).
    Here we must do that ourselves. */
static dwarf_data_t base_internals = {
  FALSE, DW_END_little, 32, 32, 200, NELEMENTS(sectiondata), sectiondata
};

static int ocdwarf_secinfo(unknown_t obj, Dwarf_Unsigned idx, Dwarf_Obj_Access_Section_a* sec, int* e) {
  pdwarf_data_t pobj = CAST(pdwarf_data_t, obj);

  if (pobj && sec && e) {
    *e = 0; /* No error. Avoids unused arg */
    if (idx >= pobj->f_sectioncount) {
      return DW_DLV_NO_ENTRY;
    }

    if (pobj->f_sectarray) {
      struct sectiondata_s *fsect = pobj->f_sectarray + idx;

      sec->as_name   = fsect->sd_secname;
      sec->as_type   = 0;
      sec->as_flags  = 0;
      sec->as_addr   = fsect->sd_addr;
      sec->as_offset = 0;
      sec->as_size   = fsect->sd_sectionsize;
      sec->as_link   = 0;
      sec->as_info   = 0;
      sec->as_addralign = 0;
      sec->as_entrysize = 1;
      return DW_DLV_OK;
    }
  }

  return DW_DLV_ERROR;
}

static Dwarf_Small ocdwarf_byteorder(unknown_t obj) {
  pdwarf_data_t pobj = CAST(pdwarf_data_t, obj);
  return pobj ? pobj->f_object_endian : DW_END_little;
}

static Dwarf_Small ocdwarf_offsize(unknown_t obj) {
  pdwarf_data_t pobj = CAST(pdwarf_data_t, obj);
  return pobj && pobj->f_offsetsize ? pobj->f_offsetsize / 8 : 0;
}

static Dwarf_Small ocdwarf_ptrsize(unknown_t obj) {
  pdwarf_data_t pobj = CAST(pdwarf_data_t, obj);
  return pobj && pobj->f_pointersize ? pobj->f_pointersize / 8 : 0;
}

static Dwarf_Unsigned ocdwarf_filesize(unknown_t obj) {
  pdwarf_data_t pobj = CAST(pdwarf_data_t, obj);
  return pobj ? pobj->f_filesize : 0;
}

static Dwarf_Unsigned ocdwarf_seccount(unknown_t obj) {
  pdwarf_data_t pobj = CAST(pdwarf_data_t, obj);
  return pobj ? pobj->f_sectioncount : 0;
}

static int ocdwarf_loadsec(unknown_t obj, Dwarf_Unsigned idx, Dwarf_Small **rdata, int *e) {
  pdwarf_data_t pobj = CAST(pdwarf_data_t, obj);
  if (pobj && e && rdata) {
    *e = 0; /* No Error, avoids compiler warning */
    if (idx >= pobj->f_sectioncount) {
      return DW_DLV_NO_ENTRY;
    }

    struct sectiondata_s *secp = pobj->f_sectarray + idx;
    *rdata = secp->sd_content;
    return DW_DLV_OK;
  }

  return DW_DLV_ERROR;
}

const Dwarf_Obj_Access_Methods_a methods = {
  ocdwarf_secinfo,
  ocdwarf_byteorder,
  ocdwarf_offsize,
  ocdwarf_ptrsize,
  ocdwarf_filesize,
  ocdwarf_seccount,
  ocdwarf_loadsec,
  0 /* no relocating anything */
};

struct Dwarf_Obj_Access_Interface_a_s dw_interface = {
  &base_internals, &methods
};

Dwarf_Half cu_version_stamp = 0;
Dwarf_Half cu_offset_size   = 0;

void ocdwarf_sfreset(handle_t p) {
  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);
    pdwarf_workspace_t ws = CAST(pdwarf_workspace_t, oc->items[OPCODE_DWARF1]);
    if (ws && ws->sf) {
      if (ws->sf->data) {
        for (Dwarf_Signed i = 0; i < ws->sf->size; ++i) {
          ocdwarf_dealloc(p, ws->sf->data[i], DW_DLA_STRING);
        }
        ocdwarf_dealloc(p, ws->sf->data, DW_DLA_LIST);
      }

      ws->sf->status = DW_DLV_ERROR;
      ws->sf->data = NULL;
      ws->sf->size = 0;
    }
  }
}

void ocdwarf_dealloc(handle_t p, unknown_t v, Dwarf_Unsigned type) {
  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);
    if (oc->items[OPCODE_DWARF1]) {
      pdwarf_workspace_t ws = CAST(pdwarf_workspace_t, oc->items[OPCODE_DWARF1]);
      dwarf_dealloc(ws->dbg, v, type);
    }
  }
}

void ocdwarf_dealloc_attribute(handle_t p, Dwarf_Attribute *v, Dwarf_Signed size) {
  if (isopcode(p)) {
    for (Dwarf_Signed i = 0; i < size; ++i) {
      dwarf_dealloc_attribute(v[i]);
    }

    ocdwarf_dealloc(p, v, DW_DLA_LIST);
  }
}

void ocdwarf_dealloc_error(handle_t p, Dwarf_Error *e) {
  if (isopcode(p) && e) {
    printf_e("message = %s", dwarf_errmsg(*e));

    popcode_t oc = CAST(popcode_t, p);
    if (oc->items[OPCODE_DWARF1]) {
      pdwarf_workspace_t ws = CAST(pdwarf_workspace_t, oc->items[OPCODE_DWARF1]);
      dwarf_dealloc_error(ws->dbg, *e);
    }
  }
}

void ocdwarf_finish(handle_t p, Dwarf_Error *e) {
  if (isopcode(p)) {
    ocdwarf_dealloc_error(p, e);

    popcode_t oc = CAST(popcode_t, p);
    if (oc->items[OPCODE_DWARF1]) {
      pdwarf_workspace_t ws = CAST(pdwarf_workspace_t, oc->items[OPCODE_DWARF1]);
      dwarf_finish(ws->dbg);
//      ws->dbg = NULL;
    }
  }
}

void ocdwarf_object_finish(handle_t p) {
  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);
    if (oc->items[OPCODE_DWARF1]) {
      pdwarf_workspace_t ws = CAST(pdwarf_workspace_t, oc->items[OPCODE_DWARF1]);
      dwarf_object_finish(ws->dbg);
      ws->dbg = NULL;
    }
  }
}

int ocdwarf_run(handle_t p, handle_t s) {
  int n = 0;
  if (isopcode(p) && isopshdr(s)) {
    popcode_t oc = CAST(popcode_t, p);
    pdwarf_workspace_t ws = CAST(pdwarf_workspace_t, oc->items[OPCODE_DWARF1]);
    if (ws) {
      int x = dwarf_object_init_b(&dw_interface, 0, 0, DW_GROUPNUMBER_ANY, &ws->dbg, &ws->err);
      if (IS_DLV_NO_ENTRY(x)) {
        printf_x("Cannot dwarf_object_init_b() NO ENTRY.");
      } else if (IS_DLV_ERROR(x)) {
        ocdwarf_dealloc_error(p, &ws->err);
        printf_x("Cannot dwarf_object_init_b().");
      }

      pdwarf_display_t d = ocdwarf_get(s);
      n = d && d->func ? d->func(p, s, &d->section) : -1;

      ocdwarf_object_finish(p);
    }
  } else if (isopcode(p) && isopshdrNN(s)) {
    popcode_t oc = CAST(popcode_t, p);
    pdwarf_workspace_t ws = CAST(pdwarf_workspace_t, oc->items[OPCODE_DWARF1]);
    if (ws) {
    pdwarf_display_t d = ocdwarf_get(s);
      if (d && d->func) {
        popcode_t oc = CAST(popcode_t, p);

//      sectiondata[1].sd_sectionsize = ocget_size(s);
//      sectiondata[1].sd_secname = ocget_name(s);
//      sectiondata[1].sd_content = ocget_data(s);

        int my_init_fd = open(oc->inpname0, O_RDONLY);
        if (-1 == my_init_fd) {
          printf_x("Giving up, cannot open '%s'", oc->inpname0);
        }

        int x = dwarf_init_b(my_init_fd, DW_GROUPNUMBER_ANY, 0, 0, &ws->dbg, &ws->err);
        if (IS_DLV_ANY_ERROR(x)) {
          printf_x("Giving up, cannot do DWARF processing '%s'", oc->inpname0);
        }

        n = d && d->func ? d->func(p, s, &d->section) : -1;
        ocdwarf_object_finish(p);
      }
    }
  }

  return n;
}

