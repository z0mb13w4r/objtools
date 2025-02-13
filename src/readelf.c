#include <string.h>

#include "printf.h"
#include "readelf.h"
#include "elfcode.h"
#include "objutils.h"

#include "static/dt_flags.ci"
#include "static/dt_flags_1.ci"
#include "static/dt_posflag_1.ci"
#include "static/gnu_property_x86_feature_1_and.ci"
#include "static/nhdrtype.ci"
#include "static/vna_flags.ci"

static convert_t DYNTAG[] = {
  {"NULL",            DT_NULL},
  {"NEEDED",          DT_NEEDED},
  {"PLTRELSZ",        DT_PLTRELSZ},
  {"PLTGOT",          DT_PLTGOT},
  {"HASH",            DT_HASH},
  {"STRTAB",          DT_STRTAB},
  {"SYMTAB",          DT_SYMTAB},
  {"RELA",            DT_RELA},
  {"RELASZ",          DT_RELASZ},
  {"RELAENT",         DT_RELAENT},
  {"STRSZ",           DT_STRSZ},
  {"SYMENT",          DT_SYMENT},
  {"INIT",            DT_INIT},
  {"FINI",            DT_FINI},
  {"SONAME",          DT_SONAME},
  {"RPATH",           DT_RPATH},
  {"SYMBOLIC",        DT_SYMBOLIC},
  {"REL",             DT_REL},
  {"RELSZ",           DT_RELSZ},
  {"RELENT",          DT_RELENT},
  {"PLTREL",          DT_PLTREL},
  {"DEBUG",           DT_DEBUG},
  {"TEXTREL",         DT_TEXTREL},
  {"JMPREL",          DT_JMPREL},
  {"BIND_NOW",        DT_BIND_NOW},
  {"INIT_ARRAY",      DT_INIT_ARRAY},
  {"FINI_ARRAY",      DT_FINI_ARRAY},
  {"INIT_ARRAYSZ",    DT_INIT_ARRAYSZ},
  {"FINI_ARRAYSZ",    DT_FINI_ARRAYSZ},
  {"RUNPATH",         DT_RUNPATH},
  {"FLAGS",           DT_FLAGS},
  {"PREINIT_ARRAY",   DT_PREINIT_ARRAY},
  {"PREINIT_ARRAYSZ", DT_PREINIT_ARRAYSZ},
  {"SYMTAB_SHNDX",    DT_SYMTAB_SHNDX},
  {"CHECKSUM",        DT_CHECKSUM},
  {"PLTPADSZ",        DT_PLTPADSZ},
  {"MOVEENT",         DT_MOVEENT},
  {"MOVESZ",          DT_MOVESZ},
  {"POSFLAG_1",       DT_POSFLAG_1},
  {"SYMINSZ",         DT_SYMINSZ},
  {"SYMINENT",        DT_SYMINENT},
  {"ADDRRNGLO",       DT_ADDRRNGLO},
  {"CONFIG",          DT_CONFIG},
  {"DEPAUDIT",        DT_DEPAUDIT},
  {"AUDIT",           DT_AUDIT},
  {"PLTPAD",          DT_PLTPAD},
  {"MOVETAB",         DT_MOVETAB},
  {"SYMINFO",         DT_SYMINFO},
  {"VERSYM",          DT_VERSYM},
  {"TLSDESC_GOT",     DT_TLSDESC_GOT},
  {"TLSDESC_PLT",     DT_TLSDESC_PLT},
  {"RELACOUNT",       DT_RELACOUNT},
  {"RELCOUNT",        DT_RELCOUNT},
  {"FLAGS_1",         DT_FLAGS_1},
  {"VERDEF",          DT_VERDEF},
  {"VERDEFNUM",       DT_VERDEFNUM},
  {"VERNEED",         DT_VERNEED},
  {"VERNEEDNUM",      DT_VERNEEDNUM},
  {"AUXILIARY",       DT_AUXILIARY},
  {"FILTER",          DT_FILTER},
  {"GNU_PRELINKED",   DT_GNU_PRELINKED},
  {"GNU_CONFLICT",    DT_GNU_CONFLICT},
  {"GNU_CONFLICTSZ",  DT_GNU_CONFLICTSZ},
  {"GNU_LIBLIST",     DT_GNU_LIBLIST},
  {"GNU_LIBLISTSZ",   DT_GNU_LIBLISTSZ},
  {"GNU_HASH",        DT_GNU_HASH},
  {0, 0}
};

