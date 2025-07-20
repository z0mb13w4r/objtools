#include "options.h"
#include "opcode-printf.h"

int opcode_printf_DEC(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISSET(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      return printf_nice(v, USE_DEC2 | mode);
    }
  }

  return printf_nice(v, USE_DEC | mode);
}

int opcode_printf_FHEX(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISSET(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      return printf_nice(v, USE_FHEX32 | mode);
    }
  }

  return printf_nice(v, USE_FHEX | mode);
}

int opcode_printf_LHEX(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISSET(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      return printf_nice(v, USE_LHEX32 | mode);
    }
  }

  return printf_nice(v, USE_LHEX | mode);
}

int opcode_printf_FADDR(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    if (ocis32(p)) {
      return printf_nice(v, USE_FHEX32 | mode);
    } else if (ocis64(p)) {
      return printf_nice(v, USE_FHEX64 | mode);
    }
  }

  return printf_nice(v, USE_FHEX32 | mode);
}

int opcode_printf_LADDR(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    if (ocis32(p)) {
      return printf_nice(v, USE_LHEX32 | mode);
    } else if (ocis64(p)) {
      return printf_nice(v, USE_LHEX64 | mode);
    }
  }

  return printf_nice(v, USE_LHEX32 | mode);
}

int opcode_printf_pluck(handle_t p, const pconvert_t z, const pick_t x, const imode_t mode) {
  int n = 0;
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      n += printf_nice(x, USE_FHEX16);
    } else if (MODE_ISSET(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      n += printf_nice(x, USE_FHEX8);
    }

    n += printf_pick(z, x, USE_SPACE | (mode & ~USE_SPECIAL));
  }

  return n;
}

