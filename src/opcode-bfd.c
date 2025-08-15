#include "objutils.h"
#include "opcode-bfd.h"

#include "static/bfd.ci"
#include "static/has_flags.ci"
#include "static/sectionhdr_flags.ci"

static void callback_section(bfd *f, asection *s, void *p) {
  popfunc_t pcb = CAST(popfunc_t, p);
  if (pcb && pcb->cbfunc) {
    MALLOCSWRAP(opwrap_t, sec, MODE_OCSHDR, s);
    pcb->cbfunc(pcb->handle, psec, pcb->param);
  }
}

int opcodebfd_programs(handle_t p, opcbfunc_t cbfunc, unknown_t param) {
  bfd* p0 = ocget(p, OPCODE_BFD);
  if (p0) {
    bfd_phdr_t* phdr = bfdget_phdr(p0);
    if (phdr) {
      size_t e_phnum = bfdget_ehdr(p0)->e_phnum;
      for (size_t i = 0; i < e_phnum; ++i, ++phdr) {
        MALLOCSWRAP(opwrap_t, oc, MODE_OCPHDR, phdr);
        cbfunc(p, poc, param);
      }
      return ECODE_OK;
    }
  }

  return ECODE_HANDLE;
}

int opcodebfd_sections(handle_t p, opcbfunc_t cbfunc, unknown_t param) {
  bfd* p0 = ocget(p, OPCODE_BFD);
  if (p0) {
    MALLOCSCBFUNC(opfunc_t, cb, MODE_OPCBFUNC, param, cbfunc, p);
    bfd_map_over_sections(p0, callback_section, pcb);
    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

//#include "printf.h"
char* opcodebfd_getsymbol0(handle_t p, const uint64_t vaddr, uint64_t *offset) {
  if (isopcode(p)) {
    pbuffer_t ps = ocget(p, OPCODE_SYMBOLS);
    if (ps && ps->size) {
      char *name = NULL;
      asymbol **cs = CAST(asymbol**, ps->data);
      for (size_t i = 0; i < ps->size; ++i) {
        if (cs[i] && 0 == (cs[i]->flags & BSF_SECTION_SYM) && (BSF_GLOBAL != cs[i]->flags)) {
          if (offset) {
            uint64_t caddr = bfd_asymbol_value(cs[i]);
            if (caddr <= vaddr) {
              uint64_t offset0 = vaddr - caddr;
              if (offset0 < *offset) {
//printf_mask(zBFDSYMBOL_FLAGS, cs[i]->flags, USE_NONE);
                name = CAST(char*, bfd_asymbol_name(cs[i]));
                *offset = offset0;
              }
            }
          } else if (bfd_asymbol_value(cs[i]) == vaddr) {
//printf_mask(zBFDSYMBOL_FLAGS, cs[i]->flags, USE_NONE);
            name = CAST(char*, bfd_asymbol_name(cs[i]));
            break;
          }
        }
      }

      return name && name[0] ? name : NULL;
    }
  }

  return NULL;
}

char* opcodebfd_getsymbol1(handle_t p, const uint64_t vaddr, uint64_t *offset) {
  STATICA(char, name, 1024);

  if (isopcode(p)) {
    pbuffer_t pr = ocget(p, OPCODE_SYMBOLS_DYNAMICRELOC);
    if (pr && pr->size) {
      arelent **cr = CAST(arelent **, pr->data);
      for (size_t i = 0; i < pr->size; ++i) {
        if (cr[i] && vaddr == cr[i]->address) {

          asymbol *sym = cr[i]->sym_ptr_ptr && *cr[i]->sym_ptr_ptr ? *cr[i]->sym_ptr_ptr : NULL;
//          asection *sec = sym && sym->section ? sym->section : NULL;
          const char *symname = sym ? bfd_asymbol_name(sym) : NULL;
          if (symname && symname[0]) {
            bool_t hidden = FALSE;
            const char *vername = NULL;
            if (0 == (sym->flags & (BSF_SECTION_SYM | BSF_SYNTHETIC))) {
              bfd* f = ocget(p, OPCODE_BFD);

              vername = bfd_get_symbol_version_string(f, sym, &hidden);
              if (bfd_is_und_section(bfd_asymbol_section(sym))) {
                hidden = TRUE;
              }
            }

            int n = snprintf(name, NELEMENTS(name), "%s", symname);
            if (vername && vername[0]) {
              snprintf(name + n, NELEMENTS(name) - n, hidden ? "@%s" : "@@%s", vername);
            }
//printf("++++");
            return name;
          }
//          const char *secname = sec ? bfd_section_name(sec) : NULL;
        }
      }
    }
  }

  return NULL;
}

char* opcodebfd_getsymbol2(handle_t p, const uint64_t vaddr, uint64_t *offset) {
  if (isopcode(p)) {
    pbuffer_t ps = ocget(p, OPCODE_SYMBOLS);
    if (ps && ps->size) {
      asymbol **cs = ps->data;
      for (size_t i = 0; i < ps->size; ++i) {
        if (cs[i] && vaddr == bfd_asymbol_value(cs[i])) {
          if (0 != (cs[i]->flags & BSF_SECTION_SYM)) {
//printf_mask(zBFDSYMBOL_FLAGS, cs[i]->flags, USE_NONE);
            char *name = CAST(char*, bfd_asymbol_name(cs[i]));
            return name && name[0] ? name : NULL;
          }
        }
      }
    }
  }

  return NULL;
}

char* opcodebfd_getsymbol(handle_t p, const uint64_t vaddr, uint64_t *offset) {
  if (isopcode(p)) {
    char* name = opcodebfd_getsymbol0(p, vaddr, NULL);
    if (NULL == name) {
      name = opcodebfd_getsymbol1(p, vaddr, NULL);
    }
    if (NULL == name) {
      name = opcodebfd_getsymbol2(p, vaddr, NULL);
    }
    if (NULL == name && offset) {
      uint64_t offset0 = UINT_MAX;
      name = opcodebfd_getsymbol0(p, vaddr, &offset0);
      *offset = offset0;
    }
    return name;
  }

  return NULL;
}

