#include <string.h>

#include "opcode.h"
#include "pecode.h"
#include "printf.h"
#include "elfcode.h"
#include "options.h"
#include "objutils.h"
#include "opcode-bfd.h"
#include "opcode-elf.h"
#include "opcode-lib.h"
#include "opcode-capstone.h"

static void callback_find_max_sectionhdr_name(bfd *f ATTRIBUTE_UNUSED, asection *s, void *p) {
  /* Ignore linker created section. */
  if (s->flags & SEC_LINKER_CREATED) return;

  size_t *max_name_size = (size_t *) p;
  const char* name = bfd_section_name(s);
  size_t name_size = strlen(name);
  if (name_size > *max_name_size) {
    *max_name_size = name_size;
  }
}

static handle_t create_symbols(bfd *f) {
  if (f && bfd_get_file_flags(f) & HAS_SYMS) {
    long size = bfd_get_symtab_upper_bound(f);
    if (size > 0) {
      pbuffer_t bsy = bmalloc();
      if (bsy) {
        asymbol **asy = (asymbol **) xmalloc(size);
        if (asy) {
          long count = bfd_canonicalize_symtab(f, asy);
          bsy->data = asy;
          bsy->size = count;
        } else {
          bsy = bfree(bsy);
        }
      }
      return bsy;
    }
  }

  return NULL;
}

static handle_t create_symbols_dynamic(bfd *f) {
  if (f && bfd_get_file_flags(f) & DYNAMIC) {
    long size = bfd_get_dynamic_symtab_upper_bound(f);
    if (size > 0) {
      pbuffer_t bsy = bmalloc();
      if (bsy) {
        asymbol **asy = (asymbol **) xmalloc(size);
        if (asy) {
          long count = bfd_canonicalize_dynamic_symtab(f, asy);
          bsy->data = asy;
          bsy->size = count;
        } else {
          bsy = bfree(bsy);
        }
      }
      return bsy;
    }
  }

  return NULL;
}

static bool_t ismodeopwrap(const imode_t mode) {
  return MODE_GET3(mode) == MODE_OPWRAP;
}

bool_t isopcode(handle_t p) {
  return ismode(p, MODE_OPCODE);
}

bool_t isopwrap(handle_t p) {
  return ismode3(p, MODE_PUT3(MODE_OPWRAP));
}

bool_t isopshdr(handle_t p) {
  return ismode(p, MODE_OCSHDR);
}

bool_t isopphdr(handle_t p) {
  return ismode(p, MODE_OCPHDR);
}

bool_t isopehdrNN(handle_t p) {
  return ismode(p, MODE_OCEHDR32) || ismode(p, MODE_OCEHDR64);
}

bool_t isopphdrNN(handle_t p) {
  return ismode(p, MODE_OCPHDR32) || ismode(p, MODE_OCPHDR64);
}

bool_t isopshdrNN(handle_t p) {
  return ismode(p, MODE_OCSHDR32) || ismode(p, MODE_OCSHDR64);
}

bool_t isobject(handle_t p) {
  if (isopcode(p)) {
    popcode_t p0 = CAST(popcode_t, p);
    return p0->items[OPCODE_BFD] && bfd_check_format(p0->items[OPCODE_BFD], bfd_object);
  }

  return FALSE;
}

bool_t isarchive(handle_t p) {
  if (isopcode(p)) {
    popcode_t p0 = CAST(popcode_t, p);
    return p0->items[OPCODE_BFD] && bfd_check_format(p0->items[OPCODE_BFD], bfd_archive);
  }

  return FALSE;
}

bool_t iscoredump(handle_t p) {
  if (isopcode(p)) {
    popcode_t p0 = CAST(popcode_t, p);
    return p0->items[OPCODE_BFD] && bfd_check_format(p0->items[OPCODE_BFD], bfd_core);
  }

  return FALSE;
}

bool_t isattached(handle_t p) {
  if (isopcode(p)) {
    popcode_t p0 = CAST(popcode_t, p);
    return !p0->items[OPCODE_BFD];
  }

  return FALSE;
}

