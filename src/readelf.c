#include "dump.h"
#include "decode.h"
#include "opcode.h"
#include "printf.h"
#include "elfcode.h"
#include "memfind.h"
#include "ocdwarf.h"
#include "readelf.h"
#include "objutils.h"
#include "opcode-demangle.h"

#include "static/dt_flags.ci"
#include "static/dt_flags_1.ci"
#include "static/dt_posflag_1.ci"
#include "static/dyntag.ci"
#include "static/ehdrflags.ci"
#include "static/ehdrmachine.ci"
#include "static/ehdrosabi.ci"
#include "static/ehdrtype.ci"
#include "static/gnuabitab.ci"
#include "static/gnuproperty.ci"
#include "static/nhdrtype.ci"
#include "static/phdrtype.ci"
#include "static/public_tag.ci"
#include "static/reltype.ci"
#include "static/shdrflags.ci"
#include "static/shdrtype.ci"
#include "static/shnindex.ci"
#include "static/stbbind.ci"
#include "static/stttype.ci"
#include "static/stvvisibility.ci"
#include "static/vna_flags.ci"

static int dump_relocsdef0(const pbuffer_t p, const uint64_t sh_link,
                          const uint64_t st_value, const uint64_t st_name, const uint64_t st_shndx) {
  const char* symname = ecget_namebyoffset(p, sh_link, st_name);
  const char* secname = ecget_secnamebyindex(p, st_shndx);

  int n = 0;
  n += printf_nice(st_value, isELF32(p) ? USE_LHEX32: USE_LHEX64);
  if (symname && symname[0])         n += printf_text(symname, USE_LT | USE_SPACE);
  else if (secname && secname[0])    n += printf_text(secname, USE_LT | USE_SPACE);

  return n;
}

static int dump_relocsdef32(const pbuffer_t p, const poptions_t o, Elf32_Shdr *shdr, const uint64_t r_info) {
  if (shdr) {
    MEMSTACK(Elf32_Shdr, dx);
    Elf32_Shdr *d0 = ecget_shdr32byindex(p, dx, shdr->sh_link);
    if (d0) {
      Elf32_Off k = ELF32_R_SYM(r_info);
      Elf32_Sym *sym = getp(p, d0->sh_offset + (k * d0->sh_entsize), d0->sh_entsize);
      if (sym) {
        return dump_relocsdef0(p, d0->sh_link, sym->st_value, sym->st_name, sym->st_shndx);
      }
    }
  }

  return 0;
}

static int dump_relocsdef64(const pbuffer_t p, const poptions_t o, Elf64_Shdr *shdr, const uint64_t r_info) {
  if (shdr) {
    MEMSTACK(Elf64_Shdr, dx);
    Elf64_Shdr *d0 = ecget_shdr64byindex(p, dx, shdr->sh_link);
    if (d0) {
      Elf64_Off k = ELF64_R_SYM(r_info);
      Elf64_Sym *sym = getp(p, d0->sh_offset + (k * d0->sh_entsize), d0->sh_entsize);
      if (sym) {
        return dump_relocsdef0(p, d0->sh_link, sym->st_value, sym->st_name, sym->st_shndx);
      }
    }
  }

  return 0;
}

static int dump_relocsver32(const pbuffer_t p, const poptions_t o, Elf32_Shdr *shdr, const uint64_t r_info, pversion_t vnames, const size_t maxvnames) {
  int n = 0;
  if (shdr) {
    MEMSTACK(Elf32_Shdr, dx);
    Elf32_Shdr *d0 = ecget_shdr32byindex(p, dx, shdr->sh_link);
    if (d0) {
      Elf32_Off k = ELF32_R_SYM(r_info);
      Elf32_Sym *sym = getp(p, d0->sh_offset + (k * d0->sh_entsize), d0->sh_entsize);
      if (sym) {
        n += printf_nice(sym->st_value, USE_LHEX32);
        n += printf_text(opcode_demangle(o, ecget_namebyoffset(p, d0->sh_link, sym->st_name)), USE_LT | USE_SPACE);

        MEMSTACK(Elf32_Shdr, vx);
        Elf32_Shdr *v0 = ecget_shdr32bytype(p, vx, SHT_GNU_versym);
        if (v0) {
          Elf32_Versym *vs = getp(p, v0->sh_offset + (k * v0->sh_entsize), v0->sh_entsize);
          if (vs) {
            *vs = *vs & VERSYM_VERSION;
            if (*vs && *vs < maxvnames) {
              const char* namevs = ecget_namebyoffset(p, vnames[0], vnames[*vs]);
              if (namevs) {
                n += printf_text(namevs, USE_LT | USE_AT);
              }
            }
          }
        }
      }
    }
  }

  return n;
}

static int dump_relocsver64(const pbuffer_t p, const poptions_t o, Elf64_Shdr *shdr, const uint64_t r_info, pversion_t vnames, const size_t maxvnames) {
  int n = 0;
  if (shdr) {
    MEMSTACK(Elf64_Shdr, dx);
    Elf64_Shdr *d0 = ecget_shdr64byindex(p, dx, shdr->sh_link);
    if (d0) {
      Elf64_Off k = ELF64_R_SYM(r_info);
      Elf64_Sym *sym = getp(p, d0->sh_offset + (k * d0->sh_entsize), d0->sh_entsize);
      if (sym) {
        n += printf_nice(sym->st_value, USE_LHEX64);
        n += printf_text(opcode_demangle(o, ecget_namebyoffset(p, d0->sh_link, sym->st_name)), USE_LT | USE_SPACE);

        MEMSTACK(Elf64_Shdr, vx);
        Elf64_Shdr *v0 = ecget_shdr64bytype(p, vx, SHT_GNU_versym);
        if (v0) {
          Elf64_Versym *vs = getp(p, v0->sh_offset + (k * v0->sh_entsize), v0->sh_entsize);
          if (vs) {
            *vs = *vs & VERSYM_VERSION;
            if (*vs && *vs < maxvnames) {
              const char* namevs = ecget_namebyoffset(p, vnames[0], vnames[*vs]);
              if (namevs) {
                n += printf_text(namevs, USE_LT | USE_AT);
              }
            }
          }
        }
      }
    }
  }

  return n;
}