static convert_t EHDRTYPE[] = {
  {"NONE (No file type)",          ET_NONE},
  {"REL (Relocatable file)",       ET_REL},
  {"EXEC (Executable file)",       ET_EXEC},
  {"DYN (Shared object file)",     ET_DYN},
  {0, 0}
};

static convert_t EHDRMACHINE[] = {
  {"None",                         EM_NONE},
  {"AMD x86-64",                   EM_X86_64},
  {0, 0}
};

static convert_t EHDROSABI[] = {
  {"UNIX - System V",              ELFOSABI_NONE},
  {"UNIX - HP-UX",                 ELFOSABI_HPUX},
  {"UNIX - NetBSD",                ELFOSABI_NETBSD},
  {"UNIX - GNU",                   ELFOSABI_GNU},
  {"UNIX - Solaris",               ELFOSABI_SOLARIS},
  {"UNIX - AIX",                   ELFOSABI_AIX},
  {"UNIX - IRIX",                  ELFOSABI_IRIX},
  {"UNIX - FreeBSD",               ELFOSABI_FREEBSD},
  {"UNIX - TRU64",                 ELFOSABI_TRU64},
  {"Novell - Modesto",             ELFOSABI_MODESTO},
  {"UNIX - OpenBSD",               ELFOSABI_OPENBSD},
  {"VMS - OpenVMS",                ELFOSABI_OPENVMS},
  {"HP - Non-Stop Kernel",         ELFOSABI_NSK},
  {"AROS",                         ELFOSABI_AROS},
  {"FenixOS",                      ELFOSABI_FENIXOS},
  {"Nuxi CloudABI",                ELFOSABI_CLOUDABI},
  {"Stratus Technologies OpenVOS", ELFOSABI_OPENVOS},
  {0, 0}
};

static convert_t EHDRFLAGS[] = {
  {"W",                            SHF_WRITE},
  {"A",                            SHF_ALLOC},
  {"X",                            SHF_EXECINSTR},
  {"M",                            SHF_MERGE},
  {"S",                            SHF_STRINGS},
  {"I",                            SHF_INFO_LINK},
  {"L",                            SHF_LINK_ORDER},
  {"O",                            SHF_OS_NONCONFORMING},
  {"G",                            SHF_GROUP},
  {"T",                            SHF_TLS},
  {"E",                            SHF_EXCLUDE},
  {"C",                            SHF_COMPRESSED},
  {0, 0}
};

static convert_t GNUABITAB[] = {
  {"Linux",                        GNU_ABI_TAG_LINUX},
  {"Hurd",                         GNU_ABI_TAG_HURD},
  {"Solaris",                      GNU_ABI_TAG_SOLARIS},
  {"FreeBSD",                      GNU_ABI_TAG_FREEBSD},
  {"NetBSD",                       GNU_ABI_TAG_NETBSD},
  {"Syllable",                     GNU_ABI_TAG_SYLLABLE},
  {"NaCl",                         GNU_ABI_TAG_NACL},
  {0, 0},
};

static convert_t GNUPROPERTY[] = {
  {"x86 ISA used: ",               GNU_PROPERTY_X86_ISA_1_USED},
  {"x86 ISA needed: ",             GNU_PROPERTY_X86_ISA_1_NEEDED},
  {"x86 feature: ",                GNU_PROPERTY_X86_FEATURE_1_AND},
  {"x86 feature used: ",           GNU_PROPERTY_X86_FEATURE_2_USED},
  {"x86 feature needed: ",         GNU_PROPERTY_X86_FEATURE_2_NEEDED},
  {"x86 ISA used: ",               GNU_PROPERTY_X86_COMPAT_ISA_1_USED},
  {"x86 ISA needed: ",             GNU_PROPERTY_X86_COMPAT_ISA_1_NEEDED},
  {"x86 ISA used: ",               GNU_PROPERTY_X86_COMPAT_2_ISA_1_USED},
  {"x86 ISA needed: ",             GNU_PROPERTY_X86_COMPAT_2_ISA_1_NEEDED},
  {0, 0}
};

