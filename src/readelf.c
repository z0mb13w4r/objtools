#include <string.h>

#include "printf.h"
#include "readelf.h"
#include "elfcode.h"
#include "objutils.h"

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
#include "static/reltype.ci"
#include "static/shdrtype.ci"
#include "static/shnindex.ci"
#include "static/stbbind.ci"
#include "static/stttype.ci"
#include "static/stvvisibility.ci"
#include "static/vna_flags.ci"

static const char* get_gnuabitab64(const unsigned int x) {
  return strpick(zGNUABITAB, x);
}

static const char* get_symboltype64(const unsigned int x) {
  return strpick(zSTTTYPE, x);
}

static const char* get_symbolbinding64(const unsigned int x) {
  return strpick(zSTBBIND, x);
}

static const char* get_symbolvisibility64(const unsigned int x) {
  return strpick(zSTVVISIBILITY, x);
}

static const char* get_symbolindex64(const unsigned int x) {
  static char buff[32];

  const char* def = strpicknull(zSHNINDEX, x);
  if (NULL == def) {
    snprintf(buff, sizeof(buff), "%3d", x);
  } else {
    snprintf(buff, sizeof(buff), "%3s", def);
  }

  return buff;
}

static const char* get_ehdrmachine64(Elf64_Ehdr *e) {
  if (e) {
    return strpick(zEHDRMACHINE, e->e_machine);
  }

  return NULL;
}

static const char* get_ehdrosabi(pbuffer_t p) {
  if (p) {
    const unsigned int osabi = get(p, EI_OSABI);
    const char* s = strpicknull(zEHDROSABI, osabi);
    if (NULL == s) {
      Elf64_Ehdr *e = get_ehdr64(p);

      if (e && osabi >= 64) {
        switch (e->e_machine) {
        case EM_AMDGPU:
          return strpick(zEHDROSABIAMDGPU, osabi);

        case EM_ARM:
          return strpick(zEHDROSABIARM, osabi);

        case EM_MSP430:
        case EM_VISIUM:
          return strpick(zEHDROSABIMSP430, osabi);

        case EM_TI_C6000:
          return strpick(zEHDROSABIC6000, osabi);

        default:
          break;
        }
      }

      return strpickunknown(osabi);
    }

    return s;
  }

  return NULL;

}

static const char* get_reltype(Elf64_Rel *r) {
  if (r) {
    return strpick(zRELTYPE, r->r_info & 0xffff);
  }

  return NULL;
}

static const char* get_relatype(Elf64_Rela *r) {
  if (r) {
    return strpick(zRELTYPE, r->r_info & 0xffff);
  }

  return NULL;
}

static const char* get_nhdrtype64(const pbuffer_t p, Elf64_Nhdr *n) {
  if (n) {
    Elf32_Ehdr *e = get_ehdr32(p);
    if (e) {
      if (ET_CORE == e->e_type) {
        return strpick(zNHDRTYPECORE, n->n_type);
      } else {
        return strpick(zNHDRTYPE, n->n_type);
      }
    }
  }

  return NULL;
}

static int make_versionnames64(const pbuffer_t p, Elf64_Word *vnames, const size_t size) {
  Elf64_Shdr *vh = get_shdr64bytype(p, SHT_GNU_verneed);
  if (vh) {
    Elf64_Word offset = 0;
    vnames[0] = vh->sh_link;

    for (Elf64_Word j = 0; j < vh->sh_info; ++j) {
      Elf64_Verneed *vn = getp(p, vh->sh_offset, sizeof(Elf64_Verneed));
      if (vn) {
        Elf64_Word xoffset = offset + vn->vn_aux;
        for (Elf64_Half k = 0; k < vn->vn_cnt; ++k) {
          Elf64_Vernaux *va = getp(p, vh->sh_offset + xoffset, sizeof(Elf64_Vernaux));
          if (va) {
            if (va->vna_other < size) {
              vnames[va->vna_other] = va->vna_name;
            }
            xoffset += va->vna_next;
          }
        }
      }

      offset += vn->vn_next;
    }

    return vh->sh_link;
  }

  return 0;
}