static int dump_elfheader(const pbuffer_t p, const poptions_t o) {
  const int MAXSIZE = 36;

  int n = 0;
  n += printf_text("ELF HEADER", USE_LT | USE_COLON | USE_EOL);
  n += printf_text("Magic", USE_TAB | USE_COLON | SET_PAD(MAXSIZE - 1));
  n += printf_sore(getp(p, 0, 16), 16, USE_HEX | USE_SPACE | USE_EOL);

  n += printf_text("Class", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  if (isELF32(p)) {
    n += printf_text("ELF32", USE_LT | USE_SPACE | USE_EOL);
  } else if (isELF64(p)) {
    n += printf_text("ELF64", USE_LT | USE_SPACE | USE_EOL);
  } else {
    return -1;
  }

  n += printf_text("Data", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  if (isELFle(p)) {
    n += printf_text("2's complement, little endian", USE_LT | USE_SPACE | USE_EOL);
  } else if (isELFbe(p)) {
    n += printf_text("2's complement, big endian", USE_LT | USE_SPACE | USE_EOL);
  } else {
    n += printf_text("none", USE_LT | USE_SPACE | USE_EOL);
  }

  n += printf_text("Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  if (EV_CURRENT == getb(p, EI_VERSION)) {
    n += printf_text("1 (current)", USE_LT | USE_SPACE | USE_EOL);
  } else if (EV_NONE == getb(p, EI_VERSION)) {
    n += printf_text("0", USE_LT | USE_SPACE | USE_EOL);
  } else {
    n += printf_nice(getb(p, EI_VERSION), USE_UNKNOWN | USE_EOL);
  }

  n += printf_text("OS/ABI", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_text(get_EHDROSABI(p), USE_LT | USE_SPACE | USE_EOL);
  n += printf_text("ABI Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(getb(p, EI_ABIVERSION), USE_DEC | USE_EOL);

  return n;
}

static int dump_fileheader0(const pbuffer_t p, const poptions_t o, const uint64_t e_type, const uint64_t e_machine, const uint64_t e_version,
                            const uint64_t e_entry, const uint64_t e_phoff, const uint64_t e_shoff, const uint64_t e_flags,
                            const uint64_t e_ehsize, const uint64_t e_phentsize, const uint64_t e_phnum, const uint64_t e_shentsize,
                            const uint64_t e_shnum, const uint64_t e_shstrndx) {
  const int MAXSIZE = 36;

  int n = 0;
  n += printf_text("Type", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_pick(get_EHDRTYPE(p), e_type, USE_LT | USE_SPACE | USE_EOL);
  n += printf_text("Machine", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_pick(ecEHDRMACHINE, e_machine, USE_LT | USE_SPACE | USE_EOL);
  n += printf_text("Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(e_version, USE_FHEX | USE_EOL);
  n += printf_text("Entry point address", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(e_entry, USE_FHEX16 | USE_EOL);
  n += printf_text("Start of program headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(e_phoff, USE_DEC | USE_BYTES | USE_EOL);
  n += printf_text("Start of section headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(e_shoff, USE_DEC | USE_BYTES | USE_EOL);

  n += printf_text("Flags", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(e_flags, USE_FHEX);
  n += printf_mask(get_EHDRFLAGS(p), e_flags, USE_NONE);
  n += printf_text(get_EHDRFLAGSEX(p, e_flags), USE_EOL);

  n += printf_text("Size of this header", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(e_ehsize, USE_DEC | USE_BYTES | USE_EOL);
  n += printf_text("Size of program headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(e_phentsize, USE_DEC | USE_BYTES | USE_EOL);
  n += printf_text("Number of program headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(e_phnum, USE_DEC | USE_EOL);
  n += printf_text("Size of section headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(e_shentsize, USE_DEC | USE_BYTES | USE_EOL);
  n += printf_text("Number of section headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(e_shnum, USE_DEC | USE_EOL);
  n += printf_text("Section header string table index", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(e_shstrndx, USE_DEC | USE_EOL);
  n += printf_eol();

  if (MODE_ISANY(o->action, OPTPROGRAM_HASH))   n += printf_sore(p->data, p->size, USE_HASHALL | USE_EOL);

  return n;
}

static int dump_fileheader32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr) {
  int n = 0;
  n += dump_fileheader0(p, o, ehdr->e_type, ehdr->e_machine, ehdr->e_version, ehdr->e_entry, ehdr->e_phoff, ehdr->e_shoff, ehdr->e_flags,
                      ehdr->e_ehsize, ehdr->e_phentsize, ehdr->e_phnum, ehdr->e_shentsize, ehdr->e_shnum, ehdr->e_shstrndx);

  return n;
}

static int dump_fileheader64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  int n = 0;
  n += dump_fileheader0(p, o, ehdr->e_type, ehdr->e_machine, ehdr->e_version, ehdr->e_entry, ehdr->e_phoff, ehdr->e_shoff, ehdr->e_flags,
                      ehdr->e_ehsize, ehdr->e_phentsize, ehdr->e_phnum, ehdr->e_shentsize, ehdr->e_shnum, ehdr->e_shstrndx);

  return n;
}

static int dump_sectionheaders0(const pbuffer_t p, const poptions_t o, const int maxsize) {
  int n = 0;
  n += printf_text("SECTION HEADERS", USE_LT | USE_COLON | USE_EOL);
  if (MODE_ISSET(o->action, OPTREADELF_SECTIONDETAILS)) {
    n += printf_text("[Nr] Name", USE_LT | USE_TAB);
    n += printf_packeol(7);
    if (isELF64(p)) {
      n += printf_text("Type            Address          Offset            Link", USE_LT);
      n += printf_packeol(7);
      n += printf_text("Size            EntSize          Info              Align", USE_LT);
      n += printf_packeol(7);
      n += printf_text("Flags", USE_LT);
    } else {
      n += printf_text("Type            Addr     Off    Size   ES  Lk Inf Al", USE_LT);
      n += printf_packeol(7);
      n += printf_text("Flags", USE_LT);
    }
    n += printf_eol();
  } else {
    n += printf_text("[ Nr]", USE_LT | USE_TAB);
    n += printf_text("Name", USE_LT | USE_SPACE | SET_PAD(maxsize));
    n += printf_text("Type", USE_LT | USE_SPACE | SET_PAD(17));
    n += printf_text("Address", USE_LT | USE_SPACE | SET_PAD(isELF64(p) ? 17 : 9));
    n += printf_text("Off      Size     ES Flg  Lk Inf  Al", USE_LT | USE_SPACE);
    if (MODE_ISANY(o->action, OPTPROGRAM_HASH)) {
      n += printf_text("SHA-256", USE_LT | USE_SPACE);
    }
    n += printf_eol();
  }

  return n;
}

static int dump_sectionheaders1(const pbuffer_t p, const poptions_t o, const int index, const int maxsize, const uint64_t sh_type,
                                const uint64_t sh_addr, const uint64_t sh_offset, const uint64_t sh_size, const uint64_t sh_entsize,
                                const uint64_t sh_flags, const uint64_t sh_link, const uint64_t sh_info, const uint64_t sh_addralign) {
  int n = 0;

  const imode_t USE_LHEXNN = isELF64(p) ? USE_LHEX64 : USE_LHEX32;
  const imode_t USE_LHEXAA = isELF64(p) ? USE_LHEX64 : USE_LHEX24;
  const imode_t USE_LHEXBB = isELF64(p) ? USE_LHEX64 : USE_LHEX8;
  const imode_t USE_DECNN  = isELF64(p) ? USE_DEC16 : USE_DEC3;

  n += printf_nice(index, USE_DEC3 | USE_TAB | USE_SB);
  if (MODE_ISSET(o->action, OPTREADELF_SECTIONDETAILS)) {
    n += printf_text(ecget_secnamebyindex(p, index), USE_LT | USE_SPACE);
    n += printf_packeol(5);
  } else {
    n += printf_text(ecget_secnamebyindex(p, index), USE_LT | USE_SPACE | SET_PAD(maxsize));
  }

  n += printf_pick(ecSHDRTYPE, sh_type, USE_LT | USE_SPACE | SET_PAD(17));
  n += printf_nice(sh_addr, USE_LHEXNN);

  if (MODE_ISSET(o->action, OPTREADELF_SECTIONDETAILS)) {
    n += printf_nice(sh_offset, USE_LHEXAA);
    if (isELF64(p)) {
      n += printf_nice(sh_link, USE_DEC3);
      n += printf_packeol(5);
    }
    n += printf_nice(sh_size, USE_LHEXAA);
    n += printf_nice(sh_entsize, USE_LHEXBB);
    if (isELF32(p)) {
      n += printf_nice(sh_link, USE_DEC3);
    }
    n += printf_nice(sh_info, USE_DEC2);
    n += printf_nice(sh_addralign, USE_DECNN);
    n += printf_packeol(5);
    n += printf_nice(sh_flags, USE_LHEXNN | USE_SB | USE_COLON);
    n += printf_mask(ecSHDRFLAGS, sh_flags, USE_NONE);
  } else {
    n += printf_nice(sh_offset, USE_LHEX32);
    n += printf_nice(sh_size, USE_LHEX32);
    n += printf_nice(sh_entsize, USE_LHEX8);
    n += printf_mask(ecSHDRFLAGSLITE, sh_flags, USE_RT | USE_NOSPACE | SET_PAD(4));
    n += printf_nice(sh_link, USE_DEC3);
    n += printf_nice(sh_info, USE_DEC3);
    n += printf_nice(sh_addralign, USE_DEC3);
  }

  return n;
}

static int dump_sectionheaders2(const pbuffer_t p, const poptions_t o) {
  int n = 0;
  if (MODE_ISNOT(o->action, OPTREADELF_SECTIONDETAILS)) {
    n += printf_text("Key to Flags", USE_LT | USE_COLON | USE_EOL);
    n += printf_text("W (write), A (alloc), X (execute), M (merge), S (strings), I (info),", USE_LT | USE_TAB | USE_EOL);
    n += printf_text("L (link order), O (extra OS processing required), G (group), T (TLS),", USE_LT | USE_TAB | USE_EOL);
    n += printf_text("C (compressed), x (unknown), o (OS specific), E (exclude),", USE_LT | USE_TAB | USE_EOL);
    n += printf_text("l (large), p (processor specific)", USE_LT | USE_TAB | USE_EOL);
  }

  return n;
}

static int dump_sectionheaders32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr) {
  const int MAXSIZE = MAX(ecget_secnamemaxsize(p) + 1, 21);

  int n = 0;
  n += dump_sectionheaders0(p, o, MAXSIZE);

  for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf32_Shdr, sx);
    Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, i);
    if (s0) {
      n += dump_sectionheaders1(p, o, i, MAXSIZE, s0->sh_type,
               s0->sh_addr, s0->sh_offset, s0->sh_size, s0->sh_entsize,
               s0->sh_flags, s0->sh_link, s0->sh_info, s0->sh_addralign);

      if (MODE_ISANY(o->action, OPTPROGRAM_HASH)) {
        n += printf_sore(getp(p, s0->sh_offset, s0->sh_size), s0->sh_size, USE_SHA256 | USE_SPACE | USE_NOTEXT);
      }

      n += printf_eol();
    }
  }

  n += dump_sectionheaders2(p, o);

  return n;
}

static int dump_sectionheaders64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  const int MAXSIZE = MAX(ecget_secnamemaxsize(p) + 2, 21);

  int n = 0;
  n += dump_sectionheaders0(p, o, MAXSIZE);

  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf64_Shdr, sx);
    Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, i);
    if (s0) {
      n += dump_sectionheaders1(p, o, i, MAXSIZE, s0->sh_type,
               s0->sh_addr, s0->sh_offset, s0->sh_size, s0->sh_entsize,
               s0->sh_flags, s0->sh_link, s0->sh_info, s0->sh_addralign);

      if (MODE_ISANY(o->action, OPTPROGRAM_HASH)) {
        n += printf_sore(getp(p, s0->sh_offset, s0->sh_size), s0->sh_size, USE_SHA256 | USE_SPACE | USE_NOTEXT);
      }

      n += printf_eol();
    }
  }

  n += dump_sectionheaders2(p, o);

  return n;
}

static int dump_sectiongroups0(const pbuffer_t p, const poptions_t o, const int index, const int maxsize, const uint64_t flags, const uint64_t sh_info) {
  int n = 0;

  if (GRP_COMDAT == flags) {
    n += printf_text("COMDAT", USE_LT);
  }

  n += printf_text("group section", USE_LT | USE_SPACE);
  n += printf_nice(index, USE_DEC3 | USE_SB);
  n += printf_text(ecget_secnamebyindex(p, index), USE_LT | USE_SPACE | USE_SQ);
  n += printf_text(ecget_symnamebyindex(p, sh_info), USE_LT | USE_SPACE | USE_SB);
  n += printf_text("contains", USE_LT | USE_SPACE);
  n += printf_nice(maxsize, USE_DEC);
  n += printf_text("sections", USE_LT | USE_SPACE | USE_COLON);
  n += printf_eol();

  n += printf_text("Index", USE_LT | USE_SPACE | USE_SB);
  n += printf_text("Name", USE_LT | USE_SPACE);
  n += printf_eol();

  return n;
}

static int dump_sectiongroups1(const pbuffer_t p, const poptions_t o, const int index) {
  int n = 0;
  n += printf_nice(index, USE_DEC5 | USE_SB);
  n += printf_text(ecget_secnamebyindex(p, index), USE_LT | USE_SPACE);
  n += printf_eol();

  return n;
}

static int dump_sectiongroups32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr) {
  /* Scan the sections for the group section. */
  int n = 0;
  Elf32_Half cnt = 0;
  for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf32_Shdr, sx);
    Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, i);
    if (s0 && SHT_GROUP == s0->sh_type) ++cnt;
  }

  if (0 == cnt) {
    printf_w("There are no section groups in this file.");
  } else {
    for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
      MEMSTACK(Elf32_Shdr, sx);
      Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, i);
      if (s0 && SHT_GROUP == s0->sh_type) {
        uint32_t *pb = _get32byshdr(p, s0);
        if (pb) {
          size_t size = (s0->sh_size / s0->sh_entsize) - 1;
          n += dump_sectiongroups0(p, o, i, size, pb[0], s0->sh_info);

          for (size_t k = 0; k < size; ++k) {
            n += dump_sectiongroups1(p, o, pb[k + 1]);
          }
        }
      }
    }
  }

  return n;
}

static int dump_sectiongroups64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  /* Scan the sections for the group section. */
  int n = 0;
  Elf64_Half cnt = 0;
  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf64_Shdr, sx);
    Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, i);
    if (s0 && SHT_GROUP == s0->sh_type) ++cnt;
  }

  if (0 == cnt) {
    printf_w("There are no section groups in this file.");
  } else {
    for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
      MEMSTACK(Elf64_Shdr, sx);
      Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, i);
      if (s0 && SHT_GROUP == s0->sh_type) {
        uint32_t *pb = _get64byshdr(p, s0);
        if (pb) {
          size_t size = (s0->sh_size / s0->sh_entsize) - 1;
          n += dump_sectiongroups0(p, o, i, size, pb[0], s0->sh_info);

          for (size_t k = 0; k < size; ++k) {
            n += dump_sectiongroups1(p, o, pb[k + 1]);
          }
        }
      }
    }
  }

  return n;
}

static int dump_programheaders0(const uint64_t e_phnum) {
  int n = 0;
  if (0 != e_phnum) {
    n += printf_text("PROGRAM HEADERS", USE_LT | USE_COLON | USE_EOL);
    n += printf_text("Type            Offset   VirtAddr           PhysAddr           FileSiz  MemSiz   Flg Align", USE_LT | USE_TAB | USE_EOL);
  }

  return n;
}

static int dump_programheaders1(const uint64_t e_phnum) {
  int n = 0;
  if (0 != e_phnum) {
    n += printf_eol();
    n += printf_text("Section to Segment mapping", USE_LT | USE_COLON | USE_EOL);
    n += printf_text("Segment Sections...", USE_LT | USE_SPACE | USE_EOL);
  }

  return n;
}

static int dump_programheaders2(const uint64_t e_phnum) {
  int n = 0;
  if (0 == e_phnum) {
    printf_w("There are no program headers in this file.");
  } else {
    n += printf_eol();
  }

  return n;
}

