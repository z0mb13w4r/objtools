#include "dump.h"
#include "decode.h"
#include "opcode.h"
#include "pecode.h"
#include "printf.h"
#include "elfcode.h"

int dump_actions0(const pbuffer_t p, const paction_t x, const unknown_t p0, const uint64_t p0size) {
  if (x && p0 && 0 != p0size) {
    switch (x->action) {
    case ACT_ROT5:   rot5(p0, p0size);            break;
    case ACT_NOT8:   not8(p0, p0size);            break;
    case ACT_ROT13:  rot13(p0, p0size);           break;
    case ACT_NOT16:  not16(p0, p0size);           break;
    case ACT_ROT18:  rot18(p0, p0size);           break;
    case ACT_NOT32:  not32(p0, p0size);           break;

    case ACT_ADD8:   add8(p0, x->value, p0size);  break;
    case ACT_ROL8:   rol8(p0, x->value, p0size);  break;
    case ACT_ROR8:   ror8(p0, x->value, p0size);  break;
    case ACT_SHL8:   shl8(p0, x->value, p0size);  break;
    case ACT_SHR8:   shr8(p0, x->value, p0size);  break;
    case ACT_SUB8:   sub8(p0, x->value, p0size);  break;
    case ACT_XOR8:   xor8(p0, x->value, p0size);  break;

    case ACT_ADD16:  add16(p0, x->value, p0size); break;
    case ACT_ROL16:  rol16(p0, x->value, p0size); break;
    case ACT_ROR16:  ror16(p0, x->value, p0size); break;
    case ACT_SHL16:  shl16(p0, x->value, p0size); break;
    case ACT_SHR16:  shr16(p0, x->value, p0size); break;
    case ACT_SUB16:  sub16(p0, x->value, p0size); break;
    case ACT_XOR16:  xor16(p0, x->value, p0size); break;

    case ACT_ADD32:  add32(p0, x->value, p0size); break;
    case ACT_ROL32:  rol32(p0, x->value, p0size); break;
    case ACT_ROR32:  ror32(p0, x->value, p0size); break;
    case ACT_SHL32:  shl32(p0, x->value, p0size); break;
    case ACT_SHR32:  shr32(p0, x->value, p0size); break;
    case ACT_SUB32:  sub32(p0, x->value, p0size); break;
    case ACT_XOR32:  xor32(p0, x->value, p0size); break;

    default:
      break;
    }
  }

  return 0;
}

int dump_actions1(const pbuffer_t p, const poptions_t o, const uint64_t offset, const uint64_t size) {
  paction_t x = o->actions;
  while (x) {
    if (0 != size) {
      dump_actions0(p, x, getp(p, offset, size), size);
    }

    x = x->actions;
  }

  return 0;
}

int dump_actions2(const pbuffer_t p, const poptions_t o, const handle_t s, const char* name, const int action,
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

      if (MODE_ISSET(o->action, OPTPROGRAM_HASH)) {
        n += printf_sore(p0, size, USE_SHA256 | USE_EOL);
      }

      if (MODE_ISSET(o->action, OPTPROGRAM_ENTROPY)) {
        n += printf_sore(p0, size, USE_ENTROPY | USE_EOL);
      }

      if (MODE_ISSET(o->action, OPTPROGRAM_HASH) || MODE_ISSET(o->action, OPTPROGRAM_ENTROPY)) {
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

int dump_summary(const pbuffer_t p, const poptions_t o) {
  const int MAXSIZE = 10;
  int n = 0;
  if (o && issafe(p)) {
    if (MODE_ISSET(o->action, OPTREADELF_FILEHEADER) ||
        MODE_ISLOCKED(o->action, OPTPROGRAM_HASH) ||
        MODE_ISLOCKED(o->action, OPTPROGRAM_ENTROPY) ||
        MODE_ISLOCKED(o->action, OPTPROGRAM_HASH | OPTPROGRAM_ENTROPY)) {

      n += printf_text("FILE", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_text(o->inpname, USE_LT | USE_EOL);
      n += printf_text("SIZE", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(p->size, USE_DEC | USE_NOSPACE | USE_BYTES | USE_EOL);
      n += printf_text("FORMAT", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
      if (isELF32(p))      n += printf_text("ELF32", USE_LT | USE_EOL);
      else if (isELF64(p)) n += printf_text("ELF64", USE_LT | USE_EOL);
      else if (isPE32(p))  n += printf_text("PE32",  USE_LT | USE_EOL);
      else if (isPE64(p))  n += printf_text("PE32+", USE_LT | USE_EOL);
      n += printf_eol();

      if (MODE_ISSET(o->action, OPTPROGRAM_HASH)) {
        n += printf_sore(p->data, p->size, USE_HASHALL | USE_EOL);
      }
      if (MODE_ISSET(o->action, OPTPROGRAM_ENTROPY)) {
        n += printf_sore(p->data, p->size, USE_ENTROPY | USE_EOL);
      }
    }
  }

  return n;
}

