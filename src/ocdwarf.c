#include <bfd.h>
#include <dwarf.h>
#include <libdwarf.h>

#include "opcode.h"
#include "ocdwarf.h"
#include "options.h"

static const Dwarf_Sig8 zerosignature;

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
  if (isopcode(p)) {
    if (0 == (ocget_flags(s) & SEC_HAS_CONTENTS)) return FALSE;
    else if (0 == ocget_size(s)) return FALSE;

    uint64_t soffset = ocget_soffset(p, s);
    uint64_t eoffset = ocget_eoffset(p, s);
    if (soffset >= eoffset) return FALSE;

    popcode_t p0 = CAST(popcode_t, p);
    pdwarf_display_t p1 = ocdwarf_get(s);
    return p1 && MODE_ISSET(p1->action, p0->ocdump);
  } else if (isoptions(p)) {
    if (0 == ocget_size(s)) return FALSE;

    uint64_t soffset = ocget_soffset(p, s);
    uint64_t eoffset = ocget_eoffset(p, s);
    if (soffset >= eoffset) return FALSE;

    poptions_t p0 = CAST(poptions_t, p);
    pdwarf_display_t p1 = ocdwarf_get(s);
    return p1 && MODE_ISSET(p1->action, p0->ocdump);
  }

  return FALSE;
}

int ocdwarf_open(handle_t p, handle_t o) {
  return 0;
}

