#include "opcode.h"
#include "opcode-bfd.h"

int opcodebfd_programs(handle_t p, opcbfunc_t cbfunc, unknown_t param) {
  bfd* p0 = ocget(p, OPCODE_BFD);
  if (p0) {
    bfd_phdr_t* phdr = bfdget_phdr(p0);
    if (phdr) {
      size_t e_phnum = bfdget_ehdr(p0)->e_phnum;
      for (size_t i = 0; i < e_phnum; ++i, ++phdr) {
        MALLOCSMODE(opwrap_t, oc, MODE_OCPHDR);
        poc->item = phdr;
        cbfunc(p, poc, param);
      }
      return 0;
    }
  }

  return -1;
}