static int dump_elfheader(const pbuffer_t p, const poptions_t o) {
  int MAXSIZE = 36;

  printf_text("ELF HEADER", USE_LT | USE_COLON | USE_EOL);

  printf("  Magic: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
            get(p,  0), get(p,  1), get(p,  2), get(p,  3),
            get(p,  4), get(p,  5), get(p,  6), get(p,  7),
            get(p,  8), get(p,  9), get(p, 10), get(p, 11),
            get(p, 12), get(p, 13), get(p, 14), get(p, 15));

  printf_text("Class", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  if (is32(p)) {
    printf_text("ELF32", USE_LT | USE_SPACE | USE_EOL);
  } else if (is64(p)) {
    printf_text("ELF64", USE_LT | USE_SPACE | USE_EOL);
  } else {
    return -1;
  }

  printf_text("Data", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  if (isLittleEndian(p)) {
    printf_text("2's complement, little endian", USE_LT | USE_SPACE | USE_EOL);
  } else if (isBigEndian(p)) {
    printf_text("2's complement, big endian", USE_LT | USE_SPACE | USE_EOL);
  } else {
    printf_text("none", USE_LT | USE_SPACE | USE_EOL);
  }

  printf_text("Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  if (EV_CURRENT == get(p, EI_VERSION)) {
    printf_text("1 (current)", USE_LT | USE_SPACE | USE_EOL);
  } else if (EV_NONE == get(p, EI_VERSION)) {
    printf_text("0", USE_LT | USE_SPACE | USE_EOL);
  } else {
    printf_nice(get(p, EI_VERSION), USE_UNKNOWN | USE_EOL);
  }

  printf_text("OS/ABI", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_text(get_ehdrosabi(p), USE_LT | USE_SPACE | USE_EOL);
  printf_text("ABI Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(get(p, EI_ABIVERSION), USE_DEC | USE_EOL);

  return 0;
}

static int dump_fileheader32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr) {
  int MAXSIZE = 36;

  printf_text("Type", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_pick(zEHDRTYPE, ehdr->e_type, USE_LT | USE_SPACE | USE_EOL);
  printf_text("Machine", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_pick(zEHDRMACHINE, ehdr->e_machine, USE_LT | USE_SPACE | USE_EOL);
  printf_text("Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_version, USE_FHEX | USE_EOL);
  printf_text("Entry point address", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_entry, USE_FHEX16 | USE_EOL);
  printf_text("Start of program headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_phoff, USE_DEC | USE_BYTES | USE_EOL);
  printf_text("Start of section headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_shoff, USE_DEC | USE_BYTES | USE_EOL);
  printf_text("Flags", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_flags, USE_FHEX | USE_EOL);
  printf_text("Size of this header", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_ehsize, USE_DEC | USE_BYTES | USE_EOL);
  printf_text("Size of program headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_phentsize, USE_DEC | USE_BYTES | USE_EOL);
  printf_text("Number of program headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_phnum, USE_DEC | USE_EOL);
  printf_text("Size of section headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_shentsize, USE_DEC | USE_BYTES | USE_EOL);
  printf_text("Number of section headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_shnum, USE_DEC | USE_EOL);
  printf_text("Section header string table index", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_shstrndx, USE_DEC | USE_EOL);
  printf_eol();

  return 0;
}

static int dump_fileheader64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  int MAXSIZE = 36;

  printf_text("Type", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_pick(zEHDRTYPE, ehdr->e_type, USE_LT | USE_SPACE | USE_EOL);
  printf_text("Machine", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_pick(zEHDRMACHINE, ehdr->e_machine, USE_LT | USE_SPACE | USE_EOL);
  printf_text("Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_version, USE_FHEX | USE_EOL);
  printf_text("Entry point address", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_entry, USE_FHEX16 | USE_EOL);
  printf_text("Start of program headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_phoff, USE_DEC | USE_BYTES | USE_EOL);
  printf_text("Start of section headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_shoff, USE_DEC | USE_BYTES | USE_EOL);
  printf_text("Flags", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_flags, USE_FHEX | USE_EOL);
  printf_text("Size of this header", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_ehsize, USE_DEC | USE_BYTES | USE_EOL);
  printf_text("Size of program headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_phentsize, USE_DEC | USE_BYTES | USE_EOL);
  printf_text("Number of program headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_phnum, USE_DEC | USE_EOL);
  printf_text("Size of section headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_shentsize, USE_DEC | USE_BYTES | USE_EOL);
  printf_text("Number of section headers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_shnum, USE_DEC | USE_EOL);
  printf_text("Section header string table index", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  printf_nice(ehdr->e_shstrndx, USE_DEC | USE_EOL);
  printf_eol();

  return 0;
}

static int dump_sectionheaders32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr) {
  printf_text("SECTION HEADERS", USE_LT | USE_COLON | USE_EOL);
  printf("  [Nr] Name                 Type            Address          Off      Size     ES Flg Lk Inf  Al\n");

  for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
    printf_nice(i, USE_DEC2 | USE_TAB | USE_SB);

    Elf32_Shdr *shdr = get_shdr32byindex(p, i);
    if (shdr) {
      printf_text(get_secname32byindex(p, i), USE_LT | USE_SPACE | SET_PAD(21));
      printf_pick(zSHDRTYPE, shdr->sh_type, USE_LT | USE_SPACE | SET_PAD(16));
      printf_nice(shdr->sh_addr, USE_LHEX64);
      printf_nice(shdr->sh_offset, USE_LHEX32);
      printf_nice(shdr->sh_size, USE_LHEX32);
      printf_nice(shdr->sh_entsize, USE_LHEX8);
      printf_mask(EHDRFLAGS, shdr->sh_flags, USE_RT | USE_NOSPACE | SET_PAD(4));
      printf_nice(shdr->sh_link, USE_DEC2);
      printf_nice(shdr->sh_info, USE_DEC3);
      printf_nice(shdr->sh_addralign, USE_DEC3);
    }
    printf_eol();
  }

  printf_text("Key to Flags", USE_LT | USE_COLON | USE_EOL);
  printf_text("W (write), A (alloc), X (execute), M (merge), S (strings), I (info),", USE_LT | USE_TAB | USE_EOL);
  printf_text("L (link order), O (extra OS processing required), G (group), T (TLS),", USE_LT | USE_TAB | USE_EOL);
  printf_text("C (compressed), x (unknown), o (OS specific), E (exclude),", USE_LT | USE_TAB | USE_EOL);
  printf_text("l (large), p (processor specific)", USE_LT | USE_TAB | USE_EOL);
  printf_eol();

  return 0;
}

static int dump_sectionheaders64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  printf_text("SECTION HEADERS", USE_LT | USE_COLON | USE_EOL);
  printf("  [Nr] Name                 Type            Address          Off      Size     ES Flg Lk Inf  Al\n");

  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    printf_nice(i, USE_DEC2 | USE_TAB | USE_SB);

    Elf64_Shdr *shdr = get_shdr64byindex(p, i);
    if (shdr) {
      printf_text(get_secname64byindex(p, i), USE_LT | USE_SPACE | SET_PAD(21));
      printf_pick(zSHDRTYPE, shdr->sh_type, USE_LT | USE_SPACE | SET_PAD(16));
      printf_nice(shdr->sh_addr, USE_LHEX64);
      printf_nice(shdr->sh_offset, USE_LHEX32);
      printf_nice(shdr->sh_size, USE_LHEX32);
      printf_nice(shdr->sh_entsize, USE_LHEX8);
      printf_mask(EHDRFLAGS, shdr->sh_flags, USE_RT | USE_NOSPACE | SET_PAD(4));
      printf_nice(shdr->sh_link, USE_DEC2);
      printf_nice(shdr->sh_info, USE_DEC3);
      printf_nice(shdr->sh_addralign, USE_DEC3);
    }
    printf_eol();
  }

  printf_text("Key to Flags", USE_LT | USE_COLON | USE_EOL);
  printf_text("W (write), A (alloc), X (execute), M (merge), S (strings), I (info),", USE_LT | USE_TAB | USE_EOL);
  printf_text("L (link order), O (extra OS processing required), G (group), T (TLS),", USE_LT | USE_TAB | USE_EOL);
  printf_text("C (compressed), x (unknown), o (OS specific), E (exclude),", USE_LT | USE_TAB | USE_EOL);
  printf_text("l (large), p (processor specific)", USE_LT | USE_TAB | USE_EOL);
  printf_eol();

  return 0;
}

static int dump_sectiongroups32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr) {
  /* Scan the sections for the group section. */
  Elf32_Half cnt = 0;
  for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
    Elf32_Shdr *shdr = get_shdr32byindex(p, i);
    if (shdr && SHT_GROUP == shdr->sh_type) ++cnt;
  }

  if (0 == cnt) {
    printf("%s: WARNING: There are no section groups in this file.\n\n", o->prgname);
  } else {
    // TBD
  }

  return 0;
}

static int dump_sectiongroups64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  /* Scan the sections for the group section. */
  Elf64_Half cnt = 0;
  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    Elf64_Shdr *shdr = get_shdr64byindex(p, i);
    if (shdr && SHT_GROUP == shdr->sh_type) ++cnt;
  }

  if (0 == cnt) {
    printf("%s: WARNING: There are no section groups in this file.\n\n", o->prgname);
  } else {
    // TBD
  }

  return 0;
}

static int dump_programheaders64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  printf_text("PROGRAM HEADERS", USE_LT | USE_COLON | USE_EOL);
  printf("  Type            Offset   VirtAddr           PhysAddr           FileSiz  MemSiz   Flg  Align\n");
  for (Elf64_Half i = 0; i < ehdr->e_phnum; ++i) {
    Elf64_Phdr *phdr = get_phdr64byindex(p, i);
    if (phdr) {
      printf_pick(zPHDRTYPE, phdr->p_type, USE_LT | USE_TAB | SET_PAD(17));
      printf_nice(phdr->p_offset, USE_FHEX24);
      printf_nice(phdr->p_vaddr,  USE_FHEX64);
      printf_nice(phdr->p_paddr,  USE_FHEX64);
      printf_nice(phdr->p_filesz, USE_FHEX24);
      printf_nice(phdr->p_memsz,  USE_FHEX24);

      printf(" %c%c%c ",
            (phdr->p_flags & PF_R ? 'R' : ' '),
            (phdr->p_flags & PF_W ? 'W' : ' '),
            (phdr->p_flags & PF_X ? 'E' : ' '));

      printf_nice(phdr->p_align, USE_FHEX);
      printf_eol();

      if (PT_INTERP == phdr->p_type) {
        printf("    [Requesting program interpreter:" );
        printf_data(getp(p, phdr->p_offset, phdr->p_filesz), phdr->p_filesz, 0, USE_STR);
        printf("]\n");
      }
    }
  }

  printf_eol();

  printf_text("Section to Segment mapping", USE_LT | USE_COLON | USE_EOL);
  printf_text("Segment Sections...", USE_LT | USE_SPACE | USE_EOL);
  for (Elf64_Half i = 0; i < ehdr->e_phnum; ++i) {
    printf_nice(i, USE_TAB | USE_DEC2Z);

    Elf64_Phdr *phdr = get_phdr64byindex(p, i);
    if (phdr) {
      for (Elf64_Half j = 1; j < ehdr->e_shnum; ++j) {
        Elf64_Shdr *shdr = get_shdr64byindex(p, j);
        if (shdr) {
          if (shdrinphdr64(shdr, phdr)) {
            printf_text(get_secname64byindex(p, j), USE_SPACE);
          }
        }
      }
    }
    printf_eol();
  }

  printf_eol();

  return 0;
}

static int dump_dynamic64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    Elf64_Shdr *shdr = get_shdr64byindex(p, i);
    if (shdr && SHT_DYNAMIC == shdr->sh_type) {
      size_t cnt = shdr->sh_size / shdr->sh_entsize;

      printf_text("Dynamic section at offset", USE_LT);
      printf_nice(shdr->sh_offset, USE_FHEX16);
      printf_text("contains", USE_LT | USE_SPACE);
      printf_nice(cnt, USE_DEC);
      printf_text(1 == cnt ? "entry" : "entries", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
      printf(" Tag                Type                Name/Value\n");

      Elf64_Dyn *dyn = get64byshdr(p, shdr);
      for (size_t j = 0; j < cnt; j++) {
        printf_nice(dyn->d_tag, USE_FHEX64);
        printf_pick(zDYNTAG, dyn->d_tag, USE_SPACE | USE_RB | SET_PAD(20));

        if (dyn->d_tag == DT_FLAGS_1) {
          printf_text("Flags", USE_LT | USE_SPACE | USE_COLON);
          printf_masknone(zDT_FLAGS_1, dyn->d_un.d_val, USE_LT);
        } else if (dyn->d_tag == DT_POSFLAG_1) {
          printf_text("Flags", USE_LT | USE_SPACE | USE_COLON);
          printf_masknone(zDT_POSFLAG_1, dyn->d_un.d_val, USE_LT);
        } else if (dyn->d_tag == DT_FLAGS) {
          printf_text("Flags", USE_LT | USE_SPACE | USE_COLON);
          printf_masknone(zDT_FLAGS, dyn->d_un.d_val, USE_LT);
        } else if (dyn->d_tag == DT_PLTREL) {
          printf_pick(zDYNTAG, dyn->d_un.d_val, USE_SPACE);
        } else if (dyn->d_tag == DT_NULL || dyn->d_tag == DT_NEEDED || dyn->d_tag == DT_PLTGOT ||
                   dyn->d_tag == DT_HASH || dyn->d_tag == DT_STRTAB || dyn->d_tag == DT_SYMTAB ||
                   dyn->d_tag == DT_RELA || dyn->d_tag == DT_INIT || dyn->d_tag == DT_FINI ||
                   dyn->d_tag == DT_SONAME || dyn->d_tag == DT_RPATH || dyn->d_tag ==  DT_SYMBOLIC ||
                   dyn->d_tag == DT_REL || dyn->d_tag == DT_DEBUG || dyn->d_tag == DT_TEXTREL ||
                   dyn->d_tag == DT_JMPREL || dyn->d_tag == DT_RUNPATH) {

          const char *name = get_name64byoffset(p, shdr->sh_link, dyn->d_un.d_val);
          if (name) {
            if (dyn->d_tag == DT_NEEDED) {
              printf(" Shared library: [%s]", name);
              // TBD
            } else if (dyn->d_tag == DT_SONAME)        printf(" Library soname: [%s]", name);
              else if (dyn->d_tag == DT_RPATH)         printf(" Library rpath: [%s]", name);
              else if (dyn->d_tag == DT_RUNPATH)       printf(" Library runpath: [%s]", name);
              else                                     printf_nice(dyn->d_un.d_val, USE_FHEX);
          }
        } else if (dyn->d_tag == DT_PLTRELSZ || dyn->d_tag == DT_RELASZ || dyn->d_tag == DT_STRSZ ||
                   dyn->d_tag == DT_RELSZ || dyn->d_tag == DT_RELAENT || dyn->d_tag == DT_SYMENT ||
                   dyn->d_tag == DT_RELENT || dyn->d_tag == DT_PLTPADSZ || dyn->d_tag == DT_MOVEENT ||
                   dyn->d_tag == DT_MOVESZ || dyn->d_tag == DT_PREINIT_ARRAYSZ || dyn->d_tag == DT_INIT_ARRAYSZ ||
                   dyn->d_tag == DT_FINI_ARRAYSZ || dyn->d_tag == DT_GNU_CONFLICTSZ || dyn->d_tag == DT_GNU_LIBLISTSZ) {
          printf_nice(dyn->d_un.d_val, USE_DEC | USE_BYTES);
        } else if (dyn->d_tag == DT_VERDEFNUM || dyn->d_tag == DT_VERNEEDNUM ||
                   dyn->d_tag == DT_RELACOUNT || dyn->d_tag == DT_RELCOUNT) {
          printf_nice(dyn->d_un.d_val, USE_DEC);
        } else if (dyn->d_tag == DT_SYMINSZ || dyn->d_tag == DT_SYMINENT || dyn->d_tag == DT_SYMINFO ||
                   dyn->d_tag == DT_INIT_ARRAY || dyn->d_tag == DT_FINI_ARRAY) {
          printf_nice(dyn->d_un.d_val, USE_FHEX);
        } else if (dyn->d_tag == DT_GNU_PRELINKED) {
          printf_nice(dyn->d_un.d_val, USE_TIMEDATE);
        } else if (dyn->d_tag == DT_GNU_HASH) {
          printf_nice(dyn->d_un.d_val, USE_FHEX);
        } else if (dyn->d_tag >= DT_VERSYM && dyn->d_tag <= DT_VERNEEDNUM) {
          printf_nice(dyn->d_un.d_val, USE_FHEX);
        }
        printf_eol();
        ++dyn;
      }
      printf_eol();
    }
  }

  return 0;
}

static int dump_relocsrel64(const pbuffer_t p, const poptions_t o, Elf64_Shdr *shdr) {
  printf(" Offset       Info         Type                 Symbol's Value   Symbol's Name\n");

  size_t cnt = shdr->sh_size / shdr->sh_entsize;

  Elf64_Rel *rr = get64byshdr(p, shdr);
  if (rr) {
    for (size_t j = 0; j < cnt; j++) {
      Elf64_Rel *r = rr + j;

      printf_nice(r->r_offset, USE_LHEX48);
      printf_nice(r->r_info, USE_LHEX48);
      printf(" %-20s", get_reltype(r));
      // TBD
      printf_eol();
    }
  }

  return 0;
}

static int dump_relocsrela64(const pbuffer_t p, const poptions_t o, Elf64_Shdr *shdr) {
  MALLOCA(Elf64_Word, vnames, 1024);
  make_versionnames64(p, vnames, NELEMENTS(vnames));

  printf(" Offset       Info         Type                Symbol's Value   Symbol's Name + Addend\n");

  size_t cnt = shdr->sh_size / shdr->sh_entsize;

  Elf64_Shdr *dshdr = get_shdr64byindex(p, shdr->sh_link);

  Elf64_Rela *rr = get64byshdr(p, shdr);
  if (rr) {
    for (size_t j = 0; j < cnt; ++j) {
      Elf64_Rela *r = rr + j;

      printf_nice(r->r_offset, USE_LHEX48);
      printf_nice(r->r_info, USE_LHEX48);
      printf_text(get_relatype(r), USE_LT | USE_SPACE | SET_PAD(20));

// TBD
      switch (r->r_info & 0xffff) {
      case R_X86_64_NONE:
        break;
      case R_X86_64_8:
      case R_X86_64_16:
      case R_X86_64_32:
      case R_X86_64_32S:
      case R_X86_64_64:        // S + A
        printf_nice(r->r_info & 0xffff, USE_WARNING);
        break;
      case R_X86_64_RELATIVE:  // B + A
        printf("                 ");
        printf_nice(r->r_addend, USE_LHEX);
        break;
      case R_X86_64_GLOB_DAT:
      case R_X86_64_JUMP_SLOT: // S
        printf_nice(r->r_addend, USE_LHEX64);

        if (dshdr) {
          Elf64_Off  k = (r->r_info >> 32) & 0xffff;
          Elf64_Sym *s = getp(p, dshdr->sh_offset + (k * dshdr->sh_entsize), dshdr->sh_entsize);
          if (s) {
            printf_text(get_name64byoffset(p, dshdr->sh_link, s->st_name), USE_LT | USE_SPACE);

            Elf64_Shdr *vshdr = get_shdr64bytype(p, SHT_GNU_versym);
            if (vshdr) {
              Elf64_Versym *vs = getp(p, vshdr->sh_offset + (k * vshdr->sh_entsize), vshdr->sh_entsize);
              if (vs) {
                *vs = *vs & VERSYM_VERSION;
                if (*vs && *vs < NELEMENTS(vnames)) {
                  const char* namevs = get_name64byoffset(p, vnames[0], vnames[*vs]);
                  if (namevs) {
                    printf_text(namevs, USE_LT | USE_AT);
                  }
                }
              }
            }
          }
        }

        printf_text("+", USE_SPACE);
        printf_nice(r->r_addend, USE_DEC);
        break;
      case R_X86_64_PC8:
      case R_X86_64_PC16:
      case R_X86_64_PC32:
      case R_X86_64_PC64:      // S + A - P
//        break;
      case R_X86_64_GOT32:     // G + A
//        break;
      case R_X86_64_PLT32:     // L + A - P
//        break;
      case R_X86_64_SIZE32:
      case R_X86_64_SIZE64:    // Z + A
//        break;
      case R_X86_64_GOTPC32:   // GOT + A - P
//        break;
      case R_X86_64_GOTPCREL:  // G + GOT + A - P
//        break;
      case R_X86_64_GOTOFF64:  // S + A - GOT
//        break;
      case R_X86_64_COPY:
        printf_nice(r->r_info & 0xffff, USE_WARNING);
        break;
      default:
        printf_nice(r->r_info & 0xffff, USE_UNKNOWN);
        break;
      }

      printf_eol();
    }
  }

  return 0;
}

static int dump_relocsrelr64(const pbuffer_t p, const poptions_t o, Elf64_Shdr *shdr) {
  printf(" Offset       Info         Type                 Symbol's Value   Symbol's Name\n");
  // TBD
  printf_eol();

  return 0;
}

static int dump_relocs64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    Elf64_Shdr *shdr = get_shdr64byindex(p, i);
    if (shdr) {
      if (SHT_RELA == shdr->sh_type || SHT_REL == shdr->sh_type || SHT_RELR == shdr->sh_type) {
        size_t cnt = shdr->sh_size / shdr->sh_entsize;

        printf_text("Relocation section", USE_LT);
        printf_text(get_secname64byindex(p, i), USE_LT | USE_SPACE | USE_SQ);
        printf_text("at offset", USE_LT | USE_SPACE);
        printf_nice(shdr->sh_offset, USE_FHEX16);
        printf_text("contains", USE_LT | USE_SPACE);
        printf_nice(cnt, USE_DEC);
        printf_text(1 == cnt ? "entry" : "entries", USE_LT | USE_SPACE | USE_COLON | USE_EOL);

        if (SHT_REL == shdr->sh_type) {
          dump_relocsrel64(p, o, shdr);
        } else if (SHT_RELA == shdr->sh_type) {
          dump_relocsrela64(p, o, shdr);
        } else if (SHT_RELR == shdr->sh_type) {
          dump_relocsrelr64(p, o, shdr);
        }
        printf_eol();
      }
    }
  }

  return 0;
}

static int dump_unwind64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  printf("%s: WARNING: The decoding of unwind sections for machine type %s is not currently supported.\n\n",
    o->prgname, get_ehdrmachine64(ehdr));

  return 0;
}

static int dump_symbols64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  MALLOCA(Elf64_Word, vnames, 1024);
  make_versionnames64(p, vnames, NELEMENTS(vnames));

  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    Elf64_Shdr *shdr = get_shdr64byindex(p, i);
    if (shdr) {
      if (SHT_SYMTAB == shdr->sh_type || SHT_DYNSYM == shdr->sh_type) {
        size_t cnt = shdr->sh_size / shdr->sh_entsize;

        printf_text("Symbol table", USE_LT);
        printf(get_secname64byindex(p, i), USE_LT | USE_SQ | USE_SPACE);
        printf_text("at offset", USE_SPACE);
        printf_nice(shdr->sh_offset, USE_FHEX16);
        printf_text("contains", USE_SPACE);
        printf_nice(cnt, USE_DEC);
        printf_text(1 == cnt ? "entry" : "entries", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
        printf_text("   Num: Value             Size Type    Bind   Vis      Ndx Name", USE_LT | USE_EOL);

        Elf64_Sym *ss = get64byshdr(p, shdr);
        if (ss) {
          for (size_t j = 0; j < cnt; ++j) {
            Elf64_Sym *s = ss + j;
            printf_nice(j, USE_DEC5 | USE_COLON);
            printf_nice(s->st_value, USE_LHEX64);
            printf_nice(s->st_size, USE_DEC5);
            printf(" %-7s", get_symboltype64(ELF_ST_TYPE(s->st_info)));
            printf(" %-6s", get_symbolbinding64(ELF_ST_BIND(s->st_info)));

            unsigned int vis = ELF_ST_VISIBILITY(s->st_other);
            printf_text(get_symbolvisibility64(vis), USE_LT | USE_SPACE | SET_PAD(9));
            printf_text(get_symbolindex64(s->st_shndx), USE_LT | USE_SPACE);

            const char* name = get_name64byoffset(p, shdr->sh_link, s->st_name);
            if (name && 0 != name[0]) {
              printf_text(name, USE_LT | USE_SPACE);

              if (SHT_DYNSYM == shdr->sh_type) {
                Elf64_Shdr *vshdr = get_shdr64bytype(p, SHT_GNU_versym);
                if (vshdr) {
                  Elf64_Versym *vs = getp(p, vshdr->sh_offset + (j * vshdr->sh_entsize), vshdr->sh_entsize);
                  if (vs) {
                    *vs = *vs & VERSYM_VERSION;
                    if (*vs && *vs < NELEMENTS(vnames)) {
                      const char* namevs = get_name64byoffset(p, vnames[0], vnames[*vs]);
                      if (namevs) {
                        if (SHN_UNDEF == s->st_shndx)    printf("@%s (%d)", namevs, *vs);
                        else if (STV_HIDDEN == vis)      printf("@%s", namevs);
                        else                             printf("@@%s", namevs);
                      }
                    }
                  }
                }
              }
            }
            printf_eol();
          }
        }
        printf_eol();
      }
    }
  }

  return 0;
}

static int dump_gnuhash64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr, Elf64_Shdr *shdr) {
  Elf32_Word *pb = get64byshdr(p, shdr);

  if (pb) {
    Elf32_Word nbucket  = pb[0];
    Elf32_Word symbias  = pb[1];
    Elf32_Word sbitmask = is32(p) ? pb[2] : 2 * pb[2];
    //Elf32_Word shift    = pb[3];
    Elf32_Word *bitmask = &pb[4];
    Elf32_Word *bucket  = &pb[4 + sbitmask];
    Elf32_Word *chain   = &pb[4 + sbitmask + nbucket];

    MALLOCA(uint32_t, size, nbucket);

    /* compute distribution of chain lengths. */
    uint_fast32_t msize = 0;
    uint_fast32_t nsyms = 0;
    for (Elf32_Word k = 0; k < nbucket; ++k) {
      if (bucket[k] != 0) {
        Elf32_Word x = bucket[k] - symbias;
        do {
          ++nsyms;
          if (msize < ++size[k]) ++msize;
        } while ((chain[x++] & 1) == 0);
      }
    }

    /* count bits in bitmask. */
    uint_fast32_t nbits = 0;
    for (Elf32_Word k = 0; k < sbitmask; ++k) {
      uint_fast32_t x = bitmask[k];

      x = (x & 0x55555555) + ((x >> 1) & 0x55555555);
      x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
      x = (x & 0x0f0f0f0f) + ((x >> 4) & 0x0f0f0f0f);
      x = (x & 0x00ff00ff) + ((x >> 8) & 0x00ff00ff);
      nbits += (x & 0x0000ffff) + ((x >> 16) & 0x0000ffff);
    }

    MALLOCA(uint32_t, counts, msize + 1);

    for (Elf32_Word k = 0; k < nbucket; ++k) {
      ++counts[size[k]];
    }

    printf_text("Histogram for", USE_LT);
    printf_text(get_secname64byshdr(p, shdr), USE_LT | USE_DRTB | USE_SPACE);
    printf_text("bucket list length (total of", USE_LT | USE_SPACE);
    printf_nice(nbucket, USE_DEC);
    printf_text(nbucket == 1 ? "bucket)" : "buckets)", USE_LT | USE_SPACE | USE_COLON | USE_EOL);

    printf_text(" Length Number       % of total  Coverage", USE_LT | USE_EOL);

    printf("     0 ");
    printf_nice(counts[0], USE_DEC5);
    printf("         ");
    printf_nice((counts[0] * 1000.0) / nbucket, USE_PERCENT);
    printf_eol();

    uint64_t nzeros = 0;
    for (Elf32_Word i = 1; i < nbucket; ++i) {
      nzeros += counts[i] * i;

      printf_nice(i, USE_DEC5);
      printf(" ");
      printf_nice(counts[i], USE_DEC5);
      printf("         ");
      printf_nice((counts[i] * 1000.0) / nbucket, USE_PERCENT);
      printf("    ");
      printf_nice((nzeros * 1000.0) / nsyms, USE_PERCENT);
      printf_eol();
    }

    printf_eol();
  }

  return 0;
}

static int dump_histogram64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    Elf64_Shdr *shdr = get_shdr64byindex(p, i);
    if (shdr && SHT_GNU_HASH == shdr->sh_type) {
      dump_gnuhash64(p, o, ehdr, shdr);
    } if (shdr && SHT_HASH == shdr->sh_type) {
      // TBD
    }
  }

  return 0;
}

static int dump_versionsym64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr, Elf64_Shdr *shdr) {
  MALLOCA(Elf64_Word, vnames, 1024);
  make_versionnames64(p, vnames, NELEMENTS(vnames));

  size_t cnt = shdr->sh_size / shdr->sh_entsize;
  printf_text("Version symbols section", USE_LT);
  printf_text(get_secname64byshdr(p, shdr), USE_LT | USE_SQ | USE_SPACE);
  printf_text("contains", USE_SPACE);
  printf_nice(cnt, USE_DEC);
  printf_text(1 == cnt ? "entry" : "entries", USE_LT | USE_SPACE | USE_COLON | USE_EOL);

  printf_text("Addr", USE_SPACE | USE_COLON);
  printf_nice(shdr->sh_addr, USE_FHEX64);
  printf_text("Offset", USE_SPACE | USE_COLON);
  printf_nice(shdr->sh_offset, USE_FHEX24);
  printf_text("Link", USE_SPACE | USE_COLON);
  printf_nice(shdr->sh_link, USE_DEC);
  printf_text(get_secname64byindex(p, shdr->sh_link), USE_LT | USE_RB | USE_SPACE);

  for (size_t j = 0; j < cnt; ++j) {
    Elf64_Versym *vs = getp(p, shdr->sh_offset + (j * shdr->sh_entsize), shdr->sh_entsize);
    if (vs) {
      if (j % 4 == 0) {
        printf_eol();
        printf_nice(j, USE_LHEX16 | USE_COLON);
        printf(" ");
      }

      int n = 0;
      if (0 == *vs)        n += printf_text("   0  (*local*)", USE_LT | SET_PAD(20));
      else if (1 == *vs)   n += printf_text("   1  (*global*)", USE_LT | SET_PAD(20));
      else {
        n += printf("%4x%c", *vs & VERSYM_VERSION, *vs & VERSYM_HIDDEN ? 'h' : ' ');
        if (vnames[*vs & VERSYM_VERSION] && (*vs & VERSYM_VERSION) < NELEMENTS(vnames)) {
          n += printf_text(get_name64byoffset(p, vnames[0], vnames[*vs & VERSYM_VERSION]), USE_LT | USE_RB | USE_SPACE | SET_PAD(MAX(0, 20 - n)));
        } else {
          n += printf_text("???", USE_SPACE | SET_PAD(MAX(0, 20 - n)));
        }
      }
    }
  }

  if (cnt) printf_eol();
  printf_eol();

  return 0;
}

static int dump_versionneed64(const pbuffer_t p, const poptions_t o, Elf64_Shdr *shdr) {
  printf_text("Version needs section", USE_LT);
  printf_text(get_secname64byshdr(p, shdr),  USE_LT | USE_SQ | USE_SPACE);
  printf_text("contains", USE_SPACE);
  printf_nice(shdr->sh_info, USE_DEC);
  printf_text(1 == shdr->sh_info ? "entry" : "entries", USE_LT | USE_SPACE | USE_COLON | USE_EOL);

  printf_text("Addr", USE_SPACE | USE_COLON);
  printf_nice(shdr->sh_addr, USE_FHEX64);
  printf_text("Offset", USE_SPACE | USE_COLON);
  printf_nice(shdr->sh_offset, USE_FHEX24);
  printf_text("Link", USE_SPACE | USE_COLON);
  printf_nice(shdr->sh_link, USE_DEC);
  printf_text(get_secname64byindex(p, shdr->sh_link), USE_LT | USE_RB | USE_SPACE | USE_EOL);

  Elf64_Word offset = 0;
  for (Elf64_Word j = 0; j < shdr->sh_info; ++j) {
    Elf64_Verneed *vn = getp(p, shdr->sh_offset, sizeof(Elf64_Verneed));
    if (vn) {
      printf_nice(offset, USE_LHEX24 | USE_COLON);
      printf_text("Version", USE_SPACE | USE_COLON);
      printf_nice(vn->vn_version, USE_DEC);
      printf_text("File", USE_SPACE | USE_COLON);
      printf_text(get_name64byoffset(p, shdr->sh_link, vn->vn_file), USE_LT | USE_SPACE);
      printf_text("Cnt", USE_SPACE | USE_COLON);
      printf_nice(vn->vn_cnt, USE_DEC | USE_EOL);

      Elf64_Word xoffset = offset + vn->vn_aux;
      for (Elf64_Half k = 0; k < vn->vn_cnt; ++k) {
        Elf64_Vernaux *va = getp(p, shdr->sh_offset + xoffset, sizeof(Elf64_Vernaux));
        if (va) {
          printf_nice(xoffset, USE_FHEX16 | USE_COLON);
          printf_text("Name", USE_TAB | USE_COLON);
          printf_text(get_name64byoffset(p, shdr->sh_link, va->vna_name), USE_LT | USE_SPACE | SET_PAD(14));
          printf_text("Flags", USE_SPACE | USE_COLON);
          printf_masknone(zVNA_FLAGS, va->vna_flags, USE_LT);
          printf_text("Version", USE_TAB | USE_COLON);
          printf_nice(va->vna_other, USE_DEC | USE_EOL);

	  xoffset += va->vna_next;
        }
      }
    }

    offset += vn->vn_next;
  }

  printf_eol();

  return 0;
}

static int dump_version64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    Elf64_Shdr *shdr = get_shdr64byindex(p, i);
    if (shdr) {
      if (SHT_GNU_versym == shdr->sh_type) {
        dump_versionsym64(p, o, ehdr, shdr);
      } else if (SHT_GNU_verneed == shdr->sh_type) {
        dump_versionneed64(p, o, shdr);
      }
    }
  }

  return 0;
}

static int dump_actions64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  paction_t x = o->actions;
  while (x) {
    Elf64_Shdr* shdr = get_shdr64byname(p, x->secname);
    if (shdr) {
      if (ACT_HEXDUMP == x->action) {
        printf("Hex dump of section '%s':\n", x->secname);

        if (0 != shdr->sh_size && shdr->sh_type != SHT_NOBITS) {
          printf_data(getp(p, shdr->sh_offset, shdr->sh_size), shdr->sh_size, shdr->sh_addr, USE_HEXDUMP);
        } else {
          printf("%s: WARNING: section '%s' has no data to dump!\n", o->prgname, x->secname);
        }
      } else if (ACT_STRDUMP == x->action) {
        printf("String dump of section '%s':\n", x->secname);

        if (0 != shdr->sh_size && shdr->sh_type != SHT_NOBITS) {
          printf_data(getp(p, shdr->sh_offset, shdr->sh_size), shdr->sh_size, shdr->sh_addr, USE_STRDUMP);
        } else {
          printf("%s: WARNING: section '%s' has no data to dump!\n", o->prgname, x->secname);
        }
      }

      printf_eol();
    } else {
      printf("%s: WARNING: section '%s' was not dumped because it does not exist!\n", o->prgname, x->secname);
    }

    x = x->actions;
  }

  return 0;
}