static int dump_programheaders3(const pbuffer_t p, const uint64_t p_type, const uint64_t p_offset,
                               const uint64_t p_vaddr, const uint64_t p_paddr, const uint64_t p_filesz, const uint64_t p_memsz,
                               const uint64_t p_flags, const uint64_t p_align) {
  int n = 0;
  n += printf_pick(ecPHDRTYPE, p_type, USE_LT | USE_TAB | SET_PAD(17));
  n += printf_nice(p_offset, USE_FHEX24);
  n += printf_nice(p_vaddr,  USE_FHEX64);
  n += printf_nice(p_paddr,  USE_FHEX64);
  n += printf_nice(p_filesz, USE_FHEX24);
  n += printf_nice(p_memsz,  USE_FHEX24);
  n += printf_nice(p_flags & PF_R ? 'R' : ' ', USE_CHAR | USE_SPACE);
  n += printf_nice(p_flags & PF_W ? 'W' : ' ', USE_CHAR);
  n += printf_nice(p_flags & PF_X ? 'E' : ' ', USE_CHAR);
  n += printf_nice(p_align, USE_FHEX | USE_EOL);

  if (PT_INTERP == p_type) {
    n += printf("    [Requesting program interpreter:" );
    n += printf_sore(getp(p, p_offset, p_filesz), p_filesz, USE_STR);
    n += printf("]\n");
  }

  return n;
}

static int dump_programheaders32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr) {
  dump_programheaders0(ehdr->e_phnum);

  for (Elf32_Half i = 0; i < ehdr->e_phnum; ++i) {
    MEMSTACK(Elf32_Phdr, px);
    Elf32_Phdr *p0 = ecget_phdr32byindex(p, px, i);
    if (p0) {
      dump_programheaders3(p, p0->p_type, p0->p_offset, p0->p_vaddr, p0->p_paddr, p0->p_filesz, p0->p_memsz, p0->p_flags, p0->p_align);
    }
  }

  dump_programheaders1(ehdr->e_phnum);

  for (Elf32_Half i = 0; i < ehdr->e_phnum; ++i) {
    printf_nice(i, USE_TAB | USE_DEC2Z);

    MEMSTACK(Elf32_Phdr, px);
    Elf32_Phdr *p0 = ecget_phdr32byindex(p, px, i);
    if (p0) {
      for (Elf32_Half j = 1; j < ehdr->e_shnum; ++j) {
        MEMSTACK(Elf32_Shdr, sx);
        Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, j);
        if (s0) {
          if (!isTBSS32(s0, p0) && isshdrinphdr32(s0, p0)) {
            printf_text(ecget_secnamebyindex(p, j), USE_SPACE);
          }
        }
      }
    }

    printf_eol();
  }

  dump_programheaders2(ehdr->e_phnum);

  return 0;
}

static int dump_programheaders64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  dump_programheaders0(ehdr->e_phnum);

  for (Elf64_Half i = 0; i < ehdr->e_phnum; ++i) {
    MEMSTACK(Elf64_Phdr, px);
    Elf64_Phdr *p0 = ecget_phdr64byindex(p, px, i);
    if (p0) {
      dump_programheaders3(p, p0->p_type, p0->p_offset, p0->p_vaddr, p0->p_paddr, p0->p_filesz, p0->p_memsz, p0->p_flags, p0->p_align);
    }
  }

  dump_programheaders1(ehdr->e_phnum);

  for (Elf64_Half i = 0; i < ehdr->e_phnum; ++i) {
    printf_nice(i, USE_TAB | USE_DEC2Z);

    MEMSTACK(Elf64_Phdr, px);
    Elf64_Phdr *p0 = ecget_phdr64byindex(p, px, i);
    if (p0) {
      for (Elf64_Half j = 1; j < ehdr->e_shnum; ++j) {
        MEMSTACK(Elf64_Shdr, sx);
        Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, j);
        if (s0) {
          if (!isTBSS64(s0, p0) && isshdrinphdr64(s0, p0)) {
            printf_text(ecget_secnamebyindex(p, j), USE_SPACE);
          }
        }
      }
    }

    printf_eol();
  }

  dump_programheaders2(ehdr->e_phnum);

  return 0;
}

static int dump_dynamic0(const pbuffer_t p, const uint64_t sh_offset, const uint64_t count) {
  int n = 0;
  n += printf_text("Dynamic section at offset", USE_LT);
  n += printf_nice(sh_offset, isELF64(p) ? USE_LHEX64 : USE_LHEX32);
  n += printf_text("contains", USE_LT | USE_SPACE);
  n += printf_nice(count, USE_DEC);
  n += printf_text(1 == count ? "entry" : "entries", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
  n += printf_text("Tag                Type                Name/Value", USE_LT | USE_SPACE | USE_EOL);
  return n;
}

static int dump_dynamic1(const pbuffer_t p, const poptions_t o, const uint64_t d_tag, const uint64_t d_un_d_val, const uint64_t sh_link) {
  int n = 0;
  n += printf_nice(d_tag, isELF32(p) ? USE_FHEX32 : USE_FHEX64);
  n += printf_pick(ecDYNTAG, d_tag, USE_SPACE | USE_RB | SET_PAD(20));

  if (d_tag == DT_FLAGS_1) {
    n += printf_text("Flags", USE_LT | USE_SPACE | USE_COLON);
    n += printf_masknone(ecDT_FLAGS_1, d_un_d_val, USE_LT);
  } else if (d_tag == DT_POSFLAG_1) {
    n += printf_text("Flags", USE_LT | USE_SPACE | USE_COLON);
    n += printf_masknone(ecDT_POSFLAG_1, d_un_d_val, USE_LT);
  } else if (d_tag == DT_FLAGS) {
    n += printf_text("Flags", USE_LT | USE_SPACE | USE_COLON);
    n += printf_masknone(ecDT_FLAGS,d_un_d_val, USE_LT);
  } else if (d_tag == DT_PLTREL) {
    n += printf_pick(ecDYNTAG, d_un_d_val, USE_SPACE);
  } else if (d_tag == DT_NULL || isused(ecDYNTAGNAME, d_tag)) {
    const char *name = ecget_namebyoffset(p, sh_link, d_un_d_val);
    if (name && name[0]) {
      if (d_tag == DT_NEEDED) {
        n += printf(" Shared library: [%s]", name);
        // TBD
      } else if (d_tag == DT_SONAME)        n += printf(" Library soname: [%s]", name);
        else if (d_tag == DT_RPATH)         n += printf(" Library rpath: [%s]", name);
        else if (d_tag == DT_RUNPATH)       n += printf(" Library runpath: [%s]", name);
        else                                n += printf_nice(d_un_d_val, USE_FHEX);
    } else {
      n += printf_nice(d_un_d_val, USE_FHEX);
    }
  } else if (isused(ecDYNTAGBYTES, d_tag)) {
    n += printf_nice(d_un_d_val, USE_DEC | USE_BYTES);
  } else if (isused(ecDYNTAGDEC, d_tag)) {
    n += printf_nice(d_un_d_val, USE_DEC);
  } else if (isused(ecDYNTAGFHEX, d_tag)) {
    n += printf_nice(d_un_d_val, USE_FHEX);
  } else if (d_tag == DT_GNU_PRELINKED) {
    n += printf_nice(d_un_d_val, USE_TIMEDATE);
  } else if (d_tag >= DT_VERSYM && d_tag <= DT_VERNEEDNUM) {
    n += printf_nice(d_un_d_val, USE_FHEX);
  }

  n += printf_eol();

  return n;
}

static int dump_dynamic32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr) {
  /* Scan the sections for the dynamic section. */
  int n = 0;
  Elf32_Half cnt = 0;
  for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf32_Shdr, sx);
    Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, i);
    if (s0 && SHT_DYNAMIC == s0->sh_type) ++cnt;
  }

  if (0 == cnt) {
    printf_w("There is no dynamic section in this file.");
  } else {
    for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
      MEMSTACK(Elf32_Shdr, sx);
      Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, i);
      if (s0 && SHT_DYNAMIC == s0->sh_type) {
        size_t cnt = s0->sh_size / s0->sh_entsize;
        n += dump_dynamic0(p, s0->sh_offset, cnt);

        for (size_t j = 0; j < cnt; ++j) {
          MEMSTACK(Elf32_Dyn, dx);
          Elf32_Dyn *d0 = ecget_dyn32byindex(p, dx, i, j);
          n += dump_dynamic1(p, o, d0->d_tag, d0->d_un.d_val, s0->sh_link);
        }

        n += printf_eol();
      }
    }
  }

  return n;
}

static int dump_dynamic64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  /* Scan the sections for the dynamic section. */
  int n = 0;
  Elf64_Half cnt = 0;
  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf64_Shdr, sx);
    Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, i);
    if (s0 && SHT_DYNAMIC == s0->sh_type) ++cnt;
  }

  if (0 == cnt) {
    printf_w("There is no dynamic section in this file.");
  } else {
    for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
      MEMSTACK(Elf64_Shdr, sx);
      Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, i);
      if (s0 && SHT_DYNAMIC == s0->sh_type) {
        size_t cnt = s0->sh_size / s0->sh_entsize;
        n += dump_dynamic0(p, s0->sh_offset, cnt);

        Elf64_Dyn *dyn = _get64byshdr(p, s0);
        for (size_t j = 0; j < cnt; ++j, ++dyn) {
          MEMSTACK(Elf64_Dyn, dx);
          Elf64_Dyn *d0 = ecget_dyn64byindex(p, dx, i, j);
          n += dump_dynamic1(p, o, d0->d_tag, d0->d_un.d_val, s0->sh_link);
        }

        n += printf_eol();
      }
    }
  }

  return n;
}

static int dump_relocsrel32(const pbuffer_t p, const poptions_t o, Elf32_Shdr *shdr) {
  int n = 0;
  MALLOCA(version_t, vnames, 1024);
  ecmake_versionnames32(p, vnames, NELEMENTS(vnames));

  const int MAXSIZE = strlenpick(get_RELTYPE(p)) + 2;

  n += printf_text("Offset", USE_LT | USE_SPACE | SET_PAD(10));
  n += printf_text("Info", USE_LT | SET_PAD(9));
  n += printf_text("Type", USE_LT | SET_PAD(MAXSIZE));
  n += printf_text("Sym Val", USE_LT | SET_PAD(9));
  n += printf_text("Sym Name", USE_LT | USE_EOL);

  size_t cnt = shdr->sh_size / shdr->sh_entsize;

  Elf32_Rel *r = _get32byshdr(p, shdr);
  if (r) {
    for (size_t j = 0; j < cnt; ++j, ++r) {
      n += printf_nice(r->r_offset, USE_LHEX32);
      n += printf_nice(r->r_info, USE_LHEX32);
      n += printf_pick(get_RELTYPE(p), ELF32_R_TYPE(r->r_info), USE_LT | USE_SPACE | SET_PAD(MAXSIZE));

      if (isused(get_RELTYPEDEF(p), ELF32_R_TYPE(r->r_info))) {
        n += dump_relocsdef32(p, o, shdr, r->r_info);
      } else if (isused(get_RELTYPEVER(p), ELF32_R_TYPE(r->r_info))) {
        n += dump_relocsver32(p, o, shdr, r->r_info, vnames, NELEMENTS(vnames));
      } else if (!isused(get_RELTYPESAFE(p), ELF32_R_TYPE(r->r_info))) {
        n += printf_nice(ELF32_R_TYPE(r->r_info), USE_UNKNOWN);
      }

      n += printf_eol();
    }
  }

  return n;
}