unknown_t ocget(handle_t p, const imode_t mode) {
  if (isopcode(p) && mode < OPCODE_MAXITEMS) {
    popcode_t p0 = CAST(popcode_t, p);
    if (OPCODE_SYMBOLS == mode && NULL == p0->items[mode]) {
      p0->items[mode] = create_symbols(p0->items[OPCODE_BFD]);
    } else if (OPCODE_SYMBOLS_DYNAMIC == mode && NULL == p0->items[mode]) {
      p0->items[mode] = create_symbols_dynamic(p0->items[OPCODE_BFD]);
    }
    return p0->items[mode];
  } else if (isopcode(p) && OPCODE_RAWDATA == mode) {
    popcode_t p0 = CAST(popcode_t, p);
    return p0->data;
  } else if (isopcode(p) && OPCODE_RAWSYMBOLS == mode) {
    pbuffer_t p0 = ocget(p, OPCODE_SYMBOLS);
    return p0 && p0->size && p0->data ? p0->data : NULL;
  } else if (isopcode(p) && OPCODE_RAWSYMBOLS_DYNAMIC == mode) {
    pbuffer_t p0 = ocget(p, OPCODE_SYMBOLS_DYNAMIC);
    return p0 && p0->size && p0->data ? p0->data : NULL;
  } else if (ismode(p, mode) && ismodeopwrap(mode)) {
    popwrap_t p0 = CAST(popwrap_t, p);
    return p0->item;
  } else if (isopwrap(p) && mode == OPCODE_ITEM) {
    popwrap_t p0 = CAST(popwrap_t, p);
    return p0->item;
  } else if (isopwrap(p) && mode == OPCODE_PARAM) {
    popwrap_t p0 = CAST(popwrap_t, p);
    return p0->param;
  }

  return NULL;
}

bool_t ocuse_vaddr(handle_t p, uint64_t vaddr) {
  if (isopcode(p)) {
    popcode_t p0 = CAST(popcode_t, p);
    return (vaddr == OPCODE_NULLADDR) ||
           (p0->saddress == OPCODE_NULLADDR) ||
           (p0->eaddress == OPCODE_NULLADDR) ||
           (p0->saddress <= vaddr && vaddr <= p0->eaddress);
  }

  return FALSE;
}

size_t ocget_maxsectionnamesize(handle_t p) {
  size_t maxsize = 0;
  bfd* p0 = ocget(p, OPCODE_BFD);
  if (p0) {
    bfd_map_over_sections(p0, callback_find_max_sectionhdr_name, &maxsize);
  } else if (ismode(p, MODE_OCSHDR32) ||ismode(p, MODE_OCSHDR64)) {
    return get_secnamemaxsize(ocget(p, OPCODE_PARAM));
  }

  return maxsize;
}

bool_t ochas_phdr(handle_t p) {
  bfd* p0 = ocget(p, OPCODE_BFD);
  if (p0) {
    return bfdget_ehdr(p0)->e_phnum ? TRUE : FALSE;
  } else if (ismode(p, MODE_OCEHDR32)) {
    Elf32_Ehdr* p0 = ocget(p, MODE_OCEHDR32);
    return p0 ? p0->e_phnum : 0;
  } else if (ismode(p, MODE_OCEHDR64)) {
    Elf64_Ehdr* p0 = ocget(p, MODE_OCEHDR64);
    return p0 ? p0->e_phnum : 0;
  }

  return FALSE;
}

bool_t ochas_shdr(handle_t p) {
  bfd* p0 = ocget(p, OPCODE_BFD);
  if (p0) {
      return bfdget_ehdr(p0)->e_shnum ? TRUE : FALSE;
  } else if (ismode(p, MODE_OCEHDR32)) {
    Elf32_Ehdr* p0 = ocget(p, MODE_OCEHDR32);
    return p0 ? p0->e_shnum : 0;
  } else if (ismode(p, MODE_OCEHDR64)) {
    Elf64_Ehdr* p0 = ocget(p, MODE_OCEHDR64);
    return p0 ? p0->e_shnum : 0;
  }

  return FALSE;
}