static int dump_notes64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  if (ET_CORE != ehdr->e_type) {
    for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
      Elf64_Nhdr *nhdr = get_nhdr64byindex(p, i);
      if (nhdr) {
        printf("Displaying notes found in: %s\n", get_secname64byindex(p, i));
        printf("  Owner                Data size        Description\n");
        printf("  %-20s 0x%08x       %-10s\n", get_nhdrname64byindex(p, i), nhdr->n_descsz, get_nhdrtype64(p, nhdr));

        const char* cc = get_nhdrdesc64byindex(p, i);
        if (NT_GNU_BUILD_ID == nhdr->n_type) {
          printf_text("Build ID", USE_LT | USE_TAB | USE_COLON);
          printf_data(cc, nhdr->n_descsz, 0, USE_HEX);
          printf_eol();
        } else if (NT_GNU_GOLD_VERSION == nhdr->n_type) {
          printf_text("Version", USE_LT | USE_TAB | USE_COLON);
          printf_data(cc, nhdr->n_descsz, 0, USE_STR);
          printf_eol();
        } else if (NT_GNU_HWCAP == nhdr->n_type) {
          printf("  Hardware Capabilities: ");
          // TBD
        } else if (NT_GNU_PROPERTY_TYPE_0 == nhdr->n_type) {
          printf_text("Properties", USE_LT | USE_TAB | USE_COLON);

          for (Elf64_Word i = 0; i < nhdr->n_descsz; i += 8) {
            if ((nhdr->n_descsz - i) < 8) {
              printf_nice(nhdr->n_descsz, USE_CORRUPT);
              break;
            } else {
              unsigned int x = getLE(cc + i, 4);
              unsigned int datasz = getLE(cc + i + 4, 4);
              if ((nhdr->n_descsz - i + 1) < datasz) {
                printf("<corrupt type (%#x) datasz: %#x>\n", x, datasz);
                break;
              } else {
                if (x >= GNU_PROPERTY_LOPROC && x <= GNU_PROPERTY_HIPROC) {
                  if (ehdr->e_machine == EM_X86_64 || ehdr->e_machine == EM_IAMCU || ehdr->e_machine == EM_386) {
                    if (4 != datasz) {
                      printf_pick(zGNUPROPERTY, x, USE_LT | USE_SPACE);
                      printf_nice(datasz, USE_CORRUPT | USE_COLON);
                    } else {
                      printf_pick(zGNUPROPERTY, x, USE_LT | USE_SPACE | USE_COLON);
                    }

                    if (x == GNU_PROPERTY_X86_FEATURE_1_AND) {
                      printf_mask(zGNUPROPERTY_X86_FEATURE_1_AND, getLE(cc + i + 8, 4), USE_LT);
                    }
                  }
                }
              }
            }
          }
          printf_eol();
        } else if (NT_GNU_ABI_TAG == nhdr->n_type) {
          printf("  OS: %s, ABI: %ld.%ld.%ld\n",
            get_gnuabitab64(getLE(cc, 4)), getLE(cc + 4, 4), getLE(cc + 8, 4), getLE(cc + 12, 4));
        } else {
          printf("  Description Data: ");
          printf_data(cc, nhdr->n_descsz, 0, USE_HEX);
          printf_eol();
        }
        printf_eol();
      }
    }
  }

  return 0;
}

