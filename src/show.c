#include <stdio.h>

#include "show.h"
#include "printf.h"

typedef struct convert_s {
  const char* text;
  const int   type;
} convert_t, *pconvert_t;

static convert_t EHDRTYPE[] = {
  {"NONE (No file type)",      ET_NONE},
  {"REL (Relocatable file)",   ET_REL},
  {"EXEC (Executable file)",   ET_EXEC},
  {"DYN (Shared object file)", ET_DYN},
  {0,                          0}
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
  {"W", SHF_WRITE},
  {"A", SHF_ALLOC},
  {"X", SHF_EXECINSTR},
  {"M", SHF_MERGE},
  {"S", SHF_STRINGS},
  {"I", SHF_INFO_LINK},
  {"L", SHF_LINK_ORDER},
  {"O", SHF_OS_NONCONFORMING},
  {"G", SHF_GROUP},
  {"T", SHF_TLS},
  {"E", SHF_EXCLUDE},
  {"C", SHF_COMPRESSED},
  {0, 0}
};

static convert_t SHDRTYPE[] = {
  {"NULL",                   SHT_NULL},
  {"PROGBITS",               SHT_PROGBITS},
  {"SYMTAB",                 SHT_SYMTAB},
  {"STRTAB",                 SHT_STRTAB},
  {"RELA",                   SHT_RELA},
  {"RELR",                   SHT_RELR},
  {"HASH",                   SHT_HASH},
  {"DYNAMIC",                SHT_DYNAMIC},
  {"NOTE",                   SHT_NOTE},
  {"NOBITS",                 SHT_NOBITS},
  {"REL",                    SHT_REL},
  {"SHLIB",                  SHT_SHLIB},
  {"DYNSYM",                 SHT_DYNSYM},
  {"INIT_ARRAY",             SHT_INIT_ARRAY},
  {"FINI_ARRAY",             SHT_FINI_ARRAY},
  {"PREINIT_ARRAY",          SHT_PREINIT_ARRAY},
  {"GNU_HASH",               SHT_GNU_HASH},
  {"GROUP",                  SHT_GROUP},
  {"SYMTAB SECTION INDICES", SHT_SYMTAB_SHNDX},
  {"VERDEF",                 SHT_GNU_verdef},
  {"VERNEED",                SHT_GNU_verneed},
  {"VERSYM",                 SHT_GNU_versym},
  {0, 0}
};


static const char* get_ehdrtype64(Elf64_Ehdr *e) {
  static char buff[32];

  if (e) {
    for (pconvert_t x = EHDRTYPE; 0 != x->text; ++x) {
      if (x->type == e->e_type) {
        return x->text;
      }
    }

    snprintf (buff, sizeof (buff), "<unknown: %x>", e->e_type);
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
      case EM_MSP430_OLD:
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

    snprintf (buff, sizeof (buff), "<unknown: %x>", e->e_type);
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

    snprintf (buff, sizeof (buff), "<unknown: %x>", s->sh_type);
    return buff;
  }
  
  return NULL;
}

int show(const pbuffer_t p) {
  if (isELF(p)) {
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

    if (is32(p)) {
      Elf32_Ehdr *e = get_ehdr32(p);
    } else if (is64(p)) {
      Elf64_Ehdr *e = get_ehdr64(p);
      printf("  Type:                              %s\n",                    get_ehdrtype64(e));

      printf("  Version:                           0x%x\n",                  e->e_version);
      printf("  Entry point address:               0x%04lx\n",               e->e_entry);
      printf("  Start of program headers:          %lu (bytes into file)\n", e->e_phoff);
      printf("  Start of section headers:          %lu (bytes into file)\n", e->e_shoff);
      printf("  Flags:                             0x%x\n",                  e->e_flags);

      printf("  Size of this header:               %d (bytes)\n",            e->e_ehsize);
      printf("  Size of program headers:           %d (bytes)\n",            e->e_phentsize);
      printf("  Number of program headers:         %d\n",                    e->e_phnum);
      printf("  Size of section headers:           %d (bytes)\n",            e->e_shentsize);
      printf("  Number of section headers:         %d\n",                    e->e_shnum);
      printf("  Section header string table index: %d\n",                    e->e_shstrndx);

      printf("SECTION HEADERS:\n");
      char *xx = NULL;
      Elf64_Shdr *x = get_shdr64(p, e->e_shstrndx);
      if (x) {
	xx = getp(p, x->sh_offset, x->sh_size);
      }

      for (Elf64_Half i = 0; i < e->e_shnum; ++i) {
        printf("  [%2d]", i);

        Elf64_Shdr *s = get_shdr64(p, i);
        if (s) {
          if (xx) {
	    printf(" %-20s", xx + s->sh_name);
	  } else {
	    printf(" %3d", s->sh_name);
	  }
	  printf(" %-15s", get_shdrtype64(s));

          printf_nice(s->sh_addr, USE_LHEX64);
          printf_nice(s->sh_offset, USE_LHEX32);
          printf_nice(s->sh_size, USE_LHEX32);
          printf_nice(s->sh_entsize, USE_LHEX8);
	  printf(" %3s", get_shdrflags64(s));
	  printf(" %2u %3u ", s->sh_link, s->sh_info);
	  printf_nice(s->sh_addralign, USE_DECIMAL2);
        }
	printf("\n");
      }
      printf("Key to Flags:\n");
      printf("  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),\n");
      printf("  L (link order), O (extra OS processing required), G (group), T (TLS),\n");
      printf("  C (compressed), x (unknown), o (OS specific), E (exclude),\n");
      printf("  l (large), p (processor specific)\n");
    }
  }

  return 0;
}


