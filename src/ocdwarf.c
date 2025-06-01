#include <bfd.h>
#include <dwarf.h>
#include <libdwarf.h>

#include "printf.h"
#include "opcode.h"
#include "ocdwarf.h"
#include "options.h"
#include "ocdwarf-macroinfo.h"

#include <fcntl.h>

//static const int MAXSIZE = 31;

static const Dwarf_Sig8 zerosignature;

static int ocdwarf_dodebug_abbrev(handle_t p, handle_t s, handle_t d) { return 0; }

static int ocdwarf_dodebug_aranges(handle_t p, handle_t s, handle_t d) {
  printf_text("Length", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
  printf_text("Version", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
  printf_text("Offset into .debug_info", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
  printf_text("Pointer Size", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
  printf_text("Segment Size", USE_LT | USE_SPACE | USE_COLON | USE_EOL);

  printf_text("Address Length", USE_LT | USE_TAB | USE_EOL);

  return 0;
}

static int ocdwarf_dodebug_info(handle_t p, handle_t s, handle_t d) {
  int n = 0;


//  printf_text("Compilation Unit @ offset 0x0", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
//  printf_text("Length", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//  printf_text("Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//  printf_text("Abbrev Offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//  printf_text("Pointer Size", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);

  return n;
}

static int ocdwarf_dodebug_line(handle_t p, handle_t s, handle_t d) {
  if (isopcode(p)) {
    popcode_t p0 = CAST(popcode_t, p);
    if (MODE_ISSET(p0->ocdump, OPTDEBUGELF_DEBUG_LINE)) {
//      printf_text("Offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_text("Length", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_text("DWARF Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_text("Prologue Length", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_text("Minimum Instruction Length", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_text("Initial value of 'is_stmt'", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_text("Line Base", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_text("Line Range", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_text("Opcode Base", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_eol();
//      printf_text("Opcodes", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
//      printf_eol();
//      printf_text("The Directory Table", USE_LT | USE_SPACE);
//      printf_nice(0x1b, USE_COLON | USE_FHEX8 | USE_OFFSET | USE_EOL);
//      printf_eol();
//      printf_text("The File Name Table", USE_LT | USE_SPACE);
//      printf_nice(0xf8, USE_COLON | USE_FHEX8 | USE_OFFSET | USE_EOL);
//      printf_eol();
//      printf_text("Line Number Statements", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
//      printf_eol();
    }

    if (MODE_ISSET(p0->ocdump, OPTDEBUGELF_DEBUG_LINE_DECODED)) {
    }
  }

  return 0;
}

static int ocdwarf_debug_macroinfo(handle_t p, handle_t s, handle_t d) {
  int n = 0;
  n += ocdwarf_debug_macroinfo0a(p, s, d, 0x00000000, 0x0000000b, 0, 4, 232, 803);
  n += ocdwarf_debug_macroinfo1a(p, s, d, 0, 0x07, 0x0000032a, 1, 4);
  n += ocdwarf_debug_macroinfo2a(p, s, d, 1, 0x03, 0, 1, "/media/SHYLA/objtools/misc/debugLINUX64/../../src/example.c");
  n += ocdwarf_debug_macroinfo2a(p, s, d, 2, 0x03, 0, 9, "/usr/include/stdc-predef.h");
  n += ocdwarf_debug_macroinfo1a(p, s, d, 3, 0x07, 0x00000b4a, 1, 4);
  n += ocdwarf_debug_macroinfo3a(p, s, d, 4, 0x04);
  n += ocdwarf_debug_macroinfo4a(p, s, d, 6, 0x05, 23, 0x0000230a, "_CTYPE_H 1");
  n += ocdwarf_debug_macroinfo4a(p, s, d, 40, 0x06, 225, 0x0000061c, "__STD_TYPE");
  n += printf_eol();

  n += ocdwarf_debug_macroinfo0b(p, s, d, 0x0000032a, 0, 4, 347, 2077);
  n += ocdwarf_debug_macroinfo4a(p, s, d, 0, 0x05, 0, 0x00003ba7, "__STDC__ 1");

//  printf_text("Offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//  printf_text("Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//  printf_text("Offset size", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//  printf_text("Offset into", USE_LT | USE_TAB | SET_PAD(MAXSIZE) | USE_EOL);

  return n;
}

static int ocdwarf_dodebug_str(handle_t p, handle_t s, handle_t d) { return 0; }
static int ocdwarf_dodebug_stroffset(handle_t p, handle_t s, handle_t d) { return 0; }
static int ocdwarf_dodebug_types(handle_t p, handle_t s, handle_t d) { return 0; }
static int ocdwarf_doeh_frame(handle_t p, handle_t s, handle_t d) { return 0; }

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

    return 0;
  }

  return -1;
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

struct srcfilesdata {
  char ** srcfiles;
  Dwarf_Signed srcfilescount;
  int srcfilesres;
};

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

static Dwarf_Addr low_pc_addr = 0;

static void ocdwarf_sfreset(handle_t p, handle_t s, struct srcfilesdata *sf) {
  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);

    if (sf->srcfiles) {
      for (Dwarf_Signed sri = 0; sri < sf->srcfilescount; ++sri) {
        dwarf_dealloc(oc->items[OPCODE_DWARF], sf->srcfiles[sri], DW_DLA_STRING);
      }
      dwarf_dealloc(oc->items[OPCODE_DWARF], sf->srcfiles, DW_DLA_LIST);
    }

    sf->srcfilesres = DW_DLV_ERROR;
    sf->srcfiles = 0;
    sf->srcfilescount = 0;
  }
}

static int ocdwarf_printf_attr(handle_t p, handle_t s, Dwarf_Attribute attr, Dwarf_Signed anumber, Dwarf_Error *e) {
  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);

    int res = 0;

    Dwarf_Half formnum = 0;
    res = dwarf_whatform(attr, &formnum, e);
    if (res != DW_DLV_OK) {
      printf_e("dwarf_whatform failed! res %d", res);
      return res;
    }

    Dwarf_Half attrnum = 0;
    res = dwarf_whatattr(attr, &attrnum, e);
    if (res != DW_DLV_OK) {
      printf_e("dwarf_whatattr failed! res %d", res);
      return res;
    }

    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      printf_nice(anumber, USE_DEC2 | USE_SB | USE_TAB);
      printf_nice(attrnum, USE_FHEX16);
    } else {
      printf_pack(8);
    }

    printf_pick(zDWAT, attrnum, USE_SPACE | SET_PAD(18));

    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      printf_nice(formnum, USE_FHEX8);
      printf_pick(zDWFORM, formnum, USE_SPACE | SET_PAD(22));
    }

    if (isused(zFORMSTRING, formnum)) {
      char *str = NULL;
      res = dwarf_formstring(attr, &str, e);
      if (res != DW_DLV_OK) {
        printf_e("dwarf_formstring failed! res %d", res);
        return res;
      }

      printf_text(str, USE_LT | USE_SPACE);
    } else if (isused(zFORMUDATA, formnum)) {
      Dwarf_Unsigned value = 0;
      res = dwarf_formudata(attr, &value, e);
      if (res != DW_DLV_OK) {
        printf_e("dwarf_formudata failed! res %d", res);
        return res;
      }

      if (DW_AT_high_pc == attrnum) {
        printf_text("offset-from-lowpc", USE_LT | USE_SPACE | USE_TB);
        printf_nice(value, USE_DEC);
        printf_text("highpc", USE_LT | USE_SPACE | USE_TBLT | USE_COLON);
        printf_nice(low_pc_addr + value, USE_FHEX32 | USE_TBRT);
      } else if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
        printf_nice(value, USE_FHEX16);
      }

      if (DW_AT_language == attrnum) {
        printf_pick(zDWLANG, value, USE_SPACE);
      }
    } else if (isused(zFORMADDR, formnum)) {
      Dwarf_Addr addr = 0;
      res = dwarf_formaddr(attr, &addr, e);
      if (res != DW_DLV_OK) {
        printf_e("dwarf_formaddr failed! res %d", res);
        return res;
      }

      if (DW_AT_low_pc == attrnum) {
        low_pc_addr = addr;
      }

      printf_nice(addr, USE_FHEX32);
    } else if (isused(zFORMGREF, formnum)) {
      Dwarf_Unsigned value = 0;
      res = dwarf_global_formref(attr, &value, e);
      if (res != DW_DLV_OK) {
        printf_e("dwarf_global_formref failed! res %d", res);
        return res;
      }

      printf_nice(value, USE_FHEX32);
    }

    printf_eol();

    return DW_DLV_OK;
  }

  return DW_DLV_ERROR;
}