int ocdwarf_close(handle_t p) {
  return 0;
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
{0,32,32,sizeof(abbrevbytes),".debug_abbrev",abbrevbytes},
{0,32,32,sizeof(infobytes),".debug_info",infobytes},
{0,32,32,sizeof(strbytes),".debug_str",strbytes}
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

struct Dwarf_Obj_Access_Interface_a_s dw_interface =
{ &base_internals,&methods };

static int
isformstring(Dwarf_Half form)
{
    /*  Not handling every form string, just the
        ones used in simple cases. */
    switch(form) {
    case DW_FORM_string:        return TRUE;
    case DW_FORM_GNU_strp_alt:  return TRUE;
    case DW_FORM_GNU_str_index: return TRUE;
    case DW_FORM_strx:          return TRUE;
    case DW_FORM_strx1:         return TRUE;
    case DW_FORM_strx2:         return TRUE;
    case DW_FORM_strx3:         return TRUE;
    case DW_FORM_strx4:         return TRUE;
    case DW_FORM_strp:          return TRUE;
    default: break;
    };
    return FALSE;
}

static int ocdwarf_print_attr(Dwarf_Attribute attr, Dwarf_Signed anumber, Dwarf_Error *e) {
  char *str = 0;
  const char *attrname = 0;
  const char *formname = 0;
  Dwarf_Half form = 0;
  Dwarf_Half attrnum = 0;
  int res = dwarf_whatform(attr, &form, e);
  if (res != DW_DLV_OK) {
        printf("dwarf_whatform failed! res %d\n",res);
        return res;
  }
  res = dwarf_whatattr(attr, &attrnum, e);
  if (res != DW_DLV_OK) {
        printf("dwarf_whatattr failed! res %d\n",res);
        return res;
  }
  res = dwarf_get_AT_name(attrnum, &attrname);
  if (res == DW_DLV_NO_ENTRY) {
        printf("Bogus attrnum 0x%x\n",attrnum);
        attrname = "<internal error ?>";
  }
  res = dwarf_get_FORM_name(form, &formname);
  if (res == DW_DLV_NO_ENTRY) {
        printf("Bogus form 0x%x\n",attrnum);
        attrname = "<internal error ?>";
  }
  if (!isformstring(form)) {
        printf("  [%2d] Attr: %-15s  Form: %-15s\n",
            (int)anumber,attrname, formname);
        return DW_DLV_OK;
  }
  res = dwarf_formstring(attr, &str, e);
  if (res != DW_DLV_OK) {
        printf("dwarf_formstring failed! res %d\n",res);
        return res;
  }
    printf("  [%2d] Attr: %-15s  Form: %-15s %s\n",
        (int)anumber,attrname,formname,str);
  return DW_DLV_OK;
}

static void ocdwarf_dealloc(handle_t p, handle_t s, Dwarf_Attribute *attrbuf, Dwarf_Signed attrcount, Dwarf_Signed i) {
  if (isopcode(p) && isopshdr(s)) {
    popcode_t oc = CAST(popcode_t, p);

    for ( ; i < attrcount; ++i) {
      dwarf_dealloc_attribute(attrbuf[i]);
    }

    dwarf_dealloc(oc->items[OPCODE_DWARF], attrbuf, DW_DLA_LIST);
  }
}

static int ocdwarf_one_die(handle_t p, handle_t s, Dwarf_Die in_die, int level, Dwarf_Error *e) {
  if (isopcode(p) && isopshdr(s)) {
    popcode_t oc = CAST(popcode_t, p);

    Dwarf_Attribute *attrbuf = 0;
    Dwarf_Signed  attrcount = 0;
    Dwarf_Half tag = 0;
    const char * tagname = 0;

    int res = dwarf_tag(in_die, &tag, e);
    if (res != DW_DLV_OK) {
        printf("dwarf_tag failed! res %d\n",res);
        return res;
    }
    res = dwarf_get_TAG_name(tag, &tagname);
    if (res != DW_DLV_OK) {
        tagname = "<bogus tag>";
    }
    printf("%3d:  Die: %s\n",level,tagname);
    res = dwarf_attrlist(in_die, &attrbuf, &attrcount ,e);
    if (res != DW_DLV_OK) {
        printf("dwarf_attrlist failed! res %d\n",res);
        return res;
    }
    for (Dwarf_Signed i = 0; i < attrcount; ++i) {
        res = ocdwarf_print_attr(attrbuf[i], i, e);
        if (res != DW_DLV_OK) {
            ocdwarf_dealloc(p, s, attrbuf, attrcount, 0);
            printf("dwarf_attr print failed! res %d\n",res);
            return res;
        }
    }

    ocdwarf_dealloc(p, s, attrbuf, attrcount, 0);
    return DW_DLV_OK;
  }

  return DW_DLV_ERROR;
}

static int ocdwarf_do(handle_t p, handle_t s, Dwarf_Error *e) {
  if (isopcode(p) && isopshdr(s)) {
    popcode_t oc = CAST(popcode_t, p);

    Dwarf_Bool is_info = TRUE; /* our data is not DWARF4 .debug_types. */
    Dwarf_Unsigned next_cu_header_offset = 0;
    Dwarf_Unsigned cu_header_length = 0;
    Dwarf_Unsigned typeoffset     = 0;
    Dwarf_Half     extension_size = 0;
    Dwarf_Half     header_cu_type = 0;
    Dwarf_Half     version_stamp = 0;
    Dwarf_Half     address_size  = 0;
    Dwarf_Half     length_size   = 0;
    Dwarf_Sig8     type_signature = zerosignature;
    Dwarf_Off      abbrev_offset = 0;
    Dwarf_Die      cu_die = 0;
    int            level = 0;

    int res = dwarf_next_cu_header_d(oc->items[OPCODE_DWARF], is_info,
                 &cu_header_length, &version_stamp, &abbrev_offset, &address_size, &length_size, &extension_size,
                 &type_signature, &typeoffset, &next_cu_header_offset, &header_cu_type, e);
    if (res != DW_DLV_OK) {
        printf("Next cu header  result %d. "
            "Something is wrong FAIL, line %d\n",res,__LINE__);
        if (res == DW_DLV_ERROR) {
            printf("Error is: %s\n",dwarf_errmsg(*e));
        }
        exit(EXIT_FAILURE);
    }
    printf("CU header length..........0x%lx\n", (unsigned long)cu_header_length);
    printf("Version stamp.............%d\n",version_stamp);
    printf("Address size .............%d\n",address_size);
    printf("Offset size...............%d\n",length_size);
    printf("Next cu header offset.....0x%lx\n", (unsigned long)next_cu_header_offset);

    res = dwarf_siblingof_b(oc->items[OPCODE_DWARF], NULL, is_info, &cu_die, e);
    if (res != DW_DLV_OK) {
        /* There is no CU die, which should be impossible. */
        if (res == DW_DLV_ERROR) {
            printf("ERROR: dwarf_siblingof_b failed, no CU die\n");
            printf("Error is: %s\n",dwarf_errmsg(*e));
            return res;
        } else {
            printf("ERROR: dwarf_siblingof_b got NO_ENTRY, "
                "no CU die\n");
            return res;
        }
    }

    res = ocdwarf_one_die(p, s, cu_die, level, e);
    if (res != DW_DLV_OK) {
        dwarf_dealloc_die(cu_die);
        printf("ocdwarf_one_die failed! %d\n",res);
        return res;
    }

    dwarf_dealloc_die(cu_die);
    return DW_DLV_OK;
  }

  return DW_DLV_ERROR;
}

int ocdwarf_run(handle_t p, handle_t s) {
  int n = 0;
  if (isopcode(p) && isopshdr(s)) {
    popcode_t oc = CAST(popcode_t, p);
    Dwarf_Error error = 0;

    int res = dwarf_object_init_b(&dw_interface, 0, 0, DW_GROUPNUMBER_ANY,
                                   CAST(Dwarf_Debug*, &oc->items[OPCODE_DWARF]), &error);
    if (res == DW_DLV_NO_ENTRY) {
        printf("FAIL Cannot dwarf_object_init_b() NO ENTRY. \n");
        exit(EXIT_FAILURE);
    } else if (res == DW_DLV_ERROR){
        printf("FAIL Cannot dwarf_object_init_b(). \n");
        printf("msg: %s\n",dwarf_errmsg(error));
        dwarf_dealloc_error(oc->items[OPCODE_DWARF], error);
        exit(EXIT_FAILURE);
    }
    res = ocdwarf_do(p, s, &error);
    if (res != DW_DLV_OK) {
        if (res == DW_DLV_ERROR) {
            dwarf_dealloc_error(oc->items[OPCODE_DWARF], error);
        }
        printf("FAIL printing, res %d line %d\n",res,__LINE__);
        exit(EXIT_FAILURE);
    }

//    pdwarf_display_t d = ocdwarf_get(s);
//    n = d && d->func ? d->func(p, s, &d->section) : -1;

    dwarf_object_finish(oc->items[OPCODE_DWARF]);
    oc->items[OPCODE_DWARF] = NULL;
  } else if (isoptions(p) && isopshdrNN(s)) {
    pdwarf_display_t d = ocdwarf_get(s);
    n = d && d->func ? d->func(p, s, &d->section) : -1;
  }

  return n;
}