static convert_t RELTYPE[] = {
  {"R_X86_64_NONE",                R_X86_64_NONE},
  {"R_X86_64_64",                  R_X86_64_64},
  {"R_X86_64_PC32",                R_X86_64_PC32},
  {"R_X86_64_GOT32",               R_X86_64_GOT32},
  {"R_X86_64_PLT32",               R_X86_64_PLT32},
  {"R_X86_64_COPY",                R_X86_64_COPY},
  {"R_X86_64_GLOB_DAT",            R_X86_64_GLOB_DAT},
  {"R_X86_64_JUMP_SLOT",           R_X86_64_JUMP_SLOT},
  {"R_X86_64_RELATIVE",            R_X86_64_RELATIVE},
  {"R_X86_64_GOTPCREL",            R_X86_64_GOTPCREL},
  {"R_X86_64_32",                  R_X86_64_32},
  {"R_X86_64_32S",                 R_X86_64_32S},
  {"R_X86_64_16",                  R_X86_64_16},
  {"R_X86_64_PC16",                R_X86_64_PC16},
  {"R_X86_64_8",                   R_X86_64_8},
  {"R_X86_64_PC8",                 R_X86_64_PC8},
  {"R_X86_64_PC64",                R_X86_64_PC64},
  {"R_X86_64_GOTOFF64",            R_X86_64_GOTOFF64},
  {"R_X86_64_GOTPC32",             R_X86_64_GOTPC32},
  {"R_X86_64_SIZE32",              R_X86_64_SIZE32},
  {"R_X86_64_SIZE64",              R_X86_64_SIZE64},
  {0, 0}
};

static convert_t SHDRTYPE[] = {
  {"NULL",                         SHT_NULL},
  {"PROGBITS",                     SHT_PROGBITS},
  {"SYMTAB",                       SHT_SYMTAB},
  {"STRTAB",                       SHT_STRTAB},
  {"RELA",                         SHT_RELA},
  {"RELR",                         SHT_RELR},
  {"HASH",                         SHT_HASH},
  {"DYNAMIC",                      SHT_DYNAMIC},
  {"NOTE",                         SHT_NOTE},
  {"NOBITS",                       SHT_NOBITS},
  {"REL",                          SHT_REL},
  {"SHLIB",                        SHT_SHLIB},
  {"DYNSYM",                       SHT_DYNSYM},
  {"INIT_ARRAY",                   SHT_INIT_ARRAY},
  {"FINI_ARRAY",                   SHT_FINI_ARRAY},
  {"PREINIT_ARRAY",                SHT_PREINIT_ARRAY},
  {"GNU_HASH",                     SHT_GNU_HASH},
  {"GROUP",                        SHT_GROUP},
  {"SYMTAB SECTION INDICES",       SHT_SYMTAB_SHNDX},
  {"VERDEF",                       SHT_GNU_verdef},
  {"VERNEED",                      SHT_GNU_verneed},
  {"VERSYM",                       SHT_GNU_versym},
  {0, 0}
};

static convert_t PHDRTYPE[] = {
  {"NULL",                         PT_NULL},
  {"LOAD",                         PT_LOAD},
  {"DYNAMIC",                      PT_DYNAMIC},
  {"INTERP",                       PT_INTERP},
  {"NOTE",                         PT_NOTE},
  {"SHLIB",                        PT_SHLIB},
  {"PHDR",                         PT_PHDR},
  {"TLS",                          PT_TLS},
  {"GNU_EH_FRAME",                 PT_GNU_EH_FRAME},
  {"GNU_STACK",                    PT_GNU_STACK},
  {"GNU_RELRO",                    PT_GNU_RELRO},
  {"GNU_PROPERTY",                 PT_GNU_PROPERTY},
  {"GNU_SFRAME",                   PT_GNU_SFRAME},
  {"OPENBSD_MUTABLE",              PT_OPENBSD_MUTABLE},
  {"OPENBSD_RANDOMIZE",            PT_OPENBSD_RANDOMIZE},
  {"OPENBSD_WXNEEDED",             PT_OPENBSD_WXNEEDED},
  {"OPENBSD_BOOTDATA",             PT_OPENBSD_BOOTDATA},
  {0, 0}
};

static const char* get_gnuabitab(const int y) {
  static char buff[32];

  for (pconvert_t x = GNUABITAB; 0 != x->text; ++x) {
    if (x->type == y) {
      return x->text;
    }
  }

  snprintf(buff, sizeof (buff), "<unknown: %x>", y);
  return buff;
}

static const char *get_gnuproperty64(const int y) {
  static char buff[32];

  for (pconvert_t x = GNUPROPERTY; 0 != x->text; ++x) {
    if (x->type == y) {
      return x->text;
    }
  }

  snprintf(buff, sizeof (buff), "<unknown: %x>", y);
  return buff;
}

static const char* get_dyntag64(const int y) {
  static char buff[32];

  for (pconvert_t x = DYNTAG; 0 != x->text; ++x) {
    if (x->type == y) {
      return x->text;
    }
  }

  snprintf(buff, sizeof (buff), "<unknown: %x>", y);
  return buff;
}

