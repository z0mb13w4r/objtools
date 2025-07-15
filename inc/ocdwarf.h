#ifndef __OCDWARF_H_
#define __OCDWARF_H_

#include <dwarf.h>
#include <libdwarf.h>

#include "opcode.h"
#include "externs.h"

#define RAGE_DWARF            {'D', 'W', 'F'}
#define MODE_DWARF            (MODE_PUT0('D') | MODE_PUT1('W') | MODE_PUT2('F'))

#define IS_DLV_OK(x)          ((x) == DW_DLV_OK)
#define IS_DLV_ERROR(x)       ((x) == DW_DLV_ERROR)
#define IS_DLV_NO_ENTRY(x)    ((x) == DW_DLV_NO_ENTRY)
#define IS_DLV_ANY_ERROR(x)   ((x) != DW_DLV_OK)

typedef enum {
  eDWARF_DEBUG_ABBREV,
  eDWARF_DEBUG_ABBREV_DWO,
  eDWARF_DEBUG_ADDR,
  eDWARF_DEBUG_ARANGES,
  eDWARF_DEBUG_CU_INDEX,
  eDWARF_DEBUG_FRAME,
  eDWARF_DEBUG_GNU_PUBNAMES,
  eDWARF_DEBUG_GNU_PUBTYPES,
  eDWARF_DEBUG_INFO,
  eDWARF_DEBUG_INFO_DWO,
  eDWARF_DEBUG_LINE,
  eDWARF_DEBUG_LINE_DWO,
  eDWARF_DEBUG_LINE_STR,
  eDWARF_DEBUG_LOC,
  eDWARF_DEBUG_LOCLISTS,
  eDWARF_DEBUG_LOCLISTS_DWO,
  eDWARF_DEBUG_LOC_DWO,
  eDWARF_DEBUG_MACINFO,
  eDWARF_DEBUG_MACINFO_DWO,
  eDWARF_DEBUG_MACRO,
  eDWARF_DEBUG_MACRO_DWO,
  eDWARF_DEBUG_NAMES,
  eDWARF_DEBUG_PUBNAMES,
  eDWARF_DEBUG_PUBTYPES,
  eDWARF_DEBUG_RANGES,
  eDWARF_DEBUG_RNGLISTS,
  eDWARF_DEBUG_RNGLISTS_DWO,
  eDWARF_DEBUG_STATIC_FUNC,
  eDWARF_DEBUG_STATIC_VARS,
  eDWARF_DEBUG_STR,
  eDWARF_DEBUG_STR_DWO,
  eDWARF_DEBUG_STR_OFFSETS,
  eDWARF_DEBUG_STR_OFFSETS_DWO,
  eDWARF_DEBUG_SUP,
  eDWARF_DEBUG_TU_INDEX,
  eDWARF_DEBUG_TYPES,
  eDWARF_DEBUG_TYPES_DWO,
  eDWARF_DEBUG_WEAKNAMES,
  eDWARF_EH_FRAME,
  eDWARF_GDB_INDEX,
  eDWARF_GNU_DEBUGALTLINK,
  eDWARF_GNU_DEBUGLINK,
  eDWARF_TRACE_ABBREV,
  eDWARF_TRACE_ARANGES,
  eDWARF_TRACE_INFO,
  eDWARF_MAX
} dwarfid_t;

typedef struct dwarf_section_s {
  smode_t mode;
  const char* uncompressed_name;
  const char* compressed_name;
  const char* xcoff_name;
  dwarfid_t   id;
} dwarf_section_t, *pdwarf_section_t;

typedef struct dwarf_display_s {
  dwarf_section_t section;
  imode_t         action;
  int             (*func)(handle_t p, handle_t s, handle_t d);
} dwarf_display_t, *pdwarf_display_t;

typedef struct dwarf_srcfiles_s {
  char       **data;
  Dwarf_Signed size;
  int          status;
} dwarf_srcfiles_t, *pdwarf_srcfiles_t;

typedef struct dwarf_statistics_s {
  Dwarf_Off soffset;
  Dwarf_Off eoffset;
  Dwarf_Unsigned idepth;
  Dwarf_Unsigned sdepth;
  Dwarf_Unsigned mcount;
} dwarf_statistics_t, *pdwarf_statistics_t;

typedef struct ocdwarf_s {
  Dwarf_Debug  dbg;
  Dwarf_Error  err;
  pdwarf_srcfiles_t sf;
  pdwarf_statistics_t st;

  Dwarf_Half cu_offset_size;
  Dwarf_Half cu_version_stamp;
} ocdwarf_t, *pocdwarf_t;

extern const Dwarf_Sig8  ZEROSIGNATURE;
extern const Dwarf_Block ZEROBLOCK;

pdwarf_display_t ocdwarf_get(handle_t s);

bool_t ocdwarf_isneeded(handle_t p, handle_t s);

int ocdwarf_open(handle_t p, handle_t o);
int ocdwarf_close(handle_t p);

int ocdwarf_run(handle_t p, handle_t s);

int ocdwarf_sfreset(handle_t p);
int ocdwarf_sfcreate(handle_t p, Dwarf_Die die, Dwarf_Error *e);

int ocdwarf_spget(handle_t p, Dwarf_Addr addr, char **name,
                     Dwarf_Unsigned *nline, Dwarf_Unsigned *ncolumn, Dwarf_Error *e);

int ocdwarf_next_cu_header(handle_t p, Dwarf_Die *cu_die, Dwarf_Error *e);

void ocdwarf_dealloc(handle_t p, unknown_t v, Dwarf_Unsigned type);
void ocdwarf_dealloc_attribute(handle_t p, Dwarf_Attribute *v, Dwarf_Signed size);
void ocdwarf_dealloc_fde_cie_list(handle_t p, Dwarf_Cie *cie_data, Dwarf_Signed cie_element_count,
                     Dwarf_Fde *fde_data, Dwarf_Signed fde_element_count);
void ocdwarf_dealloc_error(handle_t p, Dwarf_Error *e);

void ocdwarf_finish(handle_t p, Dwarf_Error *e);
void ocdwarf_object_finish(handle_t p);

#endif

