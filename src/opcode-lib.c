#include "opcode.h"
#include "printf.h"
#include "bstring.h"
#include "ocdwarf.h"
#include "options.h"
#include "opcode-lib.h"
#include "opcode-printf.h"

#define DEFAULT_SKIP_ZEROES            (8)
#define DEFAULT_SKIP_ZEROES_AT_END     (3)

static int custom_fprintf(void *p, const char * format, ...) {
  popcode_t oc = CAST(popcode_t, p);
  va_list args;
  char o[1024];

  int n = 0;
  if (oc->items[OPCODE_OUTDATA]) {
    pbstring_t ps = CAST(pbstring_t, oc->items[OPCODE_OUTDATA]);
    if (ps) {
      va_start(args, format);
      n = vsnprintf(o, sizeof(o), format, args);
      va_end(args);
      bstrcat(ps, o);
    }
  }

  return n;
}

static void custom_fprintf_address(bfd_vma vma, struct disassemble_info *di) {
  int n = 0;
  if (di && di->application_data) {
    popcode_t oc = CAST(popcode_t, di->application_data);
    if (MODE_ISSET(oc->action, OPTPROGRAM_PREFIX_ADDR)) {
      n += custom_fprintf(oc, "%8.8" PRIx64, vma);
    } else {
      n += custom_fprintf(oc, "%" PRIx64, vma);
    }
  }
}

char* opcodelib_strncat(char* dst, char* src, char* sep, size_t size) {
  if (dst && src && sep && 0 != size) {
    if (dst[0]) {
      dst = strncat(dst, sep, size);
    }
    return strncat(dst, src, size);
  }

  return NULL;
}

int opcodelib_open(handle_t p, handle_t o) {
  if (isopcode(p) && isoptions(o)) {
    poptions_t op = CAST(poptions_t, o);
    popcode_t oc = CAST(popcode_t, p);
    bfd* bf = oc->items[OPCODE_BFD];
    if (bf) {
      struct disassemble_info* di = oc->items[OPCODE_DISASSEMBLER];
      if (NULL == di) {
        di = xmalloc(sizeof(struct disassemble_info));
        oc->items[OPCODE_DISASSEMBLER] = di;
      }

      if (NULL == oc->items[OPCODE_OUTDATA]) {
        oc->items[OPCODE_OUTDATA] = bstrmallocsize(1024);
      }

      if (di) {
        MALLOCA(char, args, 1024);

        /* Construct and configure the disassembler_info class using stdout */
        init_disassemble_info(di, oc, CAST(fprintf_ftype, custom_fprintf));

        di->application_data = oc;
        di->arch = bfd_get_arch(bf);
        di->mach = bfd_get_mach(bf);
        di->flavour = bfd_get_flavour(bf);
        di->octets_per_byte = bfd_octets_per_byte(bf, NULL);
        di->skip_zeroes = DEFAULT_SKIP_ZEROES;
        di->skip_zeroes_at_end = DEFAULT_SKIP_ZEROES_AT_END;
        di->disassembler_options = NULL;
        di->print_address_func = custom_fprintf_address;

        if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_ATT_MNEMONIC)) {
          di->disassembler_options = opcodelib_strncat(args, "att-mnemonic", ",", NELEMENTS(args));
        } else if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_INTEL_MNEMONIC)) {
          di->disassembler_options = opcodelib_strncat(args, "intel-mnemoic", ",", NELEMENTS(args));
        } else if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_ATT)) {
          di->disassembler_options = opcodelib_strncat(args, "att", ",", NELEMENTS(args));
        } else if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_INTEL)) {
          di->disassembler_options = opcodelib_strncat(args, "intel", ",", NELEMENTS(args));
        }

        if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_X86_64)) {
          di->disassembler_options = opcodelib_strncat(args, "x86-64", ",", NELEMENTS(args));
        } else if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_I386)) {
          di->disassembler_options = opcodelib_strncat(args, "i386", ",", NELEMENTS(args));
        } else if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_I8086)) {
          di->disassembler_options = opcodelib_strncat(args, "i8086", ",", NELEMENTS(args));
        }

        if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_AMD64)) {
          di->disassembler_options = opcodelib_strncat(args, "amd64", ",", NELEMENTS(args));
        } else if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_INTEL64)) {
          di->disassembler_options = opcodelib_strncat(args, "intel64", ",", NELEMENTS(args));
        }

        if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_ADDR16)) {
          di->disassembler_options = opcodelib_strncat(args, "addr16", ",", NELEMENTS(args));
        } else if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_ADDR32)) {
          di->disassembler_options = opcodelib_strncat(args, "addr32", ",", NELEMENTS(args));
        } else if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_ADDR64)) {
          di->disassembler_options = opcodelib_strncat(args, "addr64", ",", NELEMENTS(args));
        }

        if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_DATA16)) {
          di->disassembler_options = opcodelib_strncat(args, "data16", ",", NELEMENTS(args));
        } else if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_DATA32)) {
          di->disassembler_options = opcodelib_strncat(args, "data32", ",", NELEMENTS(args));
        }

        if (bfd_big_endian(bf))          di->endian_code = di->display_endian = di->endian = BFD_ENDIAN_BIG;
        else if (bfd_little_endian(bf))  di->endian_code = di->display_endian = di->endian = BFD_ENDIAN_LITTLE;
        else                             di->endian_code = di->endian = BFD_ENDIAN_UNKNOWN;

        /* Construct disassembler_ftype class */
        oc->ocfunc = disassembler(di->arch, bfd_big_endian(bf), di->mach, bf);

        disassemble_init_for_target(di);
        return ECODE_OK;
      }
    }
  }

  return ECODE_HANDLE;
}

