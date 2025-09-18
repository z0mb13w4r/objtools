#include "ocdwarf-engine.h"

static int execute_store_sp(handle_t p, handle_t q, Dwarf_Die die,
                  Dwarf_Half tag, Dwarf_Bool isinfo, Dwarf_Error *e) {
  if (isopcode(p) && isocengine(q)) {
    Dwarf_Signed cattr = 0;
    Dwarf_Attribute *pattr = 0;
    int x = dwarf_attrlist(die, &pattr, &cattr, e);
    if (IS_DLV_OK(x)) {
      pocdebug_t d0 = NULL;
      pocgroups_t g0 = NULL;
      for (Dwarf_Signed i = 0; IS_DLV_OK(x) && (i < cattr); ++i) {
        Dwarf_Half nattr = 0;
        x = dwarf_whatattr(pattr[i], &nattr, e);
        if (IS_DLV_OK(x) && (DW_AT_low_pc == nattr)) {
          Dwarf_Addr vaddr = 0;
          if (IS_DLV_OK(dwarf_formaddr(pattr[i], &vaddr, e))) {
            g0 = oegetbyaddr(q, vaddr, OPENGINE_GROUP);
            if (g0 && (NULL == g0->debug)) {
              g0->debug = xmalloc(sizeof(ocdebug_t));
              g0->debug->laddr = vaddr;
            }
            d0 = g0->debug;
          }
          break;
        }
      }

// ocdwarf_printf_merit(p, die, attr, nattr, e);
      if (d0) {
        for (Dwarf_Signed i = 0; IS_DLV_OK(x) && (i < cattr); ++i) {
          Dwarf_Half nattr = 0;
          Dwarf_Half nform = 0;

          x = dwarf_whatattr(pattr[i], &nattr, e);
          if (IS_DLV_OK(x)) {
            x = dwarf_whatform(pattr[i], &nform, e);
          }
          if (IS_DLV_OK(x)) {
            if (isused(ecFORMSTRING, nform)) {
              char *str = NULL;
              if (IS_DLV_OK(dwarf_formstring(pattr[i], &str, e))) {
              }
            } else if (isused(ecFORMUDATA, nform) || isused(ecFORMBOOL, nform)) {
              Dwarf_Unsigned value = 0;
              if (IS_DLV_OK(dwarf_formudata(pattr[i], &value, e))) {
              }
            } else if (isused(ecFORMADDR, nform)) {
              Dwarf_Addr addr = 0;
              if (IS_DLV_OK(dwarf_formaddr(pattr[i], &addr, e))) {
              }
            } else if (isused(ecFORMGREF, nform)) {
              Dwarf_Unsigned value = 0;
              if (IS_DLV_OK(dwarf_global_formref(pattr[i], &value, e))) {
              }
            }
          }
        }
      }

      ocdwarf_dealloc_attribute(p, pattr, cattr);
    }

    return x;
  }

  return DW_DLV_ERROR;
}

static int execute_store_cu(handle_t p, handle_t q, Dwarf_Die die,
                  Dwarf_Half tag, Dwarf_Bool isinfo, Dwarf_Error *e) {
  if (isopcode(p) && isocengine(q)) {

    int x = execute_store_sp(p, q, die, tag, isinfo, e);
    if (IS_DLV_OK(x)) {
      x = ocdwarf_sfcreate(p, die, e);
    }

    return x;
  }

  return DW_DLV_ERROR;
}

static int execute_store(handle_t p, handle_t q, Dwarf_Die die,
                  Dwarf_Bool isinfo, Dwarf_Error *e) {
  if (isopcode(p) && isocengine(q)) {
    Dwarf_Half tag = 0;
    int x = dwarf_tag(die, &tag, e);
    if (IS_DLV_ANY_ERROR(x)) {
      if (IS_DLV_ERROR(x) && e) {
        ocdwarf_dealloc_error(p, e);
      }
      ocdwarf_object_finish(p);
      return x;
    }

    if (DW_TAG_subprogram == tag) {
      return execute_store_sp(p, q, die, tag, isinfo, e);
    } else if (DW_TAG_compile_unit == tag || DW_TAG_partial_unit == tag  || DW_TAG_type_unit == tag) {
      ocdwarf_sfreset(p);
      return execute_store_cu(p, q, die, tag, isinfo, e);
    }

    return  execute_store_sp(p, q, die, tag, isinfo, e);
  }

  return DW_DLV_ERROR;
}