uint64_t ocget_type(handle_t p) {
  if (ismode(p, MODE_OCPHDR)) {
    pbfd_phdr_t p0 = ocget(p, MODE_OCPHDR);
    return p0 ? p0->p_type : 0;
  } else if (ismode(p, MODE_OCSHDR32)) {
    Elf32_Shdr* p0 = ocget(p, MODE_OCSHDR32);
    return p0 ? p0->sh_type : 0;
  } else if (ismode(p, MODE_OCSHDR64)) {
    Elf64_Shdr* p0 = ocget(p, MODE_OCSHDR64);
    return p0 ? p0->sh_type : 0;
  } else if (ismode(p, MODE_OCPHDR32)) {
    Elf32_Phdr* p0 = ocget(p, MODE_OCPHDR32);
    return p0 ? p0->p_type : 0;
  } else if (ismode(p, MODE_OCPHDR64)) {
    Elf64_Phdr* p0 = ocget(p, MODE_OCPHDR64);
    return p0 ? p0->p_type : 0;
  }

  return 0;
}

uint64_t ocget_flags(handle_t p) {
  if (isopcode(p)) {
    bfd* p0 = ocget(p, OPCODE_BFD);
    return p0 ? bfd_get_file_flags(p0) : 0;
  } else if (ismode(p, MODE_OCSHDR)) {
    asection* p0 = ocget(p, MODE_OCSHDR);
    return p0 ? p0->flags : 0;
  } else if (ismode(p, MODE_OCPHDR)) {
    pbfd_phdr_t p0 = ocget(p, MODE_OCPHDR);
    return p0 ? p0->p_flags : 0;
  } else if (ismode(p, MODE_OCSHDR32)) {
    Elf32_Shdr* p0 = ocget(p, MODE_OCSHDR32);
    return p0 ? p0->sh_flags : 0;
  } else if (ismode(p, MODE_OCSHDR64)) {
    Elf64_Shdr* p0 = ocget(p, MODE_OCSHDR64);
    return p0 ? p0->sh_flags : 0;
  } else if (ismode(p, MODE_OCPHDR32)) {
    Elf32_Phdr* p0 = ocget(p, MODE_OCPHDR32);
    return p0 ? p0->p_flags : 0;
  } else if (ismode(p, MODE_OCPHDR64)) {
    Elf64_Phdr* p0 = ocget(p, MODE_OCPHDR64);
    return p0 ? p0->p_flags : 0;
  }

  return 0;
}

uint64_t ocget_size(handle_t p) {
  if (ismode(p, MODE_OCSHDR)) {
    asection* p0 = ocget(p, MODE_OCSHDR);
    return p0 ? bfd_section_size(p0) : 0;
  } else if (ismode(p, MODE_OCPHDR)) {
    pbfd_phdr_t p0 = ocget(p, MODE_OCPHDR);
    return p0 ? p0->p_filesz : 0;
  } else if (ismode(p, MODE_OCSHDR32)) {
    Elf32_Shdr* p0 = ocget(p, MODE_OCSHDR32);
    return p0 ? p0->sh_size : 0;
  } else if (ismode(p, MODE_OCSHDR64)) {
    Elf64_Shdr* p0 = ocget(p, MODE_OCSHDR64);
    return p0 ? p0->sh_size : 0;
  } else if (ismode(p, MODE_OCSHDRPE)) {
    PIMAGE_SECTION_HEADER p0 = ocget(p, MODE_OCSHDRPE);
    return p0 ? p0->SizeOfRawData : 0;
  }

  return 0;
}

uint64_t ocget_memsize(handle_t p) {
  if (ismode(p, MODE_OCPHDR)) {
    pbfd_phdr_t p0 = ocget(p, MODE_OCPHDR);
    return p0 ? p0->p_memsz : 0;
  } else if (ismode(p, MODE_OCPHDR32)) {
    Elf32_Phdr* p0 = ocget(p, MODE_OCPHDR32);
    return p0 ? p0->p_memsz : 0;
  } else if (ismode(p, MODE_OCPHDR64)) {
    Elf64_Phdr* p0 = ocget(p, MODE_OCPHDR64);
    return p0 ? p0->p_memsz : 0;
  }

  return 0;
}

uint64_t ocget_archsize(handle_t p) {
  if (isopcode(p)) {
    bfd* p0 = ocget(p, OPCODE_BFD);
    if (p0) {
      int size = bfd_get_arch_size(p0);
      return -1 == size ? bfd_arch_bits_per_address(p0) : size;
    } else {
      handle_t p1 = ocget(p, OPCODE_RAWDATA);
      if (isELF32(p1) || isPE32(p1))       return 32;
      else if (isELF64(p1) || isPE64(p1))  return 64;
    }
  }

  return 0;
}

