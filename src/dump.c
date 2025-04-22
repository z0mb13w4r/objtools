#include "dump.h"
#include "decode.h"
#include "opcode.h"
#include "printf.h"

int dump_actions0(const pbuffer_t p, const poptions_t o, const uint64_t offset, const uint64_t size) {
  paction_t x = o->actions;
  while (x) {
    if (0 != size) {
      unknown_t p0 = getp(p, offset, size);
      switch (x->action) {
      case ACT_ROT5:   rot5(p0, size);            break;
      case ACT_NOT8:   not8(p0, size);            break;
      case ACT_ROT13:  rot13(p0, size);           break;
      case ACT_NOT16:  not16(p0, size);           break;
      case ACT_ROT18:  rot18(p0, size);           break;
      case ACT_NOT32:  not32(p0, size);           break;

      case ACT_ADD8:   add8(p0, x->value, size);  break;
      case ACT_ROL8:   rol8(p0, x->value, size);  break;
      case ACT_ROR8:   ror8(p0, x->value, size);  break;
      case ACT_SHL8:   shl8(p0, x->value, size);  break;
      case ACT_SHR8:   shr8(p0, x->value, size);  break;
      case ACT_SUB8:   sub8(p0, x->value, size);  break;
      case ACT_XOR8:   xor8(p0, x->value, size);  break;

      case ACT_ADD16:  add16(p0, x->value, size); break;
      case ACT_ROL16:  rol16(p0, x->value, size); break;
      case ACT_ROR16:  ror16(p0, x->value, size); break;
      case ACT_SHL16:  shl16(p0, x->value, size); break;
      case ACT_SHR16:  shr16(p0, x->value, size); break;
      case ACT_SUB16:  sub16(p0, x->value, size); break;
      case ACT_XOR16:  xor16(p0, x->value, size); break;

      case ACT_ADD32:  add32(p0, x->value, size); break;
      case ACT_ROL32:  rol32(p0, x->value, size); break;
      case ACT_ROR32:  ror32(p0, x->value, size); break;
      case ACT_SHL32:  shl32(p0, x->value, size); break;
      case ACT_SHR32:  shr32(p0, x->value, size); break;
      case ACT_SUB32:  sub32(p0, x->value, size); break;
      case ACT_XOR32:  xor32(p0, x->value, size); break;

      default:
        break;
      }
    }

    x = x->actions;
  }

  return 0;
}

int dump_actions1(const pbuffer_t p, const poptions_t o, const handle_t s, const char* name, const int action,
                  const uint64_t offset, const uint64_t size, const uint64_t vaddr) {
  int n = 0;

  handle_t oc = ocattach(p);

  if (ACT_HEXDUMP == action) {
    n += printf_text("Hex dump of section", USE_LT);
    n += printf_text(name, USE_LT | USE_SPACE | USE_SQ | USE_COLON | USE_EOL);
  } else if (ACT_STRDUMP8 == action || ACT_STRDUMP16 == action) {
    n += printf_text("String dump of section", USE_LT);
    n += printf_text(name, USE_LT | USE_SPACE | USE_SQ | USE_COLON | USE_EOL);
  } else if (ACT_DISASSEMBLE == action) {
    n += printf_text("Disassemble of section", USE_LT);
    n += printf_text(name, USE_LT | USE_SPACE | USE_SQ | USE_COLON | USE_EOL);

    ocdisassemble_open(oc, o);
  }

  if (ACT_HEXDUMP == action || ACT_STRDUMP8 == action || ACT_STRDUMP16 == action ||
      ACT_CODEDUMP == action || ACT_DISASSEMBLE == action) {
    if (0 != size) {
      unknown_t p0 = getp(p, offset, size);

      if (ACT_HEXDUMP == action)          n += printf_data(p0, size, vaddr, USE_HEXDUMP);
      else if (ACT_STRDUMP8 == action)    n += printf_data(p0, size, vaddr, USE_STRDUMP8);
      else if (ACT_STRDUMP16 == action)   n += printf_data(p0, size, vaddr, USE_STRDUMP16);
      else if (ACT_CODEDUMP == action)    n += printf_data(p0, size, vaddr, USE_CODEDUMP);
      else if (ACT_DISASSEMBLE == action) n += ocdisassemble_run(oc, s);

      n += printf_eol();

      if (o->action & OPTPROGRAM_HASH) {
        n += printf_sore(p0, size, USE_SHA256 | USE_EOL);
      }

      if (o->action & OPTPROGRAM_ENTROPY) {
        n += printf_sore(p0, size, USE_ENTROPY | USE_EOL);
      }

      if (o->action & OPTPROGRAM_HASH || o->action & OPTPROGRAM_ENTROPY) {
        n += printf_eol();
      }
    } else {
      printf_w("section '%s' has no data to dump!", name);
      n += printf_eol();
    }
  }

  if (ACT_DISASSEMBLE == action) {
    ocdisassemble_close(p);
  }

  occlose(oc);
  return n;
}