static void ocdwarf_dealloc(handle_t p, handle_t s, Dwarf_Attribute *a, Dwarf_Signed size, Dwarf_Signed i) {
  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);

    for ( ; i < size; ++i) {
      dwarf_dealloc_attribute(a[i]);
    }

    dwarf_dealloc(oc->items[OPCODE_DWARF], a, DW_DLA_LIST);
  }
}

static int ocdwarf_die_one(handle_t p, handle_t s, Dwarf_Die die, int level, Dwarf_Error *e) {
  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);
    int res = DW_DLV_OK;

//    char *name = 0;
//    res = dwarf_diename(in_die, &name, e);
//    if (res == DW_DLV_ERROR) {
//      printf_e("dwarf_diename, level %d", level);
//      return res;
//    }

//    printf("%s\n", name);

    Dwarf_Half tag = 0;
    res = dwarf_tag(die, &tag, e);
    if (res != DW_DLV_OK) {
      printf_e("dwarf_tag failed! res %d", res);
      return res;
    }

    Dwarf_Off overall_offset = 0;
    res = dwarf_dieoffset(die, &overall_offset, e);
    if (res != DW_DLV_OK) {
      printf_e("dwarf_dieoffset failed! res %d", res);
      return res;
    }

    Dwarf_Off offset = 0;
    res = dwarf_die_CU_offset(die, &offset, e);
    if (res != DW_DLV_OK) {
      printf_e("dwarf_die_CU_offset failed! res %d", res);
      return res;
    }

    printf_text("COMPILE_UNIT<header overall offset =", USE_LT);
    printf_nice(overall_offset - offset, USE_FHEX32 | USE_TBRT | USE_COLON | USE_EOL);

    printf_nice(level, USE_DEC2 | USE_TB);
    printf_nice(0xffffffff, USE_FHEX32 | USE_TB);
    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      printf_nice(tag, USE_FHEX16);
    }
    printf_pick(zDWTAG, tag, USE_SPACE | USE_EOL);

    Dwarf_Signed attrcount = 0;
    Dwarf_Attribute *attrbuf = 0;
    res = dwarf_attrlist(die, &attrbuf, &attrcount, e);
    if (res != DW_DLV_OK) {
      printf_e("dwarf_attrlist failed! res %d", res);
      return res;
    }

    for (Dwarf_Signed i = 0; i < attrcount; ++i) {
      res = ocdwarf_printf_attr(p, s, attrbuf[i], i, e);
      if (res != DW_DLV_OK) {
        ocdwarf_dealloc(p, s, attrbuf, attrcount, 0);
        printf("dwarf_attr print failed! res %d", res);
        return res;
      }
    }

    ocdwarf_dealloc(p, s, attrbuf, attrcount, 0);
    return DW_DLV_OK;
  }

  return DW_DLV_ERROR;
}