int opcodelib_close(handle_t p) {
  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);
    xfree(oc->items[OPCODE_OUTDATA]);
    oc->items[OPCODE_OUTDATA] = NULL;
  }

  return ECODE_OK;
}

int opcodelib_raw(handle_t p, handle_t s, unknown_t data, const size_t size, const uint64_t vaddr) {
  int n0 = 0;
  if (isopcode(p) && isopshdr(s)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    puchar_t p0 = CAST(puchar_t, data);

    uint64_t soffset = vaddr;
    uint64_t eoffset = vaddr + size;

    while (soffset < eoffset) {
      int n1 = 0;
      pbstring_t ps = CAST(pbstring_t, ocget(p, OPCODE_OUTDATA));
      bstrclr(ps);

      struct disassemble_info* di = ocget(p, OPCODE_DISASSEMBLER);
      int siz = oc->ocfunc(soffset, di);
      if (siz <= 0) return n0;

      if (ocuse_vaddr(oc, soffset)) {
        n0 += opcode_printf_lnumbers(p, soffset);

        if (MODE_ISSET(oc->action, OPTPROGRAM_PREFIX_ADDR)) {
          n1 += printf_nice(soffset, USE_LHEX32 | USE_COLON);
        } else {
          n1 += opcode_printf_LHEX(p, soffset, USE_COLON);
          n1 += printf_sore(p0, siz, USE_HEX | USE_SPACE);
          n1 += printf_pack(31 - n1);
        }

        n1 += printf_sore(ps->data, ps->size, USE_STR | USE_SPACE);
        n1 += printf_eol();
      }

      soffset += siz;
      p0 += siz;
      n0 += n1;
    }
  }

  return n0;
}

int opcodelib_run(handle_t p, handle_t s) {
  int n = 0;
  if (isopcode(p) && isopshdr(s)) {
    struct disassemble_info* di = ocget(p, OPCODE_DISASSEMBLER);
    if (di) {
      di->buffer_vma    = ocget_vmaddress(s);
      di->buffer_length = ocget_size(s);
      di->section       = ocget(s, MODE_OCSHDR);
      di->buffer        = xmalloc(di->buffer_length);

      if (bfd_get_section_contents(ocget(p, OPCODE_BFD), di->section, di->buffer, 0, di->buffer_length)) {
        n += opcodelib_raw(p, s, di->buffer, ocget_size(s), ocget_vmaddress(s));
      }

      free(di->buffer);
      di->buffer = NULL;
    }
  }

  return n;
}

