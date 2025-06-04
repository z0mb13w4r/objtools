#include "opcode.h"
#include "printf.h"
#include "bstring.h"
#include "options.h"
#include "opcode-lib.h"

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
        return 0;
      }
    }
  }

  return -1;
}

int opcodelib_close(handle_t p) {
  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);
    xfree(oc->items[OPCODE_OUTDATA]);
    oc->items[OPCODE_OUTDATA] = NULL;
  }

  return 0;
}

static const char  *prev_functionname = NULL;
static unsigned int prev_line = -1;
static unsigned int prev_discriminator = -1;

static int opcodelib_lnumbers(handle_t p, handle_t s, const uint64_t vaddr) {
  int n = 0;
  if (isopcode(p) && isopshdr(s)) {
//    popcode_t oc = CAST(popcode_t, p);

    const char *filename;
//    const char *afilename = NULL;
    const char *functionname;
    unsigned int linenumber;
    unsigned int discriminator;

#define bfd_find_nearest_line_with_alt(abfd, alt_filename, sec, syms, off, \
				       file, func, line, disc) \
       BFD_SEND (abfd, _bfd_find_nearest_line_with_alt, \
		 (abfd, alt_filename, syms, sec, off, file, func, line, disc))

    bfd_set_error(bfd_error_no_error);
//    if (!bfd_find_nearest_line_with_alt(CAST(bfd*, ocget(p, OPCODE_BFD)), afilename, ocget(s, MODE_OCSHDR), ocget(p, OPCODE_RAWSYMBOLS),
//                                        vaddr, &filename, &functionname, &linenumber, &discriminator)) {
//    if (
    bfd_find_nearest_line_discriminator(CAST(bfd*, ocget(p, OPCODE_BFD)), ocget(s, MODE_OCSHDR), ocget(p, OPCODE_RAWSYMBOLS),
                                            vaddr, &filename, &functionname, &linenumber, &discriminator);
//					    ) {
//    }

      if (NULL != filename && 0 == filename[0]) filename = NULL;
      if (NULL != functionname && 0 == functionname[0]) functionname = NULL;

      if (NULL != functionname && (NULL == prev_functionname || 0 != strcmp(functionname, prev_functionname))) {
        printf("%s():\n", functionname);
        prev_functionname = functionname;
        prev_line = -1;
      }
//printf("+++ %d %d\n", linenumber, discriminator);
      if (linenumber > 0 && (linenumber != prev_line || discriminator != prev_discriminator)) {
        printf("%s:%u", filename == NULL ? "???" : filename, linenumber);
        if (discriminator > 0) {
          printf_nice(discriminator, USE_DISCRIMINATOR);
          prev_discriminator = discriminator;
        }
        n += printf_eol();
        prev_line = linenumber;
      }
//    }
  }

  return n;
}

int opcodelib_raw(handle_t p, handle_t s, unknown_t data, const size_t size, const uint64_t vaddr) {
  int n = 0;
  if (isopcode(p) && isopshdr(s)) {
    popcode_t oc = CAST(popcode_t, p);
    puchar_t p0 = CAST(puchar_t, data);

    uint64_t soffset = vaddr;
    uint64_t eoffset = vaddr + size;

    while (soffset < eoffset) {
      int n2 = 0;
      pbstring_t ps = CAST(pbstring_t, ocget(p, OPCODE_OUTDATA));
      bstrclr(ps);

      struct disassemble_info* di = ocget(p, OPCODE_DISASSEMBLER);
      int siz = oc->ocfunc(soffset, di);
      if (siz <= 0) return n;

      if (ocuse_vaddr(oc, soffset)) {
        if (MODE_ISSET(oc->action, OPTPROGRAM_LINE_NUMBERS | OPTPROGRAM_SOURCE_CODE)) {
          n += opcodelib_lnumbers(p, s, soffset);
          n += printf_pack(3);
        }

        if (MODE_ISSET(oc->action, OPTPROGRAM_PREFIX_ADDR)) {
          n2 += printf_nice(soffset, USE_LHEX32 | USE_COLON);
        } else {
          n2 += printf_nice(soffset, USE_HEX4 | USE_COLON);
          n2 += printf_sore(p0, siz, USE_HEX | USE_SPACE);
          n2 += printf_pack(31 - n2);
        }

        n2 += printf_sore(ps->data, ps->size, USE_STR | USE_SPACE | USE_EOL);
      }

      soffset += siz;
      p0 += siz;
      n += n2;
    }
  }

  return n;
}

int opcodelib_run(handle_t p, handle_t s) {
  int n = 0;
  if (isopcode(p) && isopshdr(s)) {
//    popcode_t oc = CAST(popcode_t, p);
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