uint64_t ocget_position(handle_t p) {
  if (ismode(p, MODE_OCSHDR)) {
    asection* p0 = ocget(p, MODE_OCSHDR);
    return p0 ? p0->filepos : 0;
  }

  return 0;
}

uint64_t ocget_alignment(handle_t p) {
  if (ismode(p, MODE_OCSHDR)) {
    asection* p0 = ocget(p, MODE_OCSHDR);
    return p0 ? bfd_section_alignment(p0) : 0;
  } else if (ismode(p, MODE_OCPHDR)) {
    pbfd_phdr_t p0 = ocget(p, MODE_OCPHDR);
    return p0 ? ulog2(p0->p_align) : 0;
  } else if (ismode(p, MODE_OCSHDR32)) {
    Elf32_Shdr* p0 = ocget(p, MODE_OCSHDR32);
    return p0 ? p0->sh_addralign : 0;
  } else if (ismode(p, MODE_OCSHDR64)) {
    Elf64_Shdr* p0 = ocget(p, MODE_OCSHDR64);
    return p0 ? p0->sh_addralign : 0;
  } else if (ismode(p, MODE_OCPHDR32)) {
    Elf32_Phdr* p0 = ocget(p, MODE_OCPHDR32);
    return p0 ? p0->p_align : 0;
  } else if (ismode(p, MODE_OCPHDR64)) {
    Elf64_Phdr* p0 = ocget(p, MODE_OCPHDR64);
    return p0 ? p0->p_align : 0;
  }

  return 0;
}

uint64_t ocget_offset(handle_t p) {
 if (ismode(p, MODE_OCPHDR)) {
    pbfd_phdr_t p0 = ocget(p, MODE_OCPHDR);
    return p0 ? p0->p_offset : 0;
  } else if (ismode(p, MODE_OCSHDR32)) {
    Elf32_Shdr* p0 = ocget(p, MODE_OCSHDR32);
    return p0 ? p0->sh_offset : 0;
  } else if (ismode(p, MODE_OCSHDR64)) {
    Elf64_Shdr* p0 = ocget(p, MODE_OCSHDR64);
    return p0 ? p0->sh_offset : 0;
  } else if (ismode(p, MODE_OCPHDR32)) {
    Elf32_Phdr* p0 = ocget(p, MODE_OCPHDR32);
    return p0 ? p0->p_offset : 0;
  } else if (ismode(p, MODE_OCPHDR64)) {
    Elf64_Phdr* p0 = ocget(p, MODE_OCPHDR64);
    return p0 ? p0->p_offset : 0;
  } else if (ismode(p, MODE_OCSHDRPE)) {
    PIMAGE_SECTION_HEADER p0 = ocget(p, MODE_OCSHDRPE);
    return p0 ? peconvert2va(p0, p0->VirtualAddress): 0;
  }

  return 0;
}

uint64_t ocget_paddress(handle_t p) {
 if (ismode(p, MODE_OCPHDR)) {
    pbfd_phdr_t p0 = ocget(p, MODE_OCPHDR);
    return p0 ? p0->p_paddr : 0;
  } else if (ismode(p, MODE_OCPHDR32)) {
    Elf32_Phdr* p0 = ocget(p, MODE_OCPHDR32);
    return p0 ? p0->p_paddr : 0;
  } else if (ismode(p, MODE_OCPHDR64)) {
    Elf64_Phdr* p0 = ocget(p, MODE_OCPHDR64);
    return p0 ? p0->p_paddr : 0;
  }

  return 0;
}

uint64_t ocget_saddress(handle_t p) {
  bfd* p0 = ocget(p, OPCODE_BFD);
  if (p0) {
    return bfd_get_start_address(p0);
  }

  return 0;
}

uint64_t ocget_lmaddress(handle_t p) {
  if (ismode(p, MODE_OCSHDR)) {
    return bfd_section_lma(ocget(p, MODE_OCSHDR));
  }

  return 0;
}

