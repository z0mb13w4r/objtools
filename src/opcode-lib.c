#include "opcode.h"
#include "printf.h"
#include "opcode-lib.h"

#define DEFAULT_SKIP_ZEROES            (8)
#define DEFAULT_SKIP_ZEROES_AT_END     (3)

int opcodelib_open(handle_t p, handle_t o) {
  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);
    bfd* bf = oc->items[OPCODE_BFD];
    if (bf) {
      struct disassemble_info* di = oc->items[OPCODE_DISASSEMBLER];
      if (NULL == di) {
        di = mallocx(sizeof(struct disassemble_info));
        oc->items[OPCODE_DISASSEMBLER] = di;
      }
      if (di) {
        /* Construct and configure the disassembler_info class using stdout */
        init_disassemble_info(di, stdout, (fprintf_ftype)fprintf);

        di->application_data = oc;
        di->arch = bfd_get_arch(bf);
        di->mach = bfd_get_mach(bf);
        di->flavour = bfd_get_flavour(bf);
        di->octets_per_byte = bfd_octets_per_byte(bf, NULL);
        di->skip_zeroes = DEFAULT_SKIP_ZEROES;
        di->skip_zeroes_at_end = DEFAULT_SKIP_ZEROES_AT_END;
        di->disassembler_options = NULL;  // -M options, --disassembler-options=options

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
  return 0;
}

int opcodelib_run(handle_t p, handle_t s) {
  if (isopcode(p) && isopsection(s)) {
//    uint64_t soffset = ocget_soffset(p, s);
//    uint64_t eoffset = ocget_eoffset(p, s);

//    uint64_t soffset = ocget_saddress(p);
//    uint64_t eoffset = ocget_saddress(p) + ocget_size(s);

    uint64_t soffset = ocget_vmaddress(s);
    uint64_t eoffset = ocget_vmaddress(s) + ocget_size(s);

    popcode_t oc = CAST(popcode_t, p);
    struct disassemble_info* di = oc->items[OPCODE_DISASSEMBLER];
    if (di) {
      di->buffer_vma    = ocget_vmaddress(s);
      di->buffer_length = ocget_size(s);
      di->section       = ocget(s, MODE_OCSECTION);
      di->buffer        = mallocx(di->buffer_length);

      if (bfd_get_section_contents(oc->items[OPCODE_BFD], di->section, di->buffer, 0, di->buffer_length)) {
        while (soffset < eoffset) {
          printf("%4lx: ", soffset);
          int siz = oc->ocfunc(soffset, di);
          if (siz <= 0) return -1;
          soffset += siz;
          printf_eol();
        }
      }

      free(di->buffer);
      di->buffer = NULL;

      return 0;
    }
  }

  return -1;
}

