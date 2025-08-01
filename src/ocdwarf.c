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

const Dwarf_Sig8  ZEROSIGNATURE;
const Dwarf_Block ZEROBLOCK;

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
    popcode_t p0 = ocget(p, OPCODE_THIS);
    pdwarf_display_t p1 = ocdwarf_get(s);
    return p1 && MODE_ISANY(p1->action, p0->ocdump);
  } else if (isoptions(p)) {
    poptions_t p0 = CAST(poptions_t, p);
    pdwarf_display_t p1 = ocdwarf_get(s);
    return p1 && MODE_ISANY(p1->action, p0->ocdump);
  }

  return FALSE;
}

int ocdwarf_open(handle_t p, handle_t o) {
  if (isopcode(p) && isoptions(o)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    poptions_t op = CAST(poptions_t, o);

    oc->action = op->action;
    oc->ocdump = op->ocdump;
    oc->saddress = op->saddress;
    oc->eaddress = op->eaddress;

    MEMCPYA(oc->inpname0, op->inpname0);
    MEMCPYA(oc->inpname1, op->inpname1);

    oc->items[OPCODE_DWARF] = xmalloc(sizeof(ocdwarf_t));
    if (oc->items[OPCODE_DWARF]) {
      pocdwarf_t ws = CAST(pocdwarf_t, oc->items[OPCODE_DWARF]);
      ws->sf = xmalloc(sizeof(dwarf_srcfiles_t));
      ws->st = xmalloc(sizeof(dwarf_statistics_t));
      if (ws) {
        ws->sf->status = DW_DLV_ERROR;
      }

      int my_init_fd = open(oc->inpname0, O_RDONLY);
      if (-1 == my_init_fd) {
        printf_x("Giving up, cannot open '%s'", oc->inpname0);
      }

      int x = dwarf_init_b(my_init_fd, DW_GROUPNUMBER_ANY, 0, 0, &ws->dbg, &ws->err);
      if (IS_DLV_ANY_ERROR(x)) {
        printf_x("Giving up, cannot do DWARF processing '%s'", oc->inpname0);
      }
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int ocdwarf_close(handle_t p) {
  if (isopcode(p)) {
    ocdwarf_sfreset(p);
    ocdwarf_object_finish(p);
    ocdwarf_finish(p, NULL);

    popcode_t oc = ocget(p, OPCODE_THIS);
    if (oc) {
      pocdwarf_t ws = CAST(pocdwarf_t, oc->items[OPCODE_DWARF]);
      if (ws) {
        xfree(ws->sf);
        xfree(ws->st);
      }

      zfree(&oc->items[OPCODE_DWARF]);
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
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

int ocdwarf_sfreset(handle_t p) {
  if (isopcode(p)) {
    pdwarf_srcfiles_t sf = ocget(p, OPCODE_DWARF_SRCFILES);
    if (sf) {
      if (sf->data) {
        for (Dwarf_Signed i = 0; i < sf->size; ++i) {
          ocdwarf_dealloc(p, sf->data[i], DW_DLA_STRING);
        }
        ocdwarf_dealloc(p, sf->data, DW_DLA_LIST);
      }

      sf->status = DW_DLV_ERROR;
      sf->data = NULL;
      sf->size = 0;

      return ECODE_OK;
    }
  }

  return ECODE_HANDLE;
}

int ocdwarf_sfcreate(handle_t p, Dwarf_Die die, Dwarf_Error *e) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    pdwarf_srcfiles_t sf = ocget(p, OPCODE_DWARF_SRCFILES);
    if (sf) {
      if (ECODE_ISOK(ocdwarf_sfreset(p))) {
      sf->status = dwarf_srcfiles(die, &sf->data, &sf->size, e);

        int n = ECODE_OK;
        if (MODE_ISANY(oc->action, OPTPROGRAM_VERBOSE) && IS_DLV_OK(sf->status) && 0 != sf->size) {
          for (Dwarf_Signed i = 0; i < sf->size; ++i) {
            n += printf_nice(i, USE_DEC3 | USE_TB | USE_COLON);
            n += printf_text(sf->data[i], USE_LT | USE_EOL);
          }
        }

        return n;
      }
    }
  }

  return ECODE_HANDLE;
}

static int ocdwarf_spget0(handle_t p, Dwarf_Die die, Dwarf_Half tag, Dwarf_Addr addr,
              Dwarf_Bool isinfo, char **name,
              Dwarf_Unsigned *nline, Dwarf_Unsigned *ncolumn, char **source,
              Dwarf_Addr *low_pc_addr, Dwarf_Addr *high_pc_addr, Dwarf_Off *offset, Dwarf_Error *e) {
  if (isopcode(p)) {
    Dwarf_Signed cattr = 0;
    Dwarf_Attribute *pattr = 0;
    int x = dwarf_attrlist(die, &pattr, &cattr, e);
    if (IS_DLV_NO_ENTRY(x)) {
      return ECODE_NOENTRY;
    } else if (IS_DLV_ERROR(x)) {
      printf_e("dwarf_attrlist failed! errcode %d", x);
      return ECODE_DWARF;
    }

    Dwarf_Bool ismatch = FALSE;
    Dwarf_Addr xlow_pc_addr = 0;

    for (Dwarf_Signed i = 0; i < cattr; ++i) {
      Dwarf_Half nattr = 0;
      x = dwarf_whatattr(pattr[i], &nattr, e);
      if (IS_DLV_ANY_ERROR(x)) {
        printf_e("dwarf_whatattr failed! errcode %d", x);
        return ECODE_DWARF;
      } else if (DW_AT_low_pc == nattr) {
        x = dwarf_formaddr(pattr[i], &xlow_pc_addr, e);
        if (IS_DLV_ANY_ERROR(x)) {
          printf_e("dwarf_formaddr failed! errcode %d", x);
          return ECODE_DWARF;
        } else if (xlow_pc_addr == addr) {
          ismatch = TRUE;
        }
      } else if (DW_AT_high_pc == nattr) {
        Dwarf_Unsigned value = 0;
        x = dwarf_formudata(pattr[i], &value, e);
        if (IS_DLV_ANY_ERROR(x)) {
          printf_e("dwarf_formudata failed! errcode %d", x);
          return ECODE_DWARF;
        } else if ((xlow_pc_addr <= addr) && (addr <= (xlow_pc_addr + value)) && (0 != offset)) {
          ismatch = TRUE;
        }
      }
    }

    if (ismatch) {
      for (Dwarf_Signed i = 0; i < cattr; ++i) {
        Dwarf_Attribute attr = pattr[i];

        Dwarf_Half nform = 0;
        x = dwarf_whatform(attr, &nform, e);
        if (IS_DLV_ANY_ERROR(x)) {
          if (IS_DLV_ERROR(x) && e) {
            ocdwarf_dealloc_error(p, e);
          }

          ocdwarf_object_finish(p);
          printf_e("dwarf_whatform failed! errcode %d", x);
          return ECODE_DWARF;
        }

        Dwarf_Half nattr = 0;
        x = dwarf_whatattr(attr, &nattr, e);
        if (IS_DLV_ANY_ERROR(x)) {
          printf_e("dwarf_whatattr failed! errcode %d", x);
          return ECODE_DWARF;
        } else if (isused(zFORMSTRING, nform)) {
          if (name) {
            char *value = NULL;
            x = dwarf_formstring(attr, &value, e);
            if (IS_DLV_ANY_ERROR(x)) {
              printf_e("dwarf_formstring failed! errcode %d", x);
             return ECODE_DWARF;
            } else if (DW_AT_name == nattr) {
              *name = value;
            }
          }
        } else if (isused(zFORMUDATA, nform)) {
          Dwarf_Unsigned value = 0;
          x = dwarf_formudata(attr, &value, e);
          if (IS_DLV_ANY_ERROR(x)) {
            printf_e("dwarf_formudata failed! errcode %d", x);
            return ECODE_DWARF;
          } else if (DW_AT_decl_line == nattr && nline) {
            *nline = value + 1;
          } else if (DW_AT_decl_column == nattr && ncolumn) {
            *ncolumn = value;
          } else if (DW_AT_high_pc == nattr) {
            if (low_pc_addr && high_pc_addr) {
              *high_pc_addr = *low_pc_addr + value;
            }
          } else if (DW_AT_decl_file == nattr && source) {
            pdwarf_srcfiles_t sf = ocget(p, OPCODE_DWARF_SRCFILES);
            if (sf && IS_DLV_OK(sf->status) && (0 != sf->size) && ((value - 1) < sf->size)) {
              *source = sf->data[value - 1];
            }
          }
        } else if (isused(zFORMADDR, nform)) {
          Dwarf_Addr xaddr = 0;
          x = dwarf_formaddr(attr, &xaddr, e);
          if (IS_DLV_ANY_ERROR(x)) {
            printf_e("dwarf_formaddr failed! errcode %d", x);
            return ECODE_DWARF;
          } else if (DW_AT_low_pc == nattr) {
            if (low_pc_addr) {
              *low_pc_addr = xaddr;
            }
            if (offset) {
              *offset = addr - xaddr;
            }
          }
        }
      }
    }

    ocdwarf_dealloc_attribute(p, pattr, cattr);
    return ismatch ? ECODE_OK : ECODE_NOENTRY;
  }

  return ECODE_HANDLE;
}

static int ocdwarf_spget1(handle_t p, Dwarf_Die die, Dwarf_Addr addr, Dwarf_Bool isinfo, char **name,
                     Dwarf_Unsigned *nline, Dwarf_Unsigned *ncolumn, char **source,
                     Dwarf_Addr *low_pc_addr, Dwarf_Addr *high_pc_addr, Dwarf_Off *offset, Dwarf_Error *e) {
  if (isopcode(p)) {
    Dwarf_Half tag = 0;
    int x = dwarf_tag(die, &tag, e);
    if (IS_DLV_ANY_ERROR(x)) {
      if (IS_DLV_ERROR(x) && e) {
        ocdwarf_dealloc_error(p, e);
      }
      ocdwarf_object_finish(p);
      printf_x("dwarf_tag failed! errcode %d", x);
      return ECODE_DWARF;
    }

    if (DW_TAG_subprogram == tag) {
      return ocdwarf_spget0(p, die, tag, addr, isinfo, name, nline, ncolumn,
                     source, low_pc_addr, high_pc_addr, offset, e);
    } else if (DW_TAG_compile_unit == tag || DW_TAG_partial_unit == tag || DW_TAG_type_unit == tag) {
      ocdwarf_sfcreate(p, die, e);
    }

    return ECODE_NOENTRY;
  }

  return ECODE_HANDLE;
}

static int ocdwarf_spget2(handle_t p, Dwarf_Die die, Dwarf_Addr addr, Dwarf_Bool isinfo, char **name,
                  Dwarf_Unsigned *nline, Dwarf_Unsigned *ncolumn, char **source,
                  Dwarf_Addr *low_pc_addr, Dwarf_Addr *high_pc_addr, Dwarf_Off *offset, Dwarf_Error *e) {
  if (isopcode(p)) {
    Dwarf_Die cur_die = die;

    for ( ; ; ) {
      Dwarf_Die child = 0;
      int x = dwarf_child(cur_die, &child, e);
      if (IS_DLV_ERROR(x)) {
        dwarf_dealloc_die(cur_die);
        ocdwarf_finish(p, e);
        printf_x("dwarf_child failed! errcode %d", x);
        return ECODE_DWARF;
      } else if (IS_DLV_OK(x)) {
        x = ocdwarf_spget2(p, child, addr, isinfo, name, nline, ncolumn,
                     source, low_pc_addr, high_pc_addr, offset, e);
        dwarf_dealloc_die(child);
        if (ECODE_ISOK(x)) {
          if (cur_die != die) {
            dwarf_dealloc_die(cur_die);
          }
          return ECODE_OK;
        }
      }
      /* x == DW_DLV_NO_ENTRY or DW_DLV_OK */
      Dwarf_Die sib_die = 0;
      x = dwarf_siblingof_b(ocget(p, OPCODE_DWARF_DEBUG), cur_die, isinfo, &sib_die, e);
      if (IS_DLV_NO_ENTRY(x)) return ECODE_NOENTRY;
      else if (IS_DLV_ERROR(x)) {
        ocdwarf_finish(p, e);
        printf_x("dwarf_siblingof_b failed! errcode %d", x);
        return ECODE_DWARF;
      }

      if (cur_die != die) {
        dwarf_dealloc_die(cur_die);
      }

      if (ECODE_ISOK(ocdwarf_spget1(p, sib_die, addr, isinfo, name, nline, ncolumn,
                     source, low_pc_addr, high_pc_addr, offset, e))) {
        dwarf_dealloc_die(sib_die);
        return ECODE_OK;
      }

      cur_die = sib_die;
    }
  }

  return ECODE_HANDLE;
}

static int ocdwarf_spget3(handle_t p, Dwarf_Addr addr, Dwarf_Bool isinfo, char **name,
                  Dwarf_Unsigned *nline, Dwarf_Unsigned *ncolumn, char **source,
                  Dwarf_Addr *low_pc_addr, Dwarf_Addr *high_pc_addr, Dwarf_Off *offset, Dwarf_Error *e) {
  if (isopcode(p)) {
      Dwarf_Die no_die = 0;
      Dwarf_Die cu_die = 0;

      int x = dwarf_siblingof_b(ocget(p, OPCODE_DWARF_DEBUG), no_die, isinfo, &cu_die, e);
      if (IS_DLV_NO_ENTRY(x)) return ECODE_NOENTRY;
      else if (IS_DLV_ERROR(x)) {
        ocdwarf_dealloc_error(p, NULL);
        printf_e("dwarf_siblingof_b failed! errcode %d", x);
        return ECODE_DWARF;
      }

      x = ocdwarf_spget2(p, cu_die, addr, isinfo, name, nline, ncolumn,
                     source, low_pc_addr, high_pc_addr, offset, e);
      dwarf_dealloc_die(cu_die);
      return x;
  }

  return ECODE_HANDLE;
}

int ocdwarf_dfget3(handle_t p, Dwarf_Die die, Dwarf_Addr addr, Dwarf_Bool isinfo, char **source,
                     Dwarf_Unsigned *nline, Dwarf_Unsigned *ncolumn, Dwarf_Unsigned *discriminator,
                     Dwarf_Error *e) {
  if (isopcode(p)) {
    Dwarf_Signed line_count = 0;
    Dwarf_Line  *line_array = NULL;
    Dwarf_Small  table_count = 0;
    Dwarf_Unsigned line_version = 0;
    Dwarf_Line_Context line_context = 0;

    int x = dwarf_srclines_b(die, &line_version, &table_count, &line_context, e);
    if (IS_DLV_OK(x)) {
      x = dwarf_srclines_from_linecontext(line_context, &line_array, &line_count, e);
    }
    if (IS_DLV_OK(x)) {
      for (Dwarf_Signed i = 0; i < line_count; ++i) {
        Dwarf_Addr pc = 0;
        Dwarf_Line k = line_array[i];
        x = dwarf_lineaddr(k, &pc, e);
        if (IS_DLV_ANY_ERROR(x)) continue;
        else if (addr != pc) continue;

        Dwarf_Unsigned value0 = 0;
        Dwarf_Unsigned value1 = 0;
        Dwarf_Bool value2 = FALSE;
        Dwarf_Bool value3 = FALSE;
        char *value4 = NULL;

        if (nline) {
          x = dwarf_lineno(k, &value0, e);
          if (IS_DLV_OK(x)) {
            *nline = value0;
          }
        }

        if (ncolumn) {
          x = dwarf_lineoff_b(k, &value0, e);
          if (IS_DLV_OK(x)) {
            *ncolumn = value0;
          }
        }

        if (discriminator) {
          x = dwarf_prologue_end_etc(k, &value2, &value3, &value0, &value1, e);
          if (IS_DLV_OK(x)) {
            *discriminator = value1;
          }
        }

        if (source) {
          x = dwarf_linesrc(k, &value4, e);
          if (IS_DLV_OK(x)) {
            *source = value4;
          }
        }

	dwarf_srclines_dealloc_b(line_context);
	return ECODE_OK;
      }

      dwarf_srclines_dealloc_b(line_context);
    }

    return ECODE_NOENTRY;
  }

  return ECODE_HANDLE;
}

int ocdwarf_spget(handle_t p, Dwarf_Addr addr, char** name,
                     Dwarf_Unsigned *nline, Dwarf_Unsigned *ncolumn, Dwarf_Unsigned *discriminator, char **source,
                     Dwarf_Addr *low_pc_addr, Dwarf_Addr *high_pc_addr, Dwarf_Off *offset, Dwarf_Error *e) {
  if (isopcode(p)) {
    Dwarf_Bool isinfo = TRUE; /* our data is not DWARF4 .debug_types. */

    for ( ; ; ) {
      Dwarf_Die cu_die = 0;
//if (addr == 0x12cd || addr == 0x16b0) printf("+++A+\n");
      int x = ocdwarf_next_cu_header(p, &cu_die, e);
      if (ECODE_ISEVIL(x)) {
        if (ECODE_ISFAILED(x)) {
          ocdwarf_dealloc_error(p, e);
          printf_e("ocdwarf_next_cu_header failed! %d", x);
        }
//if (addr == 0x12cd || addr == 0x16b0) printf("%d\n", x);
        return x;
      }

      x = ocdwarf_sfcreate(p, cu_die, e);
      if (ECODE_ISFAILED(x)) {
        dwarf_dealloc_die(cu_die);
        printf_e("ocdwarf_sfcreate failed! %d", x);
        return x;
      }

      x = ocdwarf_spget3(p, addr, isinfo, name, nline, ncolumn,
                     source, low_pc_addr, high_pc_addr, offset, e);
      if (ECODE_ISFAILED(x)) {
        dwarf_dealloc_die(cu_die);
        printf_e("ocdwarf_spget3 failed! %d", x);
        return x;
      } else if (ECODE_ISWARNING(x) && (0 != offset)) {
        x = ocdwarf_dfget3(p, cu_die, addr, isinfo, source, nline, ncolumn,
                     discriminator, e);
        if (ECODE_ISFAILED(x)) {
          printf_e("ocdwarf_dfget3 failed! %d", x);
        }
      }

      dwarf_dealloc_die(cu_die);
      ocdwarf_sfreset(p);

      if (ECODE_ISOK(x)) {
        return ECODE_OK;
      }
    }
  }

  return ECODE_HANDLE;
}

int ocdwarf_next_cu_header(handle_t p, Dwarf_Die *cu_die, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && cu_die) {
    pocdwarf_t ws = ocget(p, OPCODE_DWARF);
    popcode_t oc = ocget(p, OPCODE_THIS);

    Dwarf_Bool isinfo = TRUE; /* our data is not DWARF4 .debug_types. */
    Dwarf_Unsigned next_cu_header_offset = 0;
    Dwarf_Unsigned cu_header_length = 0;
    Dwarf_Unsigned type_offset    = 0;
    Dwarf_Half     extension_size = 0;
    Dwarf_Half     header_cu_type = 0;
    Dwarf_Half     address_size  = 0;
    Dwarf_Sig8     type_signature = ZEROSIGNATURE;
    Dwarf_Off      abbrev_offset = 0;

    x = dwarf_next_cu_header_e(ocget(p, OPCODE_DWARF_DEBUG), isinfo, cu_die, &cu_header_length, &ws->cu_version_stamp,
                     &abbrev_offset, &address_size, &ws->cu_offset_size, &extension_size, &type_signature, &type_offset,
                     &next_cu_header_offset, &header_cu_type, e);

    if (IS_DLV_OK(x) && MODE_ISANY(oc->action, OPTPROGRAM_VERBOSE)) {
      n += printf_text("CU header length", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(cu_header_length, USE_FHEX | USE_EOL);
      n += printf_text("Version stamp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(ws->cu_version_stamp, USE_DEC | USE_EOL);
      n += printf_text("Abbrev offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(abbrev_offset, USE_DEC | USE_EOL);
      n += printf_text("Address size", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(address_size, USE_DEC | USE_EOL);
      n += printf_text("Extension size", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(extension_size, USE_DEC | USE_EOL);
      n += printf_text("Type offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(type_offset, USE_FHEX | USE_EOL);
      n += printf_text("CU offset size", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(ws->cu_offset_size, USE_DEC | USE_EOL);
      n += printf_text("CU next header offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(next_cu_header_offset, USE_FHEX | USE_EOL);
      n += printf_text("CU header type", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(header_cu_type, USE_FHEX | USE_EOL);
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

void ocdwarf_dealloc(handle_t p, unknown_t v, Dwarf_Unsigned type) {
  if (isopcode(p)) {
    pocdwarf_t p0 = ocget(p, OPCODE_DWARF);
    if (p0) {
      dwarf_dealloc(p0->dbg, v, type);
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

void ocdwarf_dealloc_fde_cie_list(handle_t p, Dwarf_Cie *cie_data, Dwarf_Signed cie_element_count,
                     Dwarf_Fde *fde_data, Dwarf_Signed fde_element_count) {
  if (isopcode(p)) {
    pocdwarf_t p0 = ocget(p, OPCODE_DWARF);
    if (p0) {
      dwarf_dealloc_fde_cie_list(ocget(p, OPCODE_DWARF_DEBUG), cie_data, cie_element_count,
                     fde_data, fde_element_count);
    }
  }
}

void ocdwarf_dealloc_error(handle_t p, Dwarf_Error *e) {
  if (isopcode(p)) {
    Dwarf_Error *e0 = e ? e : ocget(p, OPCODE_DWARF_ERROR);
    if (e0 && *e0) {
      printf_e("message = %s", dwarf_errmsg(*e0));

      pocdwarf_t p0 = ocget(p, OPCODE_DWARF);
      if (p0) {
        dwarf_dealloc_error(p0->dbg, *e0);
      }
    }
  }
}

void ocdwarf_finish(handle_t p, Dwarf_Error *e) {
  if (isopcode(p)) {
    ocdwarf_dealloc_error(p, e);

    pocdwarf_t p0 = ocget(p, OPCODE_DWARF);
    if (p0) {
      dwarf_finish(p0->dbg);
//      p0->dbg = NULL;
    }
  }
}

void ocdwarf_object_finish(handle_t p) {
  if (isopcode(p)) {
    pocdwarf_t p0 = ocget(p, OPCODE_DWARF);
    if (p0) {
      dwarf_object_finish(p0->dbg);
//      p0->dbg = NULL;
    }
  }
}

int ocdwarf_run(handle_t p, handle_t s) {
  int n = 0;
  if (isopcode(p) && isopshdr(s)) {
    pocdwarf_t ws = ocget(p, OPCODE_DWARF);
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
    pocdwarf_t ws = ocget(p, OPCODE_DWARF);
    if (ws) {
    pdwarf_display_t d = ocdwarf_get(s);
      if (d && d->func) {
//      sectiondata[1].sd_sectionsize = ocget_size(s);
//      sectiondata[1].sd_secname = ocget_name(s);
//      sectiondata[1].sd_content = ocget_data(s);

        n = d && d->func ? d->func(p, s, &d->section) : -1;
      }
    }
  }

  return n;
}