static int ocdwarf_die_data(handle_t p, handle_t s, Dwarf_Die die,
                  Dwarf_Bool isinfo, int level, struct srcfilesdata *sf, Dwarf_Error *e) {
  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);
    int res = DW_DLV_OK;

    char *name = 0;
    res = dwarf_diename(die, &name, e);
    if (res == DW_DLV_ERROR) {
      dwarf_object_finish(oc->items[OPCODE_DWARF]);
      printf_x("dwarf_diename, level %d", level);
    } else if (res == DW_DLV_NO_ENTRY) {
      name = "<no DW_AT_name attr>";
    }

    printf_text(name, USE_LT | USE_EOL);

    Dwarf_Half tag = 0;
    res = dwarf_tag(die, &tag, e);
    if (res != DW_DLV_OK) {
      if (res == DW_DLV_ERROR && e) {
        dwarf_dealloc_error(oc->items[OPCODE_DWARF], *e);
      }
      dwarf_object_finish(oc->items[OPCODE_DWARF]);
      printf_x("dwarf_tag, level %d", level);
    }

    const char *tagname = 0;
    res = dwarf_get_TAG_name(tag, &tagname);
    if (res != DW_DLV_OK) {
      if (res == DW_DLV_ERROR && e) {
        dwarf_dealloc_error(oc->items[OPCODE_DWARF], *e);
      }

      dwarf_object_finish(oc->items[OPCODE_DWARF]);
      printf_x("dwarf_get_TAG_name, level %d", level);
    }

    printf_text(tagname, USE_LT | USE_EOL);

    Dwarf_Attribute attr = 0;
    res = dwarf_attr(die, DW_AT_name, &attr, e);
    if (res == DW_DLV_OK) {
      Dwarf_Half formnum = 0;
      res = dwarf_whatform(attr, &formnum, e);
      if (res != DW_DLV_OK) {
        if (res == DW_DLV_ERROR && e) {
          dwarf_dealloc_error(oc->items[OPCODE_DWARF], *e);
        }

        dwarf_object_finish(oc->items[OPCODE_DWARF]);
        printf_x("dwarf_whatform, level %d", level);
      }

      const char *formname = 0;
      res = dwarf_get_FORM_name(formnum, &formname);
      if (res != DW_DLV_OK) {
        formname = "form-name-unavailable";
      }

      dwarf_dealloc(oc->items[OPCODE_DWARF], attr, DW_DLA_ATTR);
      printf_text(formname, USE_LT | USE_EOL);
    }

    return DW_DLV_OK;
  }

  return DW_DLV_ERROR;
}