static int dump_relocsrel64(const pbuffer_t p, const poptions_t o, Elf64_Shdr *shdr) {
  int n = 0;
  const int MAXSIZE = strlenpick(zRELTYPE64) + 2;

  n += printf_text("Offset", USE_LT | USE_SPACE | SET_PAD(14));
  n += printf_text("Info", USE_LT | SET_PAD(13));
  n += printf_text("Type", USE_LT | SET_PAD(MAXSIZE));
  n += printf_text("Symbol's Value", USE_LT | SET_PAD(17));
  n += printf_text("Symbol's Name", USE_LT | USE_EOL);

  size_t cnt = shdr->sh_size / shdr->sh_entsize;

  Elf64_Rel *r = _get64byshdr(p, shdr);
  if (r) {
    for (size_t j = 0; j < cnt; ++j, ++r) {
      n += printf_nice(r->r_offset, USE_LHEX48);
      n += printf_nice(r->r_info, USE_LHEX48);
      n += printf_pick(zRELTYPE64, ELF64_R_TYPE(r->r_info), USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
// TBD
      n += printf_eol();
    }
  }

  return n;
}

static int dump_relocsrela32(const pbuffer_t p, const poptions_t o, Elf32_Shdr *shdr) {
  int n = 0;
  const int MAXSIZE = strlenpick(get_RELTYPE(p)) + 2;

  n += printf_text("Offset", USE_LT | USE_SPACE | SET_PAD(10));
  n += printf_text("Info", USE_LT | SET_PAD(9));
  n += printf_text("Type", USE_LT | SET_PAD(MAXSIZE));
  n += printf_text("Sym Val", USE_LT | SET_PAD(9));
  n += printf_text("Sym Name + Addend", USE_LT | USE_EOL);

  size_t cnt = shdr->sh_size / shdr->sh_entsize;

  Elf32_Rela *r = _get32byshdr(p, shdr);
  if (r) {
    for (size_t j = 0; j < cnt; ++j, ++r) {
      n += printf_nice(r->r_offset, USE_LHEX48);
      n += printf_nice(r->r_info, USE_LHEX48);
      n += printf_pick(get_RELTYPE(p), ELF32_R_TYPE(r->r_info), USE_LT | USE_SPACE | SET_PAD(MAXSIZE));

      n += printf_eol();
    }
  }

  return n;
}

static int dump_relocsrela64(const pbuffer_t p, const poptions_t o, Elf64_Shdr *shdr) {
  int n = 0;
  MALLOCA(version_t, vnames, 1024);
  ecmake_versionnames64(p, vnames, NELEMENTS(vnames));

  const int MAXSIZE = strlenpick(get_RELTYPE(p)) + 2;

  n += printf_text("Offset", USE_LT | USE_SPACE | SET_PAD(14));
  n += printf_text("Info", USE_LT | SET_PAD(13));
  n += printf_text("Type", USE_LT | SET_PAD(MAXSIZE));
  n += printf_text("Symbol's Value", USE_LT | SET_PAD(17));
  n += printf_text("Symbol's Name + Addend", USE_LT | USE_EOL);

  size_t cnt = shdr->sh_size / shdr->sh_entsize;
  Elf64_Rela *r = _get64byshdr(p, shdr);
  if (r) {
    for (size_t j = 0; j < cnt; ++j, ++r) {
      n += printf_nice(r->r_offset, USE_LHEX48);
      n += printf_nice(r->r_info, USE_LHEX48);
      n += printf_pick(get_RELTYPE(p), ELF64_R_TYPE(r->r_info), USE_LT | USE_SPACE | SET_PAD(MAXSIZE));

      if (isused(get_RELTYPEDEF(p), ELF64_R_TYPE(r->r_info))) {
        n += dump_relocsdef64(p, o, shdr, r->r_info);
      } else if (isused(get_RELTYPEVER(p), ELF64_R_TYPE(r->r_info))) {
        n += dump_relocsver64(p, o, shdr, r->r_info, vnames, NELEMENTS(vnames));
      }

      if (isused(get_RELTYPESHEX8(p), ELF64_R_TYPE(r->r_info))) {
        n += printf_nice(r->r_addend, USE_SHEX8);
      } else if (isused(get_RELTYPESHEX16(p), ELF64_R_TYPE(r->r_info))) {
        n += printf_nice(r->r_addend, USE_SHEX16);
      } else if (isused(get_RELTYPESHEX32(p), ELF64_R_TYPE(r->r_info))) {
        n += printf_nice(r->r_addend, USE_SHEX32);
      } else if (isused(get_RELTYPESHEX64(p), ELF64_R_TYPE(r->r_info))) {
        n += printf_nice(r->r_addend, USE_SHEX64);
      } else if (isused(get_RELTYPEPACK(p), ELF64_R_TYPE(r->r_info))) {
        n += printf_pack(17);
        n += printf_nice(r->r_addend, USE_LHEX);
      } else if (!isused(get_RELTYPESAFE(p), ELF64_R_TYPE(r->r_info))) {
        n += printf_nice(r->r_info & 0xffff, USE_UNKNOWN);
      }

      n += printf_eol();
    }
  }

  return n;
}

static int dump_relocsrelr32(const pbuffer_t p, const poptions_t o, Elf32_Shdr *shdr) {
  printf_text("Offset       Info         Type                Symbol's Value   Symbol's Name", USE_LT | USE_SPACE | USE_EOL);

  // TBD
  printf_eol();

  return 0;
}

static int dump_relocsrelr64(const pbuffer_t p, const poptions_t o, Elf64_Shdr *shdr) {
  printf_text("Offset       Info         Type                Symbol's Value   Symbol's Name", USE_LT | USE_SPACE | USE_EOL);

  // TBD
  printf_eol();

  return 0;
}

static int dump_relocs0(const pbuffer_t p, const poptions_t o, const int index,
                     const uint64_t sh_type, const uint64_t sh_offset, const uint64_t sh_size, const uint64_t count) {
  int n = 0;
  const char* name = ecget_secnamebyindex(p, index);
  if (MODE_ISANY(o->action, OPTREADELF_USEDYNAMIC)) {
    if (strstr(name, ".plt")) {
      n += printf_text("PLT", USE_SPACE | USE_SQ);
    } else {
      n += printf_pick(ecSHDRTYPE, sh_type, USE_SPACE | USE_SQ);
    }
    n += printf_text("relocation section at offset", USE_LT | USE_SPACE);
    n += printf_nice(sh_offset, USE_FHEX);
    n += printf_text("contains", USE_LT | USE_SPACE);
    n += printf_nice(sh_size, USE_DEC);
    n += printf_text("bytes", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
  } else {
    n += printf_text("Relocation section", USE_LT);
    n += printf_text(name, USE_LT | USE_SPACE | USE_SQ);
    n += printf_text("at offset", USE_LT | USE_SPACE);
    n += printf_nice(sh_offset, isELF64(p) ? USE_LHEX64 : USE_LHEX32);
    n += printf_text("contains", USE_LT | USE_SPACE);
    n += printf_nice(count, USE_DEC);
    n += printf_text(1 == count ? "entry" : "entries", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
  }

  return n;
}

static int dump_relocs32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr) {
  /* Scan the sections for the relocs section. */
  int n = 0;
  Elf32_Half cnt = 0;
  for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf32_Shdr, sx);
    Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, i);
    if (s0 && (SHT_RELA == s0->sh_type || SHT_REL == s0->sh_type || SHT_RELR == s0->sh_type)) ++cnt;
  }

  if (0 == cnt) {
    printf_w("There are no dynamic relocations in this file.");
  } else {
    for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
      MEMSTACK(Elf32_Shdr, sx);
      Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, i);
      if (s0) {
        if (SHT_RELA == s0->sh_type || SHT_REL == s0->sh_type || SHT_RELR == s0->sh_type) {
          size_t cnt = s0->sh_size / s0->sh_entsize;
          n += dump_relocs0(p, o, i, s0->sh_type, s0->sh_offset, s0->sh_size, cnt);

          if (SHT_REL == s0->sh_type) {
            n += dump_relocsrel32(p, o, s0);
          } else if (SHT_RELA == s0->sh_type) {
            n += dump_relocsrela32(p, o, s0);
          } else if (SHT_RELR == s0->sh_type) {
            n += dump_relocsrelr32(p, o, s0);
          }

          n += printf_eol();
        }
      }
    }
  }

  return n;
}

static int dump_relocs64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  /* Scan the sections for the relocs section. */
  int n = 0;
  Elf64_Half cnt = 0;
  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf64_Shdr, sx);
    Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, i);
    if (s0 && (SHT_RELA == s0->sh_type || SHT_REL == s0->sh_type || SHT_RELR == s0->sh_type)) ++cnt;
  }

  if (0 == cnt) {
    printf_w("There are no dynamic relocations in this file.");
  } else {
    for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
      MEMSTACK(Elf64_Shdr, sx);
      Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, i);
      if (s0) {
        if (SHT_RELA == s0->sh_type || SHT_REL == s0->sh_type || SHT_RELR == s0->sh_type) {
          size_t cnt = s0->sh_size / s0->sh_entsize;
          n += dump_relocs0(p, o, i, s0->sh_type, s0->sh_offset, s0->sh_size, cnt);

          if (SHT_REL == s0->sh_type) {
            n += dump_relocsrel64(p, o, s0);
          } else if (SHT_RELA == s0->sh_type) {
            n += dump_relocsrela64(p, o, s0);
          } else if (SHT_RELR == s0->sh_type) {
            n += dump_relocsrelr64(p, o, s0);
          }

          n += printf_eol();
        }
      }
    }
  }

  return n;
}

static uint64_t prel31_decode(const bool_t yes, const uint64_t value, const uint64_t vaddr) {
  uint64_t offset = value & 0x7fffffff;
  if (offset & 0x40000000) {
    offset |= ~CAST(uint64_t, 0x7fffffff);
  }

  if (yes) {
    offset <<= 1;
  }

  return vaddr + offset;
}

static int dump_unwind0(const pbuffer_t p, const poptions_t o, const int index,
                     const uint64_t sh_type, const uint64_t sh_offset, const uint64_t sh_size, const uint64_t count) {
  int n = 0;
  n += printf_text("Unwind section", USE_LT);
  n += printf_text(ecget_secnamebyindex(p, index), USE_LT | USE_SPACE | USE_SQ);
  n += printf_text("at offset", USE_LT | USE_SPACE);
  n += printf_nice(sh_offset, isELF64(p) ? USE_LHEX64 : USE_LHEX32);
  n += printf_text("contains", USE_LT | USE_SPACE);
  n += printf_nice(count, USE_DEC);
  n += printf_text(1 == count ? "entry" : "entries", USE_LT | USE_SPACE | USE_COLON | USE_EOL);

  return n;
}