uint64_t ocget_vmaddress(handle_t p) {
  if (ismode(p, MODE_OCSHDR)) {
    return bfd_section_vma(ocget(p, MODE_OCSHDR));
  } else if (ismode(p, MODE_OCPHDR)) {
    pbfd_phdr_t p0 = ocget(p, MODE_OCPHDR);
    return p0 ? p0->p_vaddr : 0;
  } else if (ismode(p, MODE_OCSHDR32)) {
    Elf32_Shdr* p0 = ocget(p, MODE_OCSHDR32);
    return p0 ? p0->sh_addr : 0;
  } else if (ismode(p, MODE_OCSHDR64)) {
    Elf64_Shdr* p0 = ocget(p, MODE_OCSHDR64);
    return p0 ? p0->sh_addr : 0;
  } else if (ismode(p, MODE_OCSHDRPE)) {
    PIMAGE_SECTION_HEADER p0 = ocget(p, MODE_OCSHDRPE);
    return p0 ? p0->VirtualAddress : 0;
  }

  return 0;
}

uint64_t ocget_opb(handle_t p, handle_t s) {
  uint64_t opb = 1;

  if (isopcode(p) && isopshdr(s)) {
    opb = bfd_octets_per_byte(ocget(p, OPCODE_BFD), ocget(p, MODE_OCSHDR));
  } else if (isopcode(p) && ismode(s, MODE_OCSHDR32)) {
  } else if (isopcode(p) && ismode(s, MODE_OCSHDR64)) {
  }

  return opb;
}

uint64_t ocget_soffset(handle_t p, handle_t s) {
  uint64_t soffset = 0;

  if (isopcode(p)) {
    popcode_t p0 = CAST(popcode_t, p);

    if (p0->saddress == OPCODE_NULLADDR || p0->saddress < ocget_vmaddress(s)) soffset = 0;
    else soffset = p0->saddress - ocget_vmaddress(s);
  }

  return soffset;
}

uint64_t ocget_eoffset(handle_t p, handle_t s) {
  uint64_t eoffset = 0;

  if (isopcode(p)) {
    popcode_t p0 = CAST(popcode_t, p);

    uint64_t size = ocget_size(s);
    if (0 != size) {
      /* Compute the address range to display. */
      const uint64_t opb = ocget_opb(p, s);

      if (p0->eaddress == OPCODE_NULLADDR) eoffset = size / opb;
      else {
        if (p0->eaddress < ocget_vmaddress(s)) eoffset = 0;
        else eoffset = p0->eaddress - ocget_vmaddress(s);

        if (eoffset > size / opb) eoffset = size / opb;
      }
    }
  }

  return eoffset;
}

const char* ocget_name(handle_t p) {
  bfd* p0 = ocget(p, OPCODE_BFD);
  if (p0) {
    return bfd_get_filename(p0);
  } else if (ismode(p, MODE_OCSHDR)) {
    return bfd_section_name(ocget(p, MODE_OCSHDR));
  } else if (ismode(p, MODE_OCSHDR32)) {
    return get_secname32byshdr(ocget(p, OPCODE_PARAM), ocget(p, MODE_OCSHDR32));
  } else if (ismode(p, MODE_OCSHDR64)) {
    return get_secname64byshdr(ocget(p, OPCODE_PARAM), ocget(p, MODE_OCSHDR64));
  }

  return NULL;
}

const char* ocget_fileformat(handle_t p) {
  bfd* p0 = ocget(p, OPCODE_BFD);
  if (p0 && p0->xvec) {
    return p0->xvec->name;
  }

  return NULL;
}

handle_t ocmalloc() {
  popcode_t p = xmalloc(sizeof(opcode_t));
  if (p) {
    p->saddress = OPCODE_NULLADDR; /* --start-address */
    p->eaddress = OPCODE_NULLADDR; /* --stop-address */
  }

  return setmode(p, MODE_OPCODE);
}

handle_t ocfree(handle_t p) {
  if (isopcode(p)) {
    popcode_t p0 = CAST(popcode_t, p);
    if (!isattached(p)) {
      bfree(p0->data);
    }
    bfree(p0->items[OPCODE_SYMBOLS]);
    bfree(p0->items[OPCODE_SYMBOLS_DYNAMIC]);
    xfree(p0->items[OPCODE_DISASSEMBLER]);
    xfree(p0->items[OPCODE_OUTDATA]);
    free(p);
    return NULL;
  }

  return p;
}