static const char* get_ehdrtype64(Elf64_Ehdr *e) {
  static char buff[32];

  if (e) {
    for (pconvert_t x = EHDRTYPE; 0 != x->text; ++x) {
      if (x->type == e->e_type) {
        return x->text;
      }
    }

    snprintf(buff, sizeof (buff), "<unknown: %x>", e->e_type);
    return buff;
  }

  return NULL;
}

static const char* get_ehdrmachine64(Elf64_Ehdr *e) {
  static char buff[32];

  if (e) {
    for (pconvert_t x = EHDRMACHINE; 0 != x->text; ++x) {
      if (x->type == e->e_machine) {
        return x->text;
      }
    }

    snprintf(buff, sizeof (buff), "<unknown: %x>", e->e_machine);
    return buff;
  }

  return NULL;
}

static const char* get_ehdrosabi(pbuffer_t p) {
  static char buff[32];

  if (p) {
    unsigned int osabi = get(p, EI_OSABI);
    for (pconvert_t x = EHDROSABI; 0 != x->text; ++x) {
      if (x->type == osabi) {
        return x->text;
      }
    }

    Elf64_Ehdr *e = get_ehdr64(p);

    if (e && osabi >= 64) {
      switch (e->e_machine) {
      case EM_AMDGPU:
        switch (osabi) {
          case ELFOSABI_AMDGPU_HSA:    return "AMD HSA";
          case ELFOSABI_AMDGPU_PAL:    return "AMD PAL";
          case ELFOSABI_AMDGPU_MESA3D: return "AMD Mesa3D";
          default:
            break;
        }
        break;

      case EM_ARM:
      switch (osabi) {
        case ELFOSABI_ARM:             return "ARM";
        case ELFOSABI_ARM_FDPIC:       return "ARM FDPIC";
        default:
          break;
      }
      break;

      case EM_MSP430:
      case EM_VISIUM:
        switch (osabi) {
          case ELFOSABI_STANDALONE:    return "Standalone App";
          default:
            break;
        }
        break;

      case EM_TI_C6000:
        switch (osabi) {
          case ELFOSABI_C6000_ELFABI:  return "Bare-metal C6000";
          case ELFOSABI_C6000_LINUX:   return "Linux C6000";
          default:
            break;
        }
        break;

      default:
        break;
      }
    }

    snprintf(buff, sizeof (buff), "<unknown: %x>", e->e_type);
    return buff;
  }

  return NULL;

}

static const char* get_shdrflags64(Elf64_Shdr *s) {
  static char buff[32];
  char *p = buff;

  if (s) {
    for (pconvert_t x = EHDRFLAGS; 0 != x->text; ++x) {
      if (x->type & s->sh_flags) {
        *p = x->text[0];
	++p;
      }
    }

    *p = '\0';

    return buff;
  }

  return NULL;
}

static const char* get_shdrtype64(Elf64_Shdr *s) {
  static char buff[32];

  if (s) {
    for (pconvert_t x = SHDRTYPE; 0 != x->text; ++x) {
      if (x->type == s->sh_type) {
        return x->text;
      }
    }

    snprintf(buff, sizeof (buff), "<unknown: %x>", s->sh_type);
    return buff;
  }

  return NULL;
}

static const char* get_reltype(Elf64_Rel *r) {
  static char buff[32];

  if (r) {
    for (pconvert_t x = RELTYPE; 0 != x->text; ++x) {
      if (x->type == (r->r_info & 0xff)) {
        return x->text;
      }
    }

    snprintf(buff, sizeof (buff), "<unknown: %lx>", r->r_info);
    return buff;
  }

  return NULL;
}

static const char* get_relatype(Elf64_Rela *r) {
  static char buff[32];

  if (r) {
    for (pconvert_t x = RELTYPE; 0 != x->text; ++x) {
      if (x->type == (r->r_info & 0xff)) {
        return x->text;
      }
    }

    snprintf(buff, sizeof (buff), "<unknown: %lx>", r->r_info);
    return buff;
  }

  return NULL;
}

static const char* get_nhdrtype64(const pbuffer_t p, Elf64_Nhdr *n) {
  if (n) {
    Elf32_Ehdr *e = get_ehdr32(p);
    if (e) {
      if (ET_CORE == e->e_type) {
        return get_string(zNHDRTYPECORE, n->n_type);
      } else {
        return get_string(zNHDRTYPE, n->n_type);
      }
    }
  }

  return NULL;
}

static const char* get_phdrtype64(Elf64_Phdr *p) {
  static char buff[32];

  if (p) {
    for (pconvert_t x = PHDRTYPE; 0 != x->text; ++x) {
      if (x->type == p->p_type) {
        return x->text;
      }
    }

    snprintf(buff, sizeof (buff), "<unknown: %x>", p->p_type);
    return buff;
  }

  return NULL;
}