static int dump_unwind1(const pbuffer_t p, const poptions_t o, const int index,
                     const uint64_t e_machine, const uint64_t sh_addr, const uint64_t addr0, const uint64_t addr1) {
  int n = 0;
  uint64_t offset = 0;
  uint64_t vaddr = prel31_decode(EM_TI_C6000 == e_machine, addr0, sh_addr + 8 * index);
//printf("%ld|%x|%x|%lx\n", j, p0[0], p0[1], vaddr);
  n += printf_nice(vaddr, USE_FHEX);

  const char * name = ecget_namebyaddr(p, vaddr, &offset);
  if (name && name[0]) {
    if (0 != offset) {
      n += printf_text(name, USE_LT | USE_SPACE | USE_TBLT);
      n += printf_text("+", USE_LT);
      n += printf_nice(offset, USE_FHEX | USE_TBRT | USE_NOSPACE | USE_COLON);
    } else {
      n += printf_text(name, USE_LT | USE_SPACE | USE_TB | USE_COLON);
    }
  }

  if (0x80000000 & addr0) {
    n += printf_nice(addr0, USE_CORRUPT);
  } else {
    if (1 == addr1) {
      n += printf_nice(addr1, USE_FHEX);
      n += printf_text("cantunwind", USE_LT | USE_SPACE | USE_SB);
    } else if (0x80000000 & addr1) {
      printf_nice(addr1, USE_FHEX);
    } else {
    }
  }

  n += printf_eol();

  return n;
}

static int dump_unwind32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr) {
  int n = 0;
  if (EM_ARM == ehdr->e_machine) {
    Elf32_Half cnt = 0;
    for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
      MEMSTACK(Elf32_Shdr, sx);
      Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, i);
      if (s0 && SHT_ARM_EXIDX == s0->sh_type) ++cnt;
    }

    if (0 == cnt) {
      printf_w("There are no unwind sections in this file.");
    } else {
      for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
        MEMSTACK(Elf32_Shdr, sx);
        Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, i);
        if (s0 && SHT_ARM_EXIDX == s0->sh_type) {
          size_t cnt = s0->sh_size / (sizeof(Elf32_Addr) + sizeof(Elf32_Addr));
          n += dump_unwind0(p, o, i, s0->sh_type, s0->sh_offset, s0->sh_size, cnt);

          handle_t f = fget32byshdr(p, s0);
          if (f) {
            for (size_t j = 0; j < cnt; ++j) {
              Elf32_Addr *p0 = fget(f);
              if (p0) {
                n += dump_unwind1(p, o, j, ehdr->e_machine, s0->sh_addr, p0[0], p0[1]);

                f = fstep(f, sizeof(Elf32_Addr) + sizeof(Elf32_Addr));
              }
            }
          }
        }
      }
    }

  } else {
    printf_w("The decoding of unwind sections for machine type %s is not currently supported.", strpick(ecEHDRMACHINE, ehdr->e_machine));
  }

  return n;
}

static int dump_unwind64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  int n = 0;
  if (EM_ARM == ehdr->e_machine) {
    Elf64_Half cnt = 0;
    for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
      MEMSTACK(Elf64_Shdr, sx);
      Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, i);
      if (s0 && SHT_ARM_EXIDX == s0->sh_type) ++cnt;
    }

    if (0 == cnt) {
      printf_w("There are no unwind sections in this file.");
    } else {
      for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
        MEMSTACK(Elf64_Shdr, sx);
        Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, i);
        if (s0 && SHT_ARM_EXIDX == s0->sh_type) {
          size_t cnt = s0->sh_size / (sizeof(Elf64_Addr) + sizeof(Elf64_Addr));
          n += dump_unwind0(p, o, i, s0->sh_type, s0->sh_offset, s0->sh_size, cnt);

          handle_t f = fget64byshdr(p, s0);
          if (f) {
            for (size_t j = 0; j < cnt; ++j) {
              Elf64_Addr *p0 = fget(f);
              if (p0) {
                n += dump_unwind1(p, o, j, ehdr->e_machine, s0->sh_addr, p0[0], p0[1]);

                f = fstep(f, sizeof(Elf64_Addr) + sizeof(Elf64_Addr));
              }
            }
          }
        }
      }
    }

  } else {
    printf_w("The decoding of unwind sections for machine type %s is not currently supported.", strpick(ecEHDRMACHINE, ehdr->e_machine));
  }

  return n;
}

static int dump_symbols0(const pbuffer_t p, const poptions_t o,
                         const uint64_t secindex, const uint64_t count, const uint64_t sh_offset) {
  int n = 0;

  const int MAXSIZE = isELF64(p) ? 18 : 10;

  const imode_t USE_LHEXNN = isELF64(p) ? USE_LHEX64 : USE_LHEX32;

  n += printf_text("Symbol table", USE_LT);
  if (MODE_ISANY(o->action, OPTREADELF_USEDYNAMIC)) {
    n += printf_text("for image", USE_LT | USE_SPACE);
  }

  n += printf_text(ecget_secnamebyindex(p, secindex), USE_LT | USE_SQ | USE_SPACE);
  n += printf_text("at offset", USE_SPACE);
  n += printf_nice(sh_offset, USE_LHEXNN);
  n += printf_text("contains", USE_SPACE);
  n += printf_nice(count, USE_DEC);
  n += printf_text(1 == count ? "entry" : "entries", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
  n += printf_text("   Num", USE_LT | USE_COLON);
  n += printf_text("Value", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
  n += printf_text("Size Type    Bind   Vis      Ndx Name", USE_LT | USE_SPACE | USE_EOL);
  return n;
}

static int dump_symbols1(const pbuffer_t p, const poptions_t o, const uint64_t symindex,
                         const uint64_t st_value, const uint64_t st_size, const uint64_t st_info, const uint64_t st_other, const uint64_t st_shndx) {
  int n = 0;

  const imode_t USE_LHEXNN = isELF64(p) ? USE_LHEX64 : USE_LHEX32;

  n += printf_nice(symindex, USE_DEC5 | USE_COLON);
  n += printf_nice(st_value, USE_LHEXNN);
  n += printf_nice(st_size, USE_DEC5);
  n += printf_pick(ecSTTTYPE, ELF_ST_TYPE(st_info), USE_LT | USE_SPACE | SET_PAD(8));
  n += printf_pick(ecSTBBIND, ELF_ST_BIND(st_info), USE_LT | USE_SPACE | SET_PAD(7));
  n += printf_pick(ecSTVVISIBILITY, ELF_ST_VISIBILITY(st_other), USE_LT | USE_SPACE | SET_PAD(9));
  n += printf_text(get_SHNINDEX(st_shndx), USE_LT | USE_SPACE);
  return n;
}

static int dump_symbols2(const pbuffer_t p, const poptions_t o,
                         const uint64_t sh_link, const uint64_t vna_name, const uint64_t vna_other, const uint64_t st_shndx, const uint64_t st_other) {
  int n = 0;
  const char* namevs = ecget_namebyoffset(p, sh_link, vna_name);
  if (namevs && namevs[0]) {
    if (SHN_UNDEF == st_shndx) {
      n += printf_text(namevs, USE_LT | USE_AT);
      n += printf_nice(vna_other, USE_RB | USE_DEC);
    } else if (STV_HIDDEN == ELF_ST_VISIBILITY(st_other)) {
      n += printf_text(namevs, USE_LT | USE_AT);
    } else {
      n += printf_text(namevs, USE_LT | USE_ATAT);
    }
  }
  return n;
}

static int dump_symbols32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr) {
  int n = 0;
  MALLOCA(version_t, vnames, 1024);
  ecmake_versionnames32(p, vnames, NELEMENTS(vnames));

  /* Scan the sections for the symbols section. */
  Elf32_Half cnt = 0;
  for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf32_Shdr, sx);
    Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, i);
    if (s0) {
      if ((SHT_DYNSYM == s0->sh_type) ||
          (MODE_ISNOT(o->action, OPTREADELF_USEDYNAMIC) && (SHT_SYMTAB == s0->sh_type)))
        ++cnt;
    }
  }

  if (0 == cnt) {
    printf_w("Dynamic symbol information is not available for displaying symbols.");
  } else if (MODE_ISANY(o->action, OPTREADELF_USEDYNAMIC)) {
    // binutils-2.45
    for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
      MEMSTACK(Elf32_Shdr, sx);
      Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, i);
      if (s0 && SHT_DYNSYM == s0->sh_type) {
        size_t cnt = s0->sh_size / s0->sh_entsize;

        n += dump_symbols0(p, o, i, cnt, s0->sh_offset);

        bool_t isok = FALSE; // TBD
        for (size_t j = 0; j < cnt; ++j) {
          MEMSTACK(Elf32_Sym, syx);
          Elf32_Sym* sym = ecget_sym32byindex(p, syx, i, j);
          if (sym) {
            if (SHN_UNDEF != sym->st_shndx) isok = TRUE;

            n += dump_symbols1(p, o, j, sym->st_value, sym->st_size, sym->st_info, sym->st_other, sym->st_shndx);

            const char* name = ecget_namebyoffset(p, s0->sh_link, sym->st_name);
            if (name && 0 != name[0]) {
              n += printf_text(name, USE_LT | USE_SPACE);
            }

            if (!isok) {
              MEMSTACK(Elf32_Shdr, vx);
              Elf32_Shdr *v0 = ecget_shdr32bytype(p, vx, SHT_GNU_versym);
              if (v0) {
                Elf32_Versym *vs = getp(p, v0->sh_offset + (j * v0->sh_entsize), v0->sh_entsize);
                if (vs && *vs && *vs < NELEMENTS(vnames)) {
                  n += dump_symbols2(p, o, vnames[0], vnames[*vs & VERSYM_VERSION], *vs & VERSYM_VERSION, sym->st_shndx, sym->st_other);
                }
              }
            }

            n += printf_eol();
          }
        }

        n += printf_eol();
      }
    }
  } else {
    for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
      MEMSTACK(Elf32_Shdr, sx);
      Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, i);
      if (s0) {
        if (SHT_SYMTAB == s0->sh_type || SHT_DYNSYM == s0->sh_type) {
          size_t cnt = s0->sh_size / s0->sh_entsize;
          n += dump_symbols0(p, o, i, cnt, s0->sh_offset);

          for (size_t j = 0; j < cnt; ++j) {
            MEMSTACK(Elf32_Sym, syx);
            Elf32_Sym* sym = ecget_sym32byindex(p, syx, i, j);
            if (sym) {
              n += dump_symbols1(p, o, j, sym->st_value, sym->st_size, sym->st_info, sym->st_other, sym->st_shndx);

              const char* name = ecget_namebyoffset(p, s0->sh_link, sym->st_name);
              if (name && 0 != name[0]) {
                n += printf_text(opcode_demangle(o, name), USE_LT | USE_SPACE);

                if (SHT_DYNSYM == s0->sh_type) {
                  MEMSTACK(Elf32_Shdr, vx);
                  Elf32_Shdr *v0 = ecget_shdr32bytype(p, vx, SHT_GNU_versym);
                  if (v0) {
                    Elf32_Versym *vs = getp(p, v0->sh_offset + (j * v0->sh_entsize), v0->sh_entsize);
                    if (vs && *vs && *vs < NELEMENTS(vnames)) {
                      n += dump_symbols2(p, o, vnames[0], vnames[*vs & VERSYM_VERSION], *vs & VERSYM_VERSION, sym->st_shndx, sym->st_other);
                    }
                  }
                }
              }

              n += printf_eol();
            }
          }

          n += printf_eol();
        }
      }
    }
  }

  return n;
}