static int ocdwarf_die_and_siblings(handle_t p, handle_t s, Dwarf_Die die,
                  Dwarf_Bool isinfo, int level, struct srcfilesdata *sf, Dwarf_Error *e) {
  if (isopcode(p)) {
    return ocdwarf_die_data(p, s, die, isinfo, level, sf, e);
  }

  return DW_DLV_ERROR;
}

static int ocdwarf_do(handle_t p, handle_t s, Dwarf_Error *e) {
  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
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
    int            level = 0;

    for ( ; ; ) {
      Dwarf_Die no_die = 0;
      Dwarf_Die cu_die = 0;

      MALLOCS(struct srcfilesdata, sf);
      sf.srcfilesres = DW_DLV_ERROR;

      int res = dwarf_next_cu_header_d(oc->items[OPCODE_DWARF], is_info,
                  &cu_header_length, &version_stamp, &abbrev_offset, &address_size, &length_size, &extension_size,
                  &type_signature, &typeoffset, &next_cu_header_offset, &header_cu_type, e);
      if (res == DW_DLV_NO_ENTRY) break;
      else if (res != DW_DLV_OK) {
        if (res == DW_DLV_ERROR) {
          printf_e("dwarf errmsg: %s", dwarf_errmsg(*e));
        }
        printf_x("Next cu header result %d, line %d", res, __LINE__);
      }

      printf("CU header length..........0x%lx\n", (unsigned long)cu_header_length);
      printf("Version stamp.............%d\n",version_stamp);
      printf("Address size .............%d\n",address_size);
      printf("Offset size...............%d\n",length_size);
      printf("Next cu header offset.....0x%lx\n", (unsigned long)next_cu_header_offset);

      res = dwarf_siblingof_b(oc->items[OPCODE_DWARF], no_die, is_info, &cu_die, e);
      if (DW_DLV_NO_ENTRY == res) return DW_DLV_OK;
      else if (res != DW_DLV_OK) {
        /* There is no CU die, which should be impossible. */
        if (res == DW_DLV_ERROR) {
          printf_e("dwarf errmsg: %s", dwarf_errmsg(*e));
          printf_e("dwarf_siblingof_b failed, no CU die");
          return res;
        } else {
          printf_e("dwarf_siblingof_b got NO_ENTRY, no CU die");
          return res;
        }
      }

      res = ocdwarf_die_and_siblings(p, s, cu_die, is_info, level, &sf, e);
//      res = ocdwarf_one_die(p, s, cu_die, level, e);
      if (res != DW_DLV_OK) {
        dwarf_dealloc_die(cu_die);
        printf_e("ocdwarf_one_die failed! %d", res);
        return res;
      }

      dwarf_dealloc_die(cu_die);
      ocdwarf_sfreset(p, s, &sf);
    }

    return DW_DLV_OK;
  }

  return DW_DLV_ERROR;
}