handle_t ocopen(const char* name) {
  handle_t p0 = bopen(name);
  if (p0) {
    handle_t p1 = ocmalloc();
    if (p1) {
      popcode_t p2 = CAST(popcode_t, p1);
      p2->data = p0;
      p2->items[OPCODE_BFD] = bfd_openr(name, NULL);
      return p1;
    } else {
      bfree(p0);
    }
  }

  return NULL;
}

handle_t ocattach(handle_t p) {
  if (isbuffer(p)) {
    handle_t p1 = ocmalloc();
    if (p1) {
      popcode_t p2 = CAST(popcode_t, p1);
      p2->data = p;
      return p1;
    }
  }

  return NULL;
}

int occlose(handle_t p) {
  if (isopcode(p)) {
    popcode_t p0 = CAST(popcode_t, p);
    if (p0->items[OPCODE_BFD]) {
      bfd_close(p0->items[OPCODE_BFD]);
    }
    ocfree(p);
    return 0;
  }

  return -1;
}

void occonfig(const char* name, const char* target) {
  static int isconfigured = 0;

  if (0 == isconfigured) {
    bfd_set_error_program_name(name);

    if (BFD_INIT_MAGIC != bfd_init()) {
      printf_x("libbfd ABI mismatch.");
    }
    isconfigured = -1;
  }

  if (!bfd_set_default_target(target)) {
    printf_x("can't set BFD default target to '%s': %s", target, bfd_errmsg(bfd_get_error()));
  }
}

int ocdo_programs(handle_t p, opcbfunc_t cbfunc, unknown_t param) {
  if (isopcode(p) && cbfunc) {
    if (ocget(p, OPCODE_BFD)) {
      return opcodebfd_programs(p, cbfunc, param);
    } else {
      handle_t p0 = ocget(p, OPCODE_RAWDATA);
      if (isELF(p0))  return opcodeelf_programs(p, cbfunc, param);
    }
  }

  return -1;
}

int ocdo_sections(handle_t p, opcbfunc_t cbfunc, unknown_t param) {
  if (isopcode(p) && cbfunc) {
    if (ocget(p, OPCODE_BFD)) {
      return opcodebfd_sections(p, cbfunc, param);
    } else {
      handle_t p0 = ocget(p, OPCODE_RAWDATA);
      if (isELF(p0))  return opcodeelf_sections(p, cbfunc, param);
    }
  }

  return -1;
}

int ocdisassemble_open(handle_t p, handle_t o) {
  if (isopcode(p) && isoptions(o)) {
    popcode_t oc = CAST(popcode_t, p);
    poptions_t op = CAST(poptions_t, o);

    oc->action = op->action;
    oc->ocdump = op->ocdump;
    oc->saddress = op->saddress;
    oc->eaddress = op->eaddress;

    if (oc->action & OPTPROGRAM_CAPSTONE || isattached(p)) {
      return capstone_open(p, o);
    } else {
      return opcodelib_open(p, o);
    }
  }

  return -1;
}

int ocdisassemble_close(handle_t p) {
  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);
    if (oc->action & OPTPROGRAM_CAPSTONE || isattached(p)) {
      return capstone_close(p);
    } else {
      return opcodelib_close(p);
    }
  }

  return -1;
}

int ocdisassemble_run(handle_t p, handle_t s) {
  if (isopcode(p) && ismodeNXXN(s, MODE_OCSHDRWRAP)) {
    popcode_t oc = CAST(popcode_t, p);
    if (oc->action & OPTPROGRAM_CAPSTONE || isattached(p)) {
      return capstone_run(p, s);
    } else {
      return opcodelib_run(p, s);
    }
  }

  return -1;
}

int ocdisassemble_raw(handle_t p, handle_t s, unknown_t data, const size_t size, const uint64_t vaddr) {
  if (isopcode(p) && ismodeNXXN(s, MODE_OCSHDRWRAP)) {
    popcode_t oc = CAST(popcode_t, p);
    if (oc->action & OPTPROGRAM_CAPSTONE || isattached(p)) {
      return capstone_raw(p, s, data, size, vaddr);
    } else {
      return opcodelib_raw(p, s, data, size, vaddr);
    }
  }

  return -1;
}