static int dump_symbols64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  int n = 0;
  MALLOCA(version_t, vnames, 1024);
  ecmake_versionnames64(p, vnames, NELEMENTS(vnames));

  /* Scan the sections for the symbols section. */
  Elf64_Half cnt = 0;
  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf64_Shdr, sx);
    Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, i);
    if (s0) {
      if ((SHT_DYNSYM == s0->sh_type) ||
          (MODE_ISNOT(o->action, OPTREADELF_USEDYNAMIC) && (SHT_SYMTAB == s0->sh_type)))
        ++cnt;
    }
  }

  if (0 == cnt) {
    printf_w("Dynamic symbol information is not available for displaying symbols.");
  } else if (MODE_ISANY(o->action, OPTREADELF_USEDYNAMIC)) {
    // binutils-2.45
    for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
      MEMSTACK(Elf64_Shdr, sx);
      Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, i);
      if (s0 && SHT_DYNSYM == s0->sh_type) {
        size_t cnt = s0->sh_size / s0->sh_entsize;

        n += dump_symbols0(p, o, i, cnt, s0->sh_offset);

        bool_t isok = FALSE;
        for (size_t j = 0; j < cnt; ++j) {
          MEMSTACK(Elf64_Sym, syx);
          Elf64_Sym* sym = ecget_sym64byindex(p, syx, i, j);
          if (sym) {
            if (SHN_UNDEF != sym->st_shndx) isok = TRUE;

            n += dump_symbols1(p, o, j, sym->st_value, sym->st_size, sym->st_info, sym->st_other, sym->st_shndx);

            const char* name = ecget_namebyoffset(p, s0->sh_link, sym->st_name);
            if (name && 0 != name[0]) {
              n += printf_text(name, USE_LT | USE_SPACE);
            }

            if (!isok) {
              MEMSTACK(Elf64_Shdr, vx);
              Elf64_Shdr *v0 = ecget_shdr64bytype(p, vx, SHT_GNU_versym);
              if (v0) {
                Elf64_Versym *vs = getp(p, v0->sh_offset + (j * v0->sh_entsize), v0->sh_entsize);
                if (vs && *vs && *vs < NELEMENTS(vnames)) {
                  n += dump_symbols2(p, o, vnames[0], vnames[*vs & VERSYM_VERSION], *vs & VERSYM_VERSION, sym->st_shndx, sym->st_other);
                }
              }
            }

            n += printf_eol();
          }
        }

        n += printf_eol();
      }
    }
  } else {
    for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
      MEMSTACK(Elf64_Shdr, sx);
      Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, i);
      if (s0) {
        if (SHT_SYMTAB == s0->sh_type || SHT_DYNSYM == s0->sh_type) {
          size_t cnt = s0->sh_size / s0->sh_entsize;

          n += dump_symbols0(p, o, i, cnt, s0->sh_offset);

          for (size_t j = 0; j < cnt; ++j) {
            MEMSTACK(Elf64_Sym, syx);
            Elf64_Sym* sym = ecget_sym64byindex(p, syx, i, j);
            if (sym) {
              n += dump_symbols1(p, o, j, sym->st_value, sym->st_size, sym->st_info, sym->st_other, sym->st_shndx);

              const char* name = ecget_namebyoffset(p, s0->sh_link, sym->st_name);
              if (name && 0 != name[0]) {
                n += printf_text(opcode_demangle(o, name), USE_LT | USE_SPACE);

                if (SHT_DYNSYM == s0->sh_type) {
                  MEMSTACK(Elf64_Shdr, vx);
                  Elf64_Shdr *v0 = ecget_shdr64bytype(p, vx, SHT_GNU_versym);
                  if (v0) {
                    Elf64_Versym *vs = getp(p, v0->sh_offset + (j * v0->sh_entsize), v0->sh_entsize);
                    if (vs && *vs && *vs < NELEMENTS(vnames)) {
                      n += dump_symbols2(p, o, vnames[0], vnames[*vs & VERSYM_VERSION], *vs & VERSYM_VERSION, sym->st_shndx, sym->st_other);
                    }
                  }
                }
              }

              n += printf_eol();
            }
          }

          n += printf_eol();
        }
      }
    }
  }

  return n;
}

static int dump_gnuhash0(const pbuffer_t p, uint32_t *pb, const uint64_t sh_name) {
  uint32_t nbucket  = pb[0];
  uint32_t symbias  = pb[1];
  uint32_t sbitmask = isELF32(p) ? pb[2] : 2 * pb[2];
//uint32_t shift    = pb[3];
  uint32_t *bitmask = &pb[4];
  uint32_t *bucket  = &pb[4 + sbitmask];
  uint32_t *chain   = &pb[4 + sbitmask + nbucket];

  int n = 0;
  if (pb[0] > 1 && pb[1]) {
    MALLOCA(uint32_t, size, nbucket);

    /* compute distribution of chain lengths. */
    uint_fast32_t msize = 0;
    uint_fast32_t nsyms = 0;
    for (uint32_t k = 0; k < nbucket; ++k) {
      if (bucket[k] != 0) {
        uint32_t x = bucket[k] - symbias;
        do {
          ++nsyms;
          if (msize < ++size[k]) ++msize;
        } while ((chain[x++] & 1) == 0);
      }
    }

    /* count bits in bitmask. */
    uint_fast32_t nbits = 0;
    for (uint32_t k = 0; k < sbitmask; ++k) {
      uint_fast32_t x = bitmask[k];

      x = (x & 0x55555555) + ((x >> 1) & 0x55555555);
      x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
      x = (x & 0x0f0f0f0f) + ((x >> 4) & 0x0f0f0f0f);
      x = (x & 0x00ff00ff) + ((x >> 8) & 0x00ff00ff);
      nbits += (x & 0x0000ffff) + ((x >> 16) & 0x0000ffff);
    }

    MALLOCA(uint32_t, counts, msize + 1);

    for (uint32_t k = 0; k < nbucket; ++k) {
      ++counts[size[k]];
    }

    n += printf_text("Histogram for", USE_LT);
    n += printf_text(ecget_secnamebyoffset(p, sh_name), USE_LT | USE_DRTB | USE_SPACE);
    n += printf_text("bucket list length (total of", USE_LT | USE_SPACE);
    n += printf_nice(nbucket, USE_DEC);
    n += printf_text(nbucket == 1 ? "bucket)" : "buckets)", USE_LT | USE_SPACE | USE_COLON | USE_EOL);

    n += printf_text(" Length Number       % of total  Coverage", USE_LT | USE_EOL);

    n += printf("     0 ");
    n += printf_nice(counts[0], USE_DEC5);
    n += printf("         ");
    n += printf_nice((counts[0] * 1000.0) / nbucket, USE_PERCENT);
    n += printf_eol();

    uint64_t nzeros = 0;
    for (uint32_t i = 1; i <= msize; ++i) {
      nzeros += counts[i] * i;

      n += printf_nice(i, USE_DEC5);
      n += printf_pack(1);
      n += printf_nice(counts[i], USE_DEC5);
      n += printf_pack(9);
      n += printf_nice((counts[i] * 1000.0) / nbucket, USE_PERCENT);
      n += printf_pack(4);
      n += printf_nice((nzeros * 1000.0) / nsyms, USE_PERCENT);
      n += printf_eol();
    }

    n += printf_eol();
  }

  return n;
}

static int dump_histogram32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr) {
  int n = 0;
  for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf32_Shdr, sx);
    Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, i);
    if (s0 && SHT_GNU_HASH == s0->sh_type) {
      uint32_t *pb = _get32byshdr(p, s0);
      if (pb) {
        n += dump_gnuhash0(p, pb, s0->sh_name);
      }
    } else if (s0 && SHT_HASH == s0->sh_type) {
      // TBD
    }
  }

  return n;
}

static int dump_histogram64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  int n = 0;
  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf64_Shdr, sx);
    Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, i);
    if (s0 && SHT_GNU_HASH == s0->sh_type) {
      uint32_t *pb = _get64byshdr(p, s0);
      if (pb) {
        n += dump_gnuhash0(p, pb, s0->sh_name);
      }
    } else if (s0 && SHT_HASH == s0->sh_type) {
      // TBD
    }
  }

  return n;
}

static int dump_versionsym0(const pbuffer_t p, const uint64_t count, const uint64_t sh_name, const uint64_t sh_addr, const uint64_t sh_offset, const uint64_t sh_link) {
  int n = 0;
  n += printf_text("Version symbols section", USE_LT);
  n += printf_text(ecget_secnamebyoffset(p, sh_name), USE_LT | USE_SQ | USE_SPACE);
  n += printf_text("contains", USE_SPACE);
  n += printf_nice(count, USE_DEC);
  n += printf_text(1 == count ? "entry" : "entries", USE_LT | USE_SPACE | USE_COLON | USE_EOL);

  n += printf_text("Addr", USE_SPACE | USE_COLON);
  n += printf_nice(sh_addr, USE_FHEX64);
  n += printf_text("Offset", USE_SPACE | USE_COLON);
  n += printf_nice(sh_offset, USE_FHEX24);
  n += printf_text("Link", USE_SPACE | USE_COLON);
  n += printf_nice(sh_link, USE_DEC);
  n += printf_text(ecget_secnamebyindex(p, sh_link), USE_LT | USE_RB | USE_SPACE);

  return n;
}

static int dump_versionsym1(const pbuffer_t p, const int index, const version_t vs, pversion_t vnames, const size_t maxvnames) {
  const int MAXSIZE = 20;

  int n0 = 0;
  if (index % 4 == 0) {
    n0 += printf_eol();
    n0 += printf_nice(index, USE_LHEX16 | USE_COLON);
    n0 += printf_pack(1);
  }

  const version_t vsh = vs & VERSYM_HIDDEN;
  const version_t vsx = vs & VERSYM_VERSION;

  int n1 = 0;
  if (0 == vs)         n1 += printf_text("    0  (*local*)", USE_LT | SET_PAD(MAXSIZE));
  else if (1 == vs)    n1 += printf_text("    1  (*global*)", USE_LT | SET_PAD(MAXSIZE));
  else {
    n1 += printf_nice(vsx, USE_HEX4);
    n1 += printf_nice(vsh ? 'h' : ' ', USE_CHAR);
    if (vsx < maxvnames && vnames[vsx]) {
      n1 += printf_text(ecget_namebyoffset(p, vnames[0], vnames[vsx]), USE_LT | USE_RB | USE_SPACE | SET_PAD(MAX(0, MAXSIZE - n1)));
    } else {
      n1 += printf_text("???", USE_SPACE | SET_PAD(MAX(0, MAXSIZE - n1)));
    }
  }

  return n0 + n1;
}

static int dump_versionsym2(const pbuffer_t p, const uint64_t count) {
  int n = 0;
  if (count) n += printf_eol();
  n += printf_eol();

  return n;
}

static int dump_versionsym32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr, Elf32_Shdr *shdr) {
  MALLOCA(version_t, vnames, 1024);
  ecmake_versionnames32(p, vnames, NELEMENTS(vnames));

  int n = 0;
  size_t cnt = shdr->sh_size / shdr->sh_entsize;
  n += dump_versionsym0(p, cnt, shdr->sh_name, shdr->sh_addr, shdr->sh_offset, shdr->sh_link);

  for (size_t j = 0; j < cnt; ++j) {
    Elf64_Versym *vs = getp(p, shdr->sh_offset + (j * shdr->sh_entsize), shdr->sh_entsize);
    if (vs) {
      n += dump_versionsym1(p, j, *vs, vnames, NELEMENTS(vnames));
    }
  }

  n += dump_versionsym2(p, cnt);

  return n;
}