int ocdwarf_run(handle_t p, handle_t s) {
  int n = 0;
  if (isopcode(p) && isopshdr(s)) {
    popcode_t oc = CAST(popcode_t, p);
    Dwarf_Error error = 0;

    int res = dwarf_object_init_b(&dw_interface, 0, 0, DW_GROUPNUMBER_ANY, ocgetdwarfptr(p), &error);
    if (res == DW_DLV_NO_ENTRY) {
      printf_x("Cannot dwarf_object_init_b() NO ENTRY.");
    } else if (res == DW_DLV_ERROR) {
      printf_e("dwarf errmsg: %s", dwarf_errmsg(error));
      dwarf_dealloc_error(oc->items[OPCODE_DWARF], error);
      printf_x("Cannot dwarf_object_init_b().");
    }

    res = ocdwarf_do(p, s, &error);
    if (res == DW_DLV_ERROR) {
      dwarf_dealloc_error(oc->items[OPCODE_DWARF], error);
    }

//    pdwarf_display_t d = ocdwarf_get(s);
//    n = d && d->func ? d->func(p, s, &d->section) : -1;

    dwarf_object_finish(oc->items[OPCODE_DWARF]);
    oc->items[OPCODE_DWARF] = NULL;
  } else if (isopcode(p) && isopshdrNN(s)) {
    pdwarf_display_t d = ocdwarf_get(s);
    if (d && d->func) {
      popcode_t oc = CAST(popcode_t, p);
      Dwarf_Error error = 0;

//      sectiondata[1].sd_sectionsize = ocget_size(s);
//      sectiondata[1].sd_secname = ocget_name(s);
//      sectiondata[1].sd_content = ocget_data(s);

      int my_init_fd = open(oc->inpname0, O_RDONLY);
      if (-1 == my_init_fd) {
        printf_x("Giving up, cannot open '%s'", oc->inpname0);
      }

      int res = dwarf_init_b(my_init_fd, DW_GROUPNUMBER_ANY, 0, 0, ocgetdwarfptr(p), &error);
      if (res != DW_DLV_OK) {
        printf_x("Giving up, cannot do DWARF processing '%s'", oc->inpname0);
      }

      res = ocdwarf_do(p, s, &error);
      if (res == DW_DLV_ERROR) {
        dwarf_dealloc_error(oc->items[OPCODE_DWARF], error);
      }

//      n = d && d->func ? d->func(p, s, &d->section) : -1;
      dwarf_object_finish(oc->items[OPCODE_DWARF]);
      oc->items[OPCODE_DWARF] = NULL;
    }
  } else if (isoptions(p) && isopshdrNN(s)) {
    pdwarf_display_t d = ocdwarf_get(s);
    n = d && d->func ? d->func(p, s, &d->section) : -1;
  }

  return n;
}