static int dump_elfheader(const pbuffer_t p, const poptions_t o) {
  printf("ELF HEADER:\n");

  printf("  Magic: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
            get(p,  0), get(p,  1), get(p,  2), get(p,  3),
            get(p,  4), get(p,  5), get(p,  6), get(p,  7),
            get(p,  8), get(p,  9), get(p, 10), get(p, 11),
            get(p, 12), get(p, 13), get(p, 14), get(p, 15));

  if (is32(p)) {
    printf("  Class:                             ELF32\n");
  } else if (is64(p)) {
    printf("  Class:                             ELF64\n");
  } else {
    return -1;
  }

  if (isLittleEndian(p)) {
    printf("  Data:                              2's complement, little endian\n");
  } else if (isBigEndian(p)) {
    printf("  Data:                              2's complement, big endian\n");
  } else {
    printf("  Data:                              none\n");
  }

  if (EV_CURRENT == get(p, EI_VERSION)) {
    printf("  Version:                           1 (current)\n");
  } else if (EV_NONE == get(p, EI_VERSION)) {
    printf("  Version:                           0\n");
  } else {
    printf("  Version:                           %d <unknown>\n", get(p, EI_VERSION));
  }

  printf("  OS/ABI:                            %s\n", get_ehdrosabi(p));
  printf("  ABI Version:                       %d\n", get(p, EI_ABIVERSION));

  return 0;
}

static int dump_fileheader64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  printf("  Type:                              %s\n",                    get_ehdrtype64(ehdr));
  printf("  Machine:                           %s\n",                    get_ehdrmachine64(ehdr));

  printf("  Version:                           0x%x\n",                  ehdr->e_version);
  printf("  Entry point address:               0x%04lx\n",               ehdr->e_entry);
  printf("  Start of program headers:          %lu (bytes into file)\n", ehdr->e_phoff);
  printf("  Start of section headers:          %lu (bytes into file)\n", ehdr->e_shoff);
  printf("  Flags:                             0x%x\n",                  ehdr->e_flags);

  printf("  Size of this header:               %d (bytes)\n",            ehdr->e_ehsize);
  printf("  Size of program headers:           %d (bytes)\n",            ehdr->e_phentsize);
  printf("  Number of program headers:         %d\n",                    ehdr->e_phnum);
  printf("  Size of section headers:           %d (bytes)\n",            ehdr->e_shentsize);
  printf("  Number of section headers:         %d\n",                    ehdr->e_shnum);
  printf("  Section header string table index: %d\n",                    ehdr->e_shstrndx);
  printf("\n");

  return 0;
}

static int dump_sectionheaders64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  printf("SECTION HEADERS:\n");
  printf("  [Nr] Name                 Type            Address          Off      Size     ES Flg Lk Inf  Al\n");

  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    printf("  [%2d]", i);

    Elf64_Shdr *shdr = get_shdr64byindex(p, i);
    if (shdr) {
      printf(" %-20s", get_secname64byindex(p, i));
      printf(" %-15s", get_shdrtype64(shdr));

      printf_nice(shdr->sh_addr, USE_LHEX64);
      printf_nice(shdr->sh_offset, USE_LHEX32);
      printf_nice(shdr->sh_size, USE_LHEX32);
      printf_nice(shdr->sh_entsize, USE_LHEX8);
      printf(" %3s", get_shdrflags64(shdr));
      printf(" %2u %3u ", shdr->sh_link, shdr->sh_info);
      printf_nice(shdr->sh_addralign, USE_DEC2);
    }
    printf("\n");
  }

  printf("Key to Flags:\n");
  printf("  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),\n");
  printf("  L (link order), O (extra OS processing required), G (group), T (TLS),\n");
  printf("  C (compressed), x (unknown), o (OS specific), E (exclude),\n");
  printf("  l (large), p (processor specific)\n");
  printf("\n");

  return 0;
}

