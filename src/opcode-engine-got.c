#include "opcode-engine-got.h"

static handle_t execute_new(handle_t p, const uint64_t vaddr, const char* name) {
  MALLOCA(char, oname, 1024);

  if (isoengine(p)) {
    pocgroups_t g0 = oegetbyaddr(p, vaddr, OPENGINE_GROUP);
    if (g0) {
      if (NULL == g0->symbol) {
        g0->symbol = osmalloc(vaddr);
      }

      int epos = snprintf(oname, sizeof(oname), "%s", name);
      char *cpos = strchr(oname, '@');
      if (cpos) {
        epos = cpos - oname;
      }
      snprintf(oname + epos, sizeof(oname) - epos, "@plt");

      g0->symbol->name = xstrdup(oname);
      g0->symbol->role |= OPSYMBOL_NAME;
    }
  }

  return p;
}

static uint32_t execute_u32(handle_t p, const uchar_t v0, const uchar_t v1, const uchar_t v2, const uchar_t v3) {
  return v0 | (v1 << 8) | (v2 << 16) | (v3 << 24);
}

static void execute_section32(handle_t p, handle_t s, handle_t q) {
  puchar_t pp = ocget_rawdata(s);
  if (pp) {
    uint64_t curr_vaddr = ocget_vmaddress(s);
    uint64_t prev_vaddr0 = 0;
    uint32_t prev_vaddr1 = 0;
    uint64_t prev_vaddr2 = ocget_vaddressbyname(p, ".got");
    for (uint64_t i = 0; i < ocget_size(s); ) {
      uint64_t siz = 1;

      if (0xf3 == pp[i + 0] && 0x0f == pp[i + 1] && 0x1e == pp[i + 2] && 0xfb == pp[i + 3]) { // endbr32
        prev_vaddr0 = curr_vaddr;
        siz = 4;
      } else if (0xff == pp[i + 0] && 0xa3 == pp[i + 1]) { // jmp
        prev_vaddr1 = execute_u32(p, pp[i + 2], pp[i + 3], pp[i + 4], pp[i + 5]);
        siz = 6;
      } else if (0x66 == pp[i + 0] && 0x0f == pp[i + 1] && 0x1f == pp[i + 2] && 0x44 == pp[i + 3] && 0x00 == pp[i + 4] && 0x00 == pp[i + 5]) { // nopw
        uint64_t this_vaddr = prev_vaddr2 + prev_vaddr1;
        execute_new(q, prev_vaddr0, ocget_namebyvaddr(p, this_vaddr, NULL));

        siz = 6;
      }

      i += siz;
      curr_vaddr += siz;
    }
  }
}

static void execute_section64(handle_t p, handle_t s, handle_t q) {
  puchar_t pp = ocget_rawdata(s);
  if (pp) {
    uint64_t curr_vaddr = ocget_vmaddress(s);
    uint64_t prev_vaddr0 = 0;
    uint32_t prev_vaddr1 = 0;
    for (uint64_t i = 0; i < ocget_size(s); ) {
      uint64_t siz = 1;
      if (0xf3 == pp[i + 0] && 0x0f == pp[i + 1] && 0x1e == pp[i + 2] && 0xfa == pp[i + 3]) { // endbr64
        prev_vaddr0 = curr_vaddr;
        siz = 4;
      } else if (0x0f == pp[i + 0] && 0x1f == pp[i + 1] && 0x44 == pp[i + 2] && 0x00 == pp[i + 3] && 0x00 == pp[i + 4]) { // nopl
        uint64_t this_vaddr = curr_vaddr + prev_vaddr1;
        execute_new(q, prev_vaddr0, ocget_namebyvaddr(p, this_vaddr, NULL));
        siz = 5;
      } else if (0x0f == pp[i + 0] && 0x1f == pp[i + 1] && 0x00 == pp[i + 2]) { // nopl
        siz = 3;
      } else if (0xf2 == pp[i + 0] && 0xff == pp[i + 1] && 0x25 == pp[i + 2]) { // bnd jmpq
        prev_vaddr1 = execute_u32(p, pp[i + 3], pp[i + 4], pp[i + 5], pp[i + 6]);
        siz = 7;
      } else if (0xf2 == pp[i + 0] && 0xe9 == pp[i + 1]) { // bnd jmpq
        siz = 6;
      } else if (0xff == pp[i + 0] && 0x35 == pp[i + 1]) { // pushq

        siz = 6;
      } else if (0x68 == pp[i + 0]) {
        siz = 5;
      }

      i += siz;
      curr_vaddr += siz;
    }
  }
}

static void callback_sections32(handle_t p, handle_t shdr, unknown_t param) {
  const char* name = ocget_name(shdr);
  if (0 == xstrcmp(name, ".plt.got")) {
    execute_section32(p, shdr, param);
  } else if (0 == xstrcmp(name, ".plt.sec")) {
    execute_section32(p, shdr, param);
  }
}

static void callback_sections64(handle_t p, handle_t shdr, unknown_t param) {
  const char* name = ocget_name(shdr);
  if (0 == xstrcmp(name, ".plt.got")) {
    execute_section64(p, shdr, param);
  } else if (0 == xstrcmp(name, ".plt.sec")) {
    execute_section64(p, shdr, param);
  }
}

handle_t opcode_create(handle_t p, handle_t q) {
  if (isopcode(p) && isoengine(q)) {
    if (ocisELF32(p)) {
      ocdo_sections(p, callback_sections32, q);
    } else if (ocisELF64(p)) {
      ocdo_sections(p, callback_sections64, q);
    }
    return q;
  }

  return NULL;
}