static int dump_versionsym64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr, Elf64_Shdr *shdr) {
  MALLOCA(version_t, vnames, 1024);
  ecmake_versionnames64(p, vnames, NELEMENTS(vnames));

  int n = 0;
  size_t cnt = shdr->sh_size / shdr->sh_entsize;
  n += dump_versionsym0(p, cnt, shdr->sh_name, shdr->sh_addr, shdr->sh_offset, shdr->sh_link);

  for (size_t j = 0; j < cnt; ++j) {
    Elf64_Versym *vs = getp(p, shdr->sh_offset + (j * shdr->sh_entsize), shdr->sh_entsize);
    if (vs) {
      n += dump_versionsym1(p, j, *vs, vnames, NELEMENTS(vnames));
    }
  }

  n += dump_versionsym2(p, cnt);

  return n;
}

static int dump_versionneed0(const pbuffer_t p, const uint64_t sh_name, const uint64_t sh_info,
                             const uint64_t sh_addr, const uint64_t sh_offset, const uint64_t sh_link) {
  int n = 0;
  n += printf_text("Version needs section", USE_LT);
  n += printf_text(ecget_secnamebyoffset(p, sh_name),  USE_LT | USE_SQ | USE_SPACE);
  n += printf_text("contains", USE_SPACE);
  n += printf_nice(sh_info, USE_DEC);
  n += printf_text(1 == sh_info ? "entry" : "entries", USE_LT | USE_SPACE | USE_COLON | USE_EOL);

  n += printf_text("Addr", USE_SPACE | USE_COLON);
  n += printf_nice(sh_addr, USE_FHEX64);
  n += printf_text("Offset", USE_SPACE | USE_COLON);
  n += printf_nice(sh_offset, USE_FHEX24);
  n += printf_text("Link", USE_SPACE | USE_COLON);
  n += printf_nice(sh_link, USE_DEC);
  n += printf_text(ecget_secnamebyindex(p, sh_link), USE_LT | USE_RB | USE_SPACE | USE_EOL);

  return n;
}

static int dump_versionneed1(const pbuffer_t p, const int offset, const uint64_t sh_link,
                             const uint64_t vn_version, const uint64_t vn_file, const uint64_t vn_cnt) {
  int n = 0;
  n += printf_nice(offset, USE_FHEX16 | USE_COLON);
  n += printf_text("Version", USE_SPACE | USE_COLON);
  n += printf_nice(vn_version, USE_DEC);
  n += printf_text("File", USE_SPACE | USE_COLON);
  n += printf_text(ecget_namebyoffset(p, sh_link, vn_file), USE_LT | USE_SPACE);
  n += printf_text("Cnt", USE_SPACE | USE_COLON);
  n += printf_nice(vn_cnt, USE_DEC | USE_EOL);

  return n;
}

static int dump_versionneed2(const pbuffer_t p, const int offset, const uint64_t sh_link,
                             const uint64_t vna_name, const uint64_t vna_flags, const uint64_t vna_other) {
  int n = 0;
  n += printf_nice(offset, USE_FHEX16 | USE_COLON);
  n += printf_text("Name", USE_TAB | USE_COLON);
  n += printf_text(ecget_namebyoffset(p, sh_link, vna_name), USE_LT | USE_SPACE | SET_PAD(14));
  n += printf_text("Flags", USE_SPACE | USE_COLON);
  n += printf_masknone(ecVNA_FLAGS, vna_flags, USE_LT);
  n += printf_text("Version", USE_TAB | USE_COLON);
  n += printf_nice(vna_other, USE_DEC | USE_EOL);

  return n;
}

static int dump_versionneed32(const pbuffer_t p, const poptions_t o, Elf32_Shdr *shdr) {
  int n = 0;
  n += dump_versionneed0(p, shdr->sh_name, shdr->sh_info, shdr->sh_addr, shdr->sh_offset, shdr->sh_link);

  Elf32_Word offset0 = 0;
  for (Elf32_Word j = 0; j < shdr->sh_info; ++j) {
    Elf32_Verneed *vn = getp(p, shdr->sh_offset + offset0, sizeof(Elf32_Verneed));
    if (vn) {
      n += dump_versionneed1(p, offset0, shdr->sh_link, vn->vn_version, vn->vn_file, vn->vn_cnt);

      Elf32_Word offset1 = offset0 + vn->vn_aux;
      for (Elf32_Half k = 0; k < vn->vn_cnt; ++k) {
        Elf32_Vernaux *va = getp(p, shdr->sh_offset + offset1, sizeof(Elf32_Vernaux));
        if (va) {
          n += dump_versionneed2(p, offset1, shdr->sh_link, va->vna_name, va->vna_flags, va->vna_other);
          offset1 += va->vna_next;
        }
      }

      offset0 += vn->vn_next;
    }
  }

  n += printf_eol();

  return n;
}

static int dump_versionneed64(const pbuffer_t p, const poptions_t o, Elf64_Shdr *shdr) {
  int n = 0;
  n += dump_versionneed0(p, shdr->sh_name, shdr->sh_info, shdr->sh_addr, shdr->sh_offset, shdr->sh_link);

  Elf64_Word offset0 = 0;
  for (Elf64_Word j = 0; j < shdr->sh_info; ++j) {
    Elf64_Verneed *vn = getp(p, shdr->sh_offset + offset0, sizeof(Elf64_Verneed));
    if (vn) {
      n += dump_versionneed1(p, offset0, shdr->sh_link, vn->vn_version, vn->vn_file, vn->vn_cnt);

      Elf64_Word offset1 = offset0 + vn->vn_aux;
      for (Elf64_Half k = 0; k < vn->vn_cnt; ++k) {
        Elf64_Vernaux *va = getp(p, shdr->sh_offset + offset1, sizeof(Elf64_Vernaux));
        if (va) {
          n += dump_versionneed2(p, offset1, shdr->sh_link, va->vna_name, va->vna_flags, va->vna_other);
          offset1 += va->vna_next;
        }
      }

      offset0 += vn->vn_next;
    }
  }

  n += printf_eol();

  return n;
}

static int dump_version32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr) {
  /* Scan the sections for the version section. */
  int n = 0;
  Elf32_Half cnt = 0;
  for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf32_Shdr, sx);
    Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, i);
    if (s0 && (SHT_GNU_versym == s0->sh_type || SHT_GNU_verneed == s0->sh_type)) ++cnt;
  }

  if (0 == cnt) {
    printf_w("No version information found in this file.");
  } else {
    for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
      MEMSTACK(Elf32_Shdr, sx);
      Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, i);
      if (s0) {
        if (SHT_GNU_versym == s0->sh_type) {
          n += dump_versionsym32(p, o, ehdr, s0);
        } else if (SHT_GNU_verneed == s0->sh_type) {
          n += dump_versionneed32(p, o, s0);
        }
      }
    }
  }

  return n;
}

static int dump_version64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  /* Scan the sections for the version section. */
  int n = 0;
  Elf64_Half cnt = 0;
  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf64_Shdr, sx);
    Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, i);
    if (s0 && (SHT_GNU_versym == s0->sh_type || SHT_GNU_verneed == s0->sh_type)) ++cnt;
  }

  if (0 == cnt) {
    printf_w("No version information found in this file.");
  } else {
    for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
      MEMSTACK(Elf64_Shdr, sx);
      Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, i);
      if (s0) {
        if (SHT_GNU_versym == s0->sh_type) {
          n += dump_versionsym64(p, o, ehdr, s0);
        } else if (SHT_GNU_verneed == s0->sh_type) {
          n += dump_versionneed64(p, o, s0);
        }
      }
    }
  }

  return n;
}

static int dump_actions32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr) {
  MALLOCA(const char*, secdone, ehdr->e_shnum);

  paction_t x = o->actions;
  while (x) {
    if (x->secname[0]) {
      MEMSTACK(Elf32_Shdr, sx);
      Elf32_Shdr* s0 = ecget_shdr32byname(p, sx, x->secname);
      if (s0) {
        MALLOCSWRAP3(opwrap_t, s, MODE_OCSHDR32, s0, p, -1);
        if (!isnamedone(secdone, NELEMENTS(secdone), x->secname)) {
          dump_actions1(p, o, s0->sh_offset, s0->sh_type != SHT_NOBITS ? s0->sh_size : 0);
        }
        dump_actions2(p, o, ps, x->secname, x->action, s0->sh_offset, s0->sh_type != SHT_NOBITS ? s0->sh_size : 0, s0->sh_addr);
      } else {
        printf_w("section '%s' was not dumped because it does not exist!", x->secname);
      }
    }

    x = x->actions;
  }

  return 0;
}

static int dump_actions64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  MALLOCA(const char*, secdone, ehdr->e_shnum);

  int n = 0;
  paction_t x = o->actions;
  while (x) {
    if (x->secname[0]) {
      MEMSTACK(Elf64_Shdr, sx);
      Elf64_Shdr* s0 = ecget_shdr64byname(p, sx, x->secname);
      if (s0) {
        MALLOCSWRAP3(opwrap_t, s, MODE_OCSHDR64, s0, p, -1);
        if (!isnamedone(secdone, NELEMENTS(secdone), x->secname)) {
          dump_actions1(p, o, s0->sh_offset, s0->sh_type != SHT_NOBITS ? s0->sh_size : 0);
        }
        n += dump_actions2(p, o, ps, x->secname, x->action, s0->sh_offset, s0->sh_type != SHT_NOBITS ? s0->sh_size : 0, s0->sh_addr);
      } else {
        printf_w("section '%s' was not dumped because it does not exist!", x->secname);
      }
    }

    x = x->actions;
  }

  return n;
}