static int dump_programheaders64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  printf("PROGRAM HEADERS:\n");
  printf("  Type            Offset VirtAddr           PhysAddr           FileSiz MemSiz Flg  Align\n");
  for (Elf64_Half i = 0; i < ehdr->e_phnum; ++i) {
    Elf64_Phdr *phdr = get_phdr64byindex(p, i);
    if (phdr) {
      printf ("  %-14s ", get_phdrtype64(phdr));
      printf_nice(phdr->p_offset, USE_FHEX16);
      printf_nice(phdr->p_vaddr,  USE_FHEX64);
      printf_nice(phdr->p_paddr,  USE_FHEX64);
      printf_nice(phdr->p_filesz, USE_FHEX16);
      printf(" ");
      printf_nice(phdr->p_memsz,  USE_FHEX16);

      printf(" %c%c%c ",
            (phdr->p_flags & PF_R ? 'R' : ' '),
            (phdr->p_flags & PF_W ? 'W' : ' '),
            (phdr->p_flags & PF_X ? 'E' : ' '));

      printf_nice(phdr->p_align, USE_FHEX);
      printf("\n");

      if (PT_INTERP == phdr->p_type) {
        printf("    [Requesting program interpreter:" );
        printf_data(getp(p, phdr->p_offset, phdr->p_filesz), phdr->p_filesz, 0, USE_STR);
        printf("]\n");
      }
    }
  }

  printf("\n");

  printf("Section to Segment mapping:\n");
  printf(" Segment Sections...\n");
  for (Elf64_Half i = 0; i < ehdr->e_phnum; ++i) {
    printf("  %2.2d", i);

    Elf64_Phdr *phdr = get_phdr64byindex(p, i);
    if (phdr) {
      for (Elf64_Half j = 1; j < ehdr->e_shnum; ++j) {
        Elf64_Shdr *shdr = get_shdr64byindex(p, j);
        if (shdr) {
          if (shdrinphdr64(shdr, phdr)) {
            printf(" %s", get_secname64byindex(p, j));
          }
        }
      }
    }
    printf("\n");
  }

  printf("\n");

  return 0;
}

static int dump_dynamic64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    Elf64_Shdr *shdr = get_shdr64byindex(p, i);
    if (shdr) {
      if (shdr && SHT_DYNAMIC == shdr->sh_type) {
        size_t cnt = shdr->sh_size / shdr->sh_entsize;

        printf("Dynamic section at offset");
        printf_nice(shdr->sh_offset, USE_FHEX16);
        printf(" contains");
        printf_nice(cnt, USE_DEC);
        printf(" %s\n", 1 == cnt ? "entry:" : "entries:");
        printf(" Tag                Type                Name/Value\n");

        Elf64_Dyn *dyn = get64byshdr(p, shdr);
        for (size_t j = 0; j < cnt; j++) {
          int n = 0;

          printf_nice(dyn->d_tag, USE_FHEX64);
          n = printf (" (%s) ", get_dyntag64(dyn->d_tag));
          printf("%*s", MAX(0, 20 - n), " ");

          if (dyn->d_tag == DT_FLAGS_1) {
            printf(" Flags:");
            printf_masknone(zDT_FLAGS_1, dyn->d_un.d_val);
	  } else if (dyn->d_tag == DT_POSFLAG_1) {
            printf(" Flags:");
            printf_masknone(zDT_POSFLAG_1, dyn->d_un.d_val);
          } else if (dyn->d_tag == DT_FLAGS) {
            printf(" Flags:");
            printf_masknone(zDT_FLAGS, dyn->d_un.d_val);
          } else if (dyn->d_tag == DT_PLTREL) {
            printf(" %s", get_dyntag64(dyn->d_un.d_val));
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
            printf_nice(dyn->d_un.d_val, USE_DEC);
            printf(" (bytes)");
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

          printf("\n");
          ++dyn;
        }
        printf("\n");
      }
    }
  }

  return 0;
}