int readelf(const pbuffer_t p, const poptions_t o) {
  if (isELF(p)) {
    if (o->action & OPTREADELF_FILEHEADER) {
      dump_elfheader(p, o);
    }

    if (is32(p)) {
      Elf32_Ehdr *ehdr = get_ehdr32(p);
      if (ehdr) {
        if (o->action & OPTREADELF_FILEHEADER)       dump_fileheader32(p, o, ehdr);
        if (o->action & OPTREADELF_SECTIONHEADERS)   dump_sectionheaders32(p, o, ehdr);
        if (o->action & OPTREADELF_SECTIONGROUPS)    dump_sectiongroups32(p, o, ehdr);
      }
    } else if (is64(p)) {
      Elf64_Ehdr *ehdr = get_ehdr64(p);
      if (ehdr) {
        if (o->action & OPTREADELF_FILEHEADER)       dump_fileheader64(p, o, ehdr);
        if (o->action & OPTREADELF_SECTIONHEADERS)   dump_sectionheaders64(p, o, ehdr);
        if (o->action & OPTREADELF_SECTIONGROUPS)    dump_sectiongroups64(p, o, ehdr);
        if (o->action & OPTREADELF_PROGRAMHEADERS)   dump_programheaders64(p, o, ehdr);
        if (o->action & OPTREADELF_DYNAMIC)          dump_dynamic64(p, o, ehdr);
        if (o->action & OPTREADELF_RELOCS)           dump_relocs64(p, o, ehdr);
        if (o->action & OPTREADELF_UNWIND)           dump_unwind64(p, o, ehdr);
        if (o->action & OPTREADELF_SYMBOLS)          dump_symbols64(p, o, ehdr);
        if (o->action & OPTREADELF_HISTOGRAM)        dump_histogram64(p, o, ehdr);
        if (o->action & OPTREADELF_VERSION)          dump_version64(p, o, ehdr);
        if (o->actions)                              dump_actions64(p, o, ehdr);
        if (o->action & OPTREADELF_NOTES)            dump_notes64(p, o, ehdr);
      }
    }
  } else {
    printf("%s: ERROR: not an ELF file - it has the wrong magic bytes at the start.\n", o->prgname);
  }

  return 0;
}