static int dump_notes0(const pbuffer_t p, const int index, const uint64_t e_machine,
                       const uint64_t n_descsz, const uint64_t n_type, const unknown_t data) {
  int n = 0;
  n += printf_text("Displaying notes found in", USE_LT);
  n += printf_text(ecget_secnamebyindex(p, index), USE_LT | USE_SPACE | USE_SQ | USE_COLON | USE_EOL);

  n += printf_text("Owner", USE_LT | USE_TAB | SET_PAD(22));
  n += printf_text("Data size", USE_LT | USE_SPACE | SET_PAD(11));
  n += printf_text("Description", USE_LT | USE_SPACE | USE_EOL);

  n += printf_text(ecget_nhdrnamebyindex(p, index), USE_LT | USE_TAB | SET_PAD(22));
  n += printf_nice(n_descsz, USE_FHEX32);
  n += printf_pick(get_NHDRTYPE(p), n_type, USE_LT | USE_SPACE | USE_EOL);

  uint32_t* pc = CAST(uint32_t*, data);

  if (NT_GNU_BUILD_ID == n_type) {
    n += printf_text("Build ID", USE_LT | USE_TAB | USE_COLON);
    n += printf_sore(data, n_descsz, USE_HEX | USE_TAB);
    n += printf_eol();
  } else if (NT_GNU_GOLD_VERSION == n_type) {
    n += printf_text("Version", USE_LT | USE_TAB | USE_COLON);
    n += printf_sore(data, n_descsz, USE_STR);
    n += printf_eol();
  } else if (NT_GNU_ABI_TAG == n_type) {
    n += printf_text("OS", USE_LT | USE_TAB | USE_COLON);
    n += printf_pick(ecGNUABITAB, pc[0], USE_LT | USE_SPACE);
    n += printf_text("ABI", USE_LT | USE_TAB | USE_COLON);
    n += printf_nice(pc[1], USE_DEC);
    n += printf_nice(pc[2], USE_DEC | USE_DOT);
    n += printf_nice(pc[3], USE_DEC | USE_DOT | USE_EOL);
  } else if (NT_GNU_PROPERTY_TYPE_0 == n_type) {
    n += printf_text("Properties", USE_LT | USE_TAB | USE_COLON);

    const uint32_t size0 = sizeof(uint32_t);
    const uint32_t size1 = size0 + size0;

    uint32_t i = 0;
    for (uint32_t j = 0; j < n_descsz; j += size1) {
      if ((n_descsz - j) < size1) {
//        n += printf_nice(n_descsz, USE_CORRUPT);
        break;
      } else {
        uint32_t x = pc[i++];
        uint32_t datasz = pc[i++];
        if ((n_descsz - j) <= datasz) {
//          n += printf("<corrupt type (%#x) datasz: %#x>\n", x, datasz);
          break;
        } else {
          if (x >= GNU_PROPERTY_LOPROC && x <= GNU_PROPERTY_HIPROC) {
            if (e_machine == EM_X86_64 || e_machine == EM_IAMCU || e_machine == EM_386) {
              if (size0 != datasz) {
                n += printf_pick(ecGNUPROPERTY, x, USE_LT | USE_SPACE);
                n += printf_nice(datasz, USE_CORRUPT | USE_COLON);
              } else {
                n += printf_pick(ecGNUPROPERTY, x, USE_LT | USE_SPACE | USE_COLON);
              }

              if (x == GNU_PROPERTY_X86_FEATURE_1_AND) {
                n += printf_mask(ecGNUPROPERTY_X86_FEATURE_1_AND, pc[i++], USE_LT);
              }
            }
          }
        }
      }
    }

    n += printf_eol();
  } else {
    n += printf_text("Description Data", USE_LT | USE_TAB | USE_COLON);
    n += printf_sore(pc, n_descsz, USE_HEX | USE_TAB);
    n += printf_eol();
  }

  n += printf_eol();

  return n;
}

static int dump_notes32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr) {
  int n = 0;
  if (ET_CORE != ehdr->e_type) {
    for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
      Elf32_Nhdr *nhdr = ecget_nhdr32byindex(p, i);
      if (nhdr) {
        Elf32_Word *pc = ecget_nhdrdesc32byindex(p, i);
        n += dump_notes0(p, i, ehdr->e_machine, nhdr->n_descsz, nhdr->n_type, pc);
      }
    }
  }

  return n;
}

static int dump_notes64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  int n = 0;
  if (ET_CORE != ehdr->e_type) {
    for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
      Elf64_Nhdr *nhdr = ecget_nhdr64byindex(p, i);
      if (nhdr) {
        Elf64_Word *pc = ecget_nhdrdesc64byindex(p, i);
        n += dump_notes0(p, i, ehdr->e_machine, nhdr->n_descsz, nhdr->n_type, pc);
      }
    }
  }

  return n;
}

static int dump_archspecific0(const pbuffer_t p, const poptions_t o, const char* name, const uint64_t sh_offset, const uint64_t sh_size) {
  int n = 0;

  return n;
}

static int dump_archspecific1(const pbuffer_t p, const poptions_t o, const char* name, const uint64_t sh_offset, const uint64_t sh_size) {
  int n = 0;

  const int MAXSIZE = strlenpick(ecPUBLICTAGARM) + 2;

  handle_t p0 = fmalloc(getp(p, sh_offset, sh_size), sh_size, MEMFIND_NOCHUNKSIZE);
  if (p0) {
    char version = fgetu8(p0);
    if ('A' == version) {
      while (!fiseof(p0)) {
        uint64_t attrsize = fgetu32(p0);
        if (attrsize > (fgetsize(p0) + 4) || attrsize < 5) break;

        char* attrname = fgetstring(p0);
        n += printf_text("Attribute Section", USE_LT | USE_COLON);
        n += printf_text(attrname, USE_LT | USE_SPACE | USE_EOL);

          bool_t isok = TRUE;
          uint64_t tag = fgetu8(p0);
          uint64_t siz = fgetu32(p0);
          if (1 == tag) {
            n += printf_text("File Attributes", USE_LT | USE_EOL);
          } else if (2 == tag) {
            n += printf_text("Section Attributes", USE_LT | USE_COLON);
          } else if (3 == tag) {
            n += printf_text("Symbol Attributes", USE_LT | USE_COLON);
          } else {
            isok = FALSE;
          }

          if (isok) {
            if (0 == xstrcmp(attrname, name)) {
              handle_t p1 = fmalloc(p0, siz - 1, MEMFIND_NOCHUNKSIZE);
              while (!fiseof(p1)) {
                uint64_t tag = fgetuleb128(p1);
                if (0 == tag) continue;

                n += printf_pick(ecPUBLICTAGARM, tag, USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
                if (isused(ecPUBLICTAGARMSTRING, tag)) {
                  n += printf_text(fgetstring(p1), USE_LT | USE_SPACE | USE_DQ);
                } else if (TAG_compatibility == tag) {
                  n += printf_text("flag", USE_LT | USE_SPACE | USE_COLON);
                  n += printf_nice(fgetuleb128(p1), USE_DEC);
                  n += printf_text("value", USE_LT | USE_SPACE | USE_COLON);
                  n += printf_text(fgetstring(p1), USE_LT | USE_SPACE);
                } else if (TAG_nodefaults == tag) {
                  n += printf_nice(fgetu8(p1), USE_BOOL);
                } else {
                  uint64_t val = fgetuleb128(p1);
                  pconvert_t p2 = convertpicknull(ecPUBLICTAGARM, tag);
                  if (p2 && p2->param) {
                    n += printf_pick(p2->param, val, USE_SPACE);
                  }
                }
                n += printf_eol();
              }
            } else if (0 == xstrcmp(attrname, "gnu")) {
            }
          }
      }
    }

    ffree(p0);
  }

  return n;
}

static int dump_archspecific32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr) {
  int n = 0;
  for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf32_Shdr, sx);
    Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, i);
    if (s0) {
      if (SHT_ARM_ATTRIBUTES == s0->sh_type) {
        n += dump_archspecific1(p, o, "aeabi", s0->sh_offset, s0->sh_size);
      } else if (SHT_GNU_ATTRIBUTES == s0->sh_type) {
        n += dump_archspecific0(p, o, NULL, s0->sh_offset, s0->sh_size);
      }
    }
  }

  return n;
}

static int dump_archspecific64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  int n = 0;
  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf64_Shdr, sx);
    Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, i);
    if (s0) {
      if (SHT_ARM_ATTRIBUTES == s0->sh_type) {
        n += dump_archspecific1(p, o, "aeabi", s0->sh_offset, s0->sh_size);
      } else if (SHT_GNU_ATTRIBUTES == s0->sh_type) {
        n += dump_archspecific0(p, o, NULL, s0->sh_offset, s0->sh_size);
      }
    }
  }

  return n;
}

static int dump_dwarf32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr) {
  int n = 0;
  handle_t oc = ocattach(p);
  ocdwarf_open(oc, o);

  for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf32_Shdr, sx);
    Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, i);
    if (s0) {
      MALLOCSWRAP3(opwrap_t, sec, MODE_OCSHDR32, s0, p, i);

      if (ocdwarf_isneeded(oc, psec)) {
        printf_text("Contents of section", USE_LT);
        printf_text(ecget_secnamebyindex(p, i), USE_LT | USE_SPACE | USE_SQ);
        printf_text("at offset", USE_LT | USE_SPACE);
        printf_nice(s0->sh_offset, USE_FHEX16 | USE_COLON | USE_EOL);

        ocdwarf_run(oc, psec);

        printf_eol();
      }
    }
  }

  ocdwarf_close(oc);
  occlose(oc);
  return n;
}


static int dump_dwarf64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  int n = 0;
  handle_t oc = ocattach(p);
  ocdwarf_open(oc, o);

  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    MEMSTACK(Elf64_Shdr, sx);
    Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, i);
    if (s0) {
      MALLOCSWRAP3(opwrap_t, sec, MODE_OCSHDR64, s0, p, i);

      if (ocdwarf_isneeded(oc, psec)) {
        printf_text("Contents of section", USE_LT);
        printf_text(ecget_secnamebyindex(p, i), USE_LT | USE_SPACE | USE_SQ);
        printf_text("at offset", USE_LT | USE_SPACE);
        printf_nice(s0->sh_offset, USE_FHEX16 | USE_COLON | USE_EOL);

        ocdwarf_run(oc, psec);

        printf_eol();
      }
    }
  }

  ocdwarf_close(oc);
  occlose(oc);
  return n;
}

int readelf(const pbuffer_t p, const poptions_t o) {
  if (isELF(p)) {
    dump_summary(p, o);

    if (MODE_ISANY(o->action, OPTREADELF_FILEHEADER))           dump_elfheader(p, o);

    if (isELF32(p)) {
      Elf32_Ehdr *ehdr = ecget_ehdr32(p, xmalloc(sizeof(Elf32_Ehdr), MODE_HEAP));
      if (ehdr) {
        if (MODE_ISANY(o->action, OPTPROGRAM_INFO))             dump_summary(p, o);
        if (MODE_ISANY(o->action, OPTREADELF_FILEHEADER))       dump_fileheader32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_SECTIONMASK))      dump_sectionheaders32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_SECTIONGROUPS))    dump_sectiongroups32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_PROGRAMHEADERS))   dump_programheaders32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_DYNAMIC))          dump_dynamic32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_RELOCS))           dump_relocs32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_UNWIND))           dump_unwind32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_SYMBOLS))          dump_symbols32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_HISTOGRAM))        dump_histogram32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_VERSION))          dump_version32(p, o, ehdr);
        if (o->actions)                                         dump_actions32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_NOTES))            dump_notes32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_ARCHSPECIFIC))     dump_archspecific32(p, o, ehdr);
        if (MODE_ISANY(o->ocdump, OPTDWARF_DEBUGGING))          dump_dwarf32(p, o, ehdr);
      }

      xfree(ehdr);
    } else if (isELF64(p)) {
      Elf64_Ehdr *ehdr = ecget_ehdr64(p, xmalloc(sizeof(Elf64_Ehdr), MODE_HEAP));
      if (ehdr) {
        if (MODE_ISANY(o->action, OPTPROGRAM_INFO))             dump_summary(p, o);
        if (MODE_ISANY(o->action, OPTREADELF_FILEHEADER))       dump_fileheader64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_SECTIONMASK))      dump_sectionheaders64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_SECTIONGROUPS))    dump_sectiongroups64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_PROGRAMHEADERS))   dump_programheaders64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_DYNAMIC))          dump_dynamic64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_RELOCS))           dump_relocs64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_UNWIND))           dump_unwind64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_SYMBOLS))          dump_symbols64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_HISTOGRAM))        dump_histogram64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_VERSION))          dump_version64(p, o, ehdr);
        if (o->actions)                                         dump_actions64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_NOTES))            dump_notes64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_ARCHSPECIFIC))     dump_archspecific64(p, o, ehdr);
        if (MODE_ISANY(o->ocdump, OPTDWARF_DEBUGGING))          dump_dwarf64(p, o, ehdr);
      }

      xfree(ehdr);
    }
  } else {
    printf_e("not an ELF file - it has the wrong magic bytes at the start.");
  }

  return 0;
}