static int dump_relocs64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    Elf64_Shdr *shdr = get_shdr64byindex(p, i);
    if (shdr) {
      if (SHT_RELA == shdr->sh_type || SHT_REL == shdr->sh_type || SHT_RELR == shdr->sh_type) {
        size_t cnt = shdr->sh_size / shdr->sh_entsize;

        printf("Relocation section");
        printf(" '%s'", get_secname64byindex(p, i));
        printf(" at offset");
        printf_nice(shdr->sh_offset, USE_FHEX16);
        printf(" contains");
        printf_nice(cnt, USE_DEC);
        printf(" %s\n", 1 == cnt ? "entry:" : "entries:");
        if (SHT_RELA == shdr->sh_type) {
          printf(" Offset       Info         Type                 Symbol's Value   Symbol's Name + Addend\n");
        } else {
          printf(" Offset       Info         Type                 Symbol's Value   Symbol's Name\n");
        }

        void *rr = get64byshdr(p, shdr);
        for (size_t j = 0; j < cnt; j++) {
          if (SHT_REL == shdr->sh_type) {
            Elf64_Rel *r = rr + (j * sizeof(Elf64_Rel));
            if (r) {
              printf_nice(r->r_offset, USE_LHEX48);
              printf_nice(r->r_info, USE_LHEX48);
              printf(" %-20s", get_reltype(r));
              // TBD
            }
          } else if (SHT_RELA == shdr->sh_type) {
            Elf64_Rela *r = rr + (j * sizeof(Elf64_Rela));
            if (r) {
              printf_nice(r->r_offset, USE_LHEX48);
              printf_nice(r->r_info, USE_LHEX48);
              printf(" %-20s", get_relatype(r));
              // TBD
              switch (r->r_info & 0xff) {
              case R_X86_64_NONE:
                break;
              case R_X86_64_8:
              case R_X86_64_16:
              case R_X86_64_32:
              case R_X86_64_32S:
              case R_X86_64_64:        // S + A
                break;
              case R_X86_64_RELATIVE:  // B + A
                printf("                 ");
                printf_nice(r->r_addend, USE_HEX);
                break;
              case R_X86_64_GLOB_DAT:
              case R_X86_64_JUMP_SLOT: // S
                printf_nice(r->r_addend, USE_LHEX64);
                //printf_data(getp(p, r->r_offset, 20), 20, 0, USE_STR);
                break;
              case R_X86_64_PC8:
              case R_X86_64_PC16:
              case R_X86_64_PC32:
              case R_X86_64_PC64:      // S + A - P
                break;
              case R_X86_64_GOT32:     // G + A
                break;
              case R_X86_64_PLT32:     // L + A - P
                break;
              case R_X86_64_SIZE32:
              case R_X86_64_SIZE64:    // Z + A
                break;
              case R_X86_64_GOTPC32:   // GOT + A - P
                break;
              case R_X86_64_GOTPCREL:  // G + GOT + A - P
                break;
              case R_X86_64_GOTOFF64:  // S + A - GOT
                break;
              case R_X86_64_COPY:
                break;
              default:
                break;
              }
            }
          } else if (SHT_RELR == shdr->sh_type) {
          }
          printf("\n");
        }
        printf("\n");
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
  // TBD
  return 0;
}

static int dump_histogram64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr) {
  // TBD
  return 0;
}

static int dump_versionsym64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr, Elf64_Shdr *shdr) {
  MALLOCA(Elf64_Word, vnames, 1024);

  for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
    Elf64_Shdr *shdr = get_shdr64byindex(p, i);
    if (shdr) {
      if (SHT_GNU_verneed == shdr->sh_type) {
        Elf64_Word offset = 0;
        vnames[0] = shdr->sh_link;

        for (Elf64_Word j = 0; j < shdr->sh_info; ++j) {
          Elf64_Verneed *vn = getp(p, shdr->sh_offset, sizeof(Elf64_Verneed));
          if (vn) {
            Elf64_Word xoffset = offset + vn->vn_aux;
            for (Elf64_Half k = 0; k < vn->vn_cnt; ++k) {
              Elf64_Vernaux *va = getp(p, shdr->sh_offset + xoffset, sizeof(Elf64_Vernaux));
              if (va) {
                vnames[va->vna_other] = va->vna_name;
	        xoffset += va->vna_next;
              }
            }
          }

          offset += vn->vn_next;
        }
      }
    }
  }

  size_t cnt = shdr->sh_size / shdr->sh_entsize;
  printf("Version symbols section");
  printf(" '%s'", get_secname64byshdr(p, shdr));
  printf(" contains");
  printf_nice(cnt, USE_DEC);
  printf(" %s\n", 1 == cnt ? "entry:" : "entries:");
  printf(" Addr:");
  printf_nice(shdr->sh_addr, USE_FHEX64);
  printf("  Offset:");
  printf_nice(shdr->sh_offset, USE_FHEX24);
  printf("  Link:");
  printf_nice(shdr->sh_link, USE_DEC);
  printf(" (%s)", get_secname64byindex(p, shdr->sh_link));

  for (size_t j = 0; j < cnt; ++j) {
    Elf64_Versym *vs = getp(p, shdr->sh_offset + (j * sizeof(Elf64_Versym)), sizeof(Elf64_Versym));
    if (vs) {
      if (j % 4 == 0) {
        printf("\n");
        printf_nice(j, USE_LHEX16);
        printf(": ");
      }

      int n = 0;
      if (0 == *vs)        n += printf("   0 (*local*)    ");
      else if (1 == *vs)   n += printf("   1 (*global*)   ");
      else {
        n += printf("%4x%c", *vs & VERSYM_VERSION, *vs & VERSYM_HIDDEN ? 'h' : ' ');
        if (vnames[*vs & VERSYM_VERSION] && (*vs & VERSYM_VERSION) < NELEMENTS(vnames)) {
          n += printf("(%s)", get_name64byoffset(p, vnames[0], vnames[*vs & VERSYM_VERSION]));
        } else {
          n += printf("???");
        }
      }
      printf("%*s", MAX(0, 20 - n), " ");
    }
  }

  if (cnt) printf("\n");
  printf("\n");

  return 0;
}