static int execute_die_and_siblings(handle_t p, handle_t q, Dwarf_Die die,
                  Dwarf_Bool isinfo, Dwarf_Error *e) {
  if (isopcode(p) && isocengine(q)) {
    Dwarf_Die cur_die = die;

    for ( ; ; ) {
      Dwarf_Die child = 0;
      int x = dwarf_child(cur_die, &child, e);
      if (IS_DLV_ERROR(x)) {
        dwarf_dealloc_die(cur_die);
        ocdwarf_finish(p, e);
        return DW_DLV_ERROR;
      } else if (IS_DLV_OK(x)) {
        x = execute_die_and_siblings(p, q, child, isinfo, e);
        dwarf_dealloc_die(child);
        child = 0;
      }

      /* x == DW_DLV_NO_ENTRY or DW_DLV_OK */
      Dwarf_Die sib_die = 0;
      x = dwarf_siblingof_b(ocget(p, OPCODE_DWARF_DEBUG), cur_die, isinfo, &sib_die, e);
      if (IS_DLV_NO_ENTRY(x)) {
        return DW_DLV_NO_ENTRY;
      } else if (IS_DLV_ERROR(x)) {
        ocdwarf_finish(p, e);
        return DW_DLV_ERROR;
      } else if (cur_die != die) {
        dwarf_dealloc_die(cur_die);
      }

      cur_die = sib_die;
      x = execute_store(p, q, cur_die, isinfo, e);
    }
  }

  return DW_DLV_ERROR;
}

static int execute_next_cu_header(handle_t p, Dwarf_Die *cu_die, Dwarf_Error *e) {
  if (isopcode(p) && cu_die) {
    pocdwarf_t ws = ocget(p, OPCODE_DWARF);

    Dwarf_Bool     isinfo = TRUE; /* our data is not DWARF4 .debug_types. */
    Dwarf_Unsigned next_cu_header_offset = 0;
    Dwarf_Unsigned cu_header_length = 0;
    Dwarf_Unsigned type_offset    = 0;
    Dwarf_Half     extension_size = 0;
    Dwarf_Half     header_cu_type = 0;
    Dwarf_Half     address_size  = 0;
    Dwarf_Sig8     type_signature = ZEROSIGNATURE;
    Dwarf_Off      abbrev_offset = 0;

    return dwarf_next_cu_header_e(ocget(p, OPCODE_DWARF_DEBUG), isinfo, cu_die, &cu_header_length, &ws->cu_version_stamp,
                     &abbrev_offset, &address_size, &ws->cu_offset_size, &extension_size, &type_signature, &type_offset,
                     &next_cu_header_offset, &header_cu_type, e);
  }

  return DW_DLV_ERROR;
}

static handle_t execute_info(handle_t p, handle_t q, Dwarf_Error *e) {
  if (isopcode(p) && isocengine(q)) {
    Dwarf_Bool isinfo = TRUE; /* our data is not DWARF4 .debug_types. */

    for ( ; ; ) {
      Dwarf_Die no_die = 0;
      Dwarf_Die cu_die = 0;

      int x = execute_next_cu_header(p, &cu_die, e);
      if (IS_DLV_OK(x)) {
        x = dwarf_siblingof_b(ocget(p, OPCODE_DWARF_DEBUG), no_die, isinfo, &cu_die, e);
      }

      if (IS_DLV_ANY_ERROR(x)) {
        if (IS_DLV_ERROR(x)) {
          ocdwarf_dealloc_error(p, NULL);
        }

        return q;
      }

      x = execute_die_and_siblings(p, q, cu_die, isinfo, e);

      dwarf_dealloc_die(cu_die);
      ocdwarf_sfreset(p);

      if (IS_DLV_ANY_ERROR(x)) {
        return q;
      }
    }

    return q;
  }

  return NULL;
}

handle_t ocdwarf_create(handle_t p, handle_t q) {
  return execute_info(p, q, ocget(p, OPCODE_DWARF_ERROR));
}