static int dump_versionneed64(const pbuffer_t p, const poptions_t o, Elf64_Shdr *shdr) {
  printf("Version needs section");
  printf(" '%s'", get_secname64byshdr(p, shdr));
  printf(" contains");
  printf_nice(shdr->sh_info, USE_DEC);
  printf(" %s\n", 1 == shdr->sh_info ? "entry:" : "entries:");

  printf(" Addr:");
  printf_nice(shdr->sh_addr, USE_FHEX64);
  printf("  Offset:");
  printf_nice(shdr->sh_offset, USE_FHEX24);
  printf("  Link:");
  printf_nice(shdr->sh_link, USE_DEC);
  printf(" (%s)", get_secname64byindex(p, shdr->sh_link));
  printf("\n");

  Elf64_Word offset = 0;
  for (Elf64_Word j = 0; j < shdr->sh_info; ++j) {
    Elf64_Verneed *vn = getp(p, shdr->sh_offset, sizeof(Elf64_Verneed));
    if (vn) {
      printf_nice(offset, USE_LHEX24);
      printf(": Version: %d  File: %s  Cnt: %d\n",
        vn->vn_version, get_name64byoffset(p, shdr->sh_link, vn->vn_file), vn->vn_cnt);

      Elf64_Word xoffset = offset + vn->vn_aux;
      for (Elf64_Half k = 0; k < vn->vn_cnt; ++k) {
        Elf64_Vernaux *va = getp(p, shdr->sh_offset + xoffset, sizeof(Elf64_Vernaux));
        if (va) {
          printf_nice(xoffset, USE_FHEX16);
          printf(":  Name: %-12s", get_name64byoffset(p, shdr->sh_link, va->vna_name));

          printf(" Flags:");
          printf_masknone(zVNA_FLAGS, va->vna_flags);
          printf("  Version: %d", va->vna_other);
          printf("\n");

	  xoffset += va->vna_next;
        }
      }
    }

    offset += vn->vn_next;
  }

  printf("\n");

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

      printf("\n");
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
          printf("  Build ID: ");
          printf_data(cc, nhdr->n_descsz, 0, USE_HEX);
          printf ("\n");
        } else if (NT_GNU_GOLD_VERSION == nhdr->n_type) {
          printf("  Version: ");
          printf_data(cc, nhdr->n_descsz, 0, USE_STR);
          printf ("\n");
        } else if (NT_GNU_HWCAP == nhdr->n_type) {
          printf("  Hardware Capabilities: ");
          // TBD
        } else if (NT_GNU_PROPERTY_TYPE_0 == nhdr->n_type) {
          printf("  Properties: ");

          for (Elf64_Word i = 0; i < nhdr->n_descsz; i += 8) {
            if ((nhdr->n_descsz - i) < 8) {
              printf("<corrupt descsz: %#x\n", nhdr->n_descsz);
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
                    if (4 != datasz)       printf("%s <corrupt length: %#x> ", get_gnuproperty64(x), datasz);
                    else                   printf("%s", get_gnuproperty64(x));

                    if (x == GNU_PROPERTY_X86_FEATURE_1_AND) {
                      printf_mask(zGNU_PROPERTY_X86_FEATURE_1_AND, getLE(cc + i + 8, 4));
                    }
                  }
                }
              }
            }
          }
          printf ("\n");
        } else if (NT_GNU_ABI_TAG == nhdr->n_type) {
          printf("  OS: %s, ABI: %ld.%ld.%ld\n",
            get_gnuabitab(getLE(cc, 4)), getLE(cc + 4, 4), getLE(cc + 8, 4), getLE(cc + 12, 4));
        } else {
          printf("  Description Data: ");
          printf_data(cc, nhdr->n_descsz, 0, USE_HEX);
          printf ("\n");
        }
        printf("\n");
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
    } else if (is64(p)) {
      Elf64_Ehdr *ehdr = get_ehdr64(p);
      if (ehdr) {
        if (o->action & OPTREADELF_FILEHEADER)       dump_fileheader64(p, o, ehdr);
        if (o->action & OPTREADELF_SECTIONHEADERS)   dump_sectionheaders64(p, o, ehdr);
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

