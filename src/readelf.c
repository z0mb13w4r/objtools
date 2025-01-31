#include <stdio.h>

#include "printf.h"
#include "readelf.h"

typedef struct convert_s {
  const char* text;
  const int   type;
} convert_t, *pconvert_t;

static convert_t EHDRTYPE[] = {
  {"NONE (No file type)",          ET_NONE},
  {"REL (Relocatable file)",       ET_REL},
  {"EXEC (Executable file)",       ET_EXEC},
  {"DYN (Shared object file)",     ET_DYN},
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

static convert_t NHDRTYPE[] = {
  {"NT_GNU_ABI_TAG (ABI version tag)",                                       NT_GNU_ABI_TAG},
  {"NT_GNU_HWCAP (DSO-supplied software HWCAP info)",                        NT_GNU_HWCAP},
  {"NT_GNU_BUILD_ID (unique build ID bitstring)",                            NT_GNU_BUILD_ID},
  {"NT_GNU_GOLD_VERSION (gold version)",                                     NT_GNU_GOLD_VERSION},
  {"NT_GNU_PROPERTY_TYPE_0",                                                 NT_GNU_PROPERTY_TYPE_0},
  {"NT_GNU_BUILD_ATTRIBUTE_OPEN",                                            NT_GNU_BUILD_ATTRIBUTE_OPEN},
  {"NT_GNU_BUILD_ATTRIBUTE_FUNC",                                            NT_GNU_BUILD_ATTRIBUTE_FUNC},
  {0, 0}
};

static convert_t NHDRTYPECORE[] = {
  {"NT_AUXV (auxiliary vector)",                                             NT_AUXV},
  {"NT_PRSTATUS (prstatus structure)",                                       NT_PRSTATUS},
  {"NT_FPREGSET (floating point registers)",                                 NT_FPREGSET},
  {"NT_PRPSINFO (prpsinfo structure)",                                       NT_PRPSINFO},
  {"NT_TASKSTRUCT (task structure)",                                         NT_TASKSTRUCT},
  {"NT_GDB_TDESC (GDB XML target description)",                              NT_GDB_TDESC},
  {"NT_PRXFPREG (user_xfpregs structure)",                                   NT_PRXFPREG},
  {"NT_PPC_VMX (ppc Altivec registers)",                                     NT_PPC_VMX},
  {"NT_PPC_VSX (ppc VSX registers)",                                         NT_PPC_VSX},
  {"NT_PPC_TAR (ppc TAR register)",                                          NT_PPC_TAR},
  {"NT_PPC_PPR (ppc PPR register)",                                          NT_PPC_PPR},
  {"NT_PPC_DSCR (ppc DSCR register)",                                        NT_PPC_DSCR},
  {"NT_PPC_EBB (ppc EBB registers)",                                         NT_PPC_EBB},
  {"NT_PPC_PMU (ppc PMU registers)",                                         NT_PPC_PMU},
  {"NT_PPC_TM_CGPR (ppc checkpointed GPR registers)",                        NT_PPC_TM_CGPR},
  {"NT_PPC_TM_CFPR (ppc checkpointed floating point registers)",             NT_PPC_TM_CFPR},
  {"NT_PPC_TM_CVMX (ppc checkpointed Altivec registers)",                    NT_PPC_TM_CVMX},
  {"NT_PPC_TM_CVSX (ppc checkpointed VSX registers)",                        NT_PPC_TM_CVSX},
  {"NT_PPC_TM_SPR (ppc TM special purpose registers)",                       NT_PPC_TM_SPR},
  {"NT_PPC_TM_CTAR (ppc checkpointed TAR register)",                         NT_PPC_TM_CTAR},
  {"NT_PPC_TM_CPPR (ppc checkpointed PPR register)",                         NT_PPC_TM_CPPR},
  {"NT_PPC_TM_CDSCR (ppc checkpointed DSCR register)",                       NT_PPC_TM_CDSCR},
  {"NT_386_TLS (x86 TLS information)",                                       NT_386_TLS},
  {"NT_386_IOPERM (x86 I/O permissions)",                                    NT_386_IOPERM},
  {"NT_X86_XSTATE (x86 XSAVE extended state)",                               NT_X86_XSTATE},
  {"NT_X86_CET (x86 CET state)",                                             NT_X86_CET},
  {"NT_S390_HIGH_GPRS (s390 upper register halves)",                         NT_S390_HIGH_GPRS},
  {"NT_S390_TIMER (s390 timer register)",                                    NT_S390_TIMER},
  {"NT_S390_TODCMP (s390 TOD comparator register)",                          NT_S390_TODCMP},
  {"NT_S390_TODPREG (s390 TOD programmable register)",                       NT_S390_TODPREG},
  {"NT_S390_CTRS (s390 control registers)",                                  NT_S390_CTRS},
  {"NT_S390_PREFIX (s390 prefix register)",                                  NT_S390_PREFIX},
  {"NT_S390_LAST_BREAK (s390 last breaking event address)",                  NT_S390_LAST_BREAK},
  {"NT_S390_SYSTEM_CALL (s390 system call restart data)",                    NT_S390_SYSTEM_CALL},
  {"NT_S390_TDB (s390 transaction diagnostic block)",                        NT_S390_TDB},
  {"NT_S390_VXRS_LOW (s390 vector registers 0-15 upper half)",               NT_S390_VXRS_LOW},
  {"NT_S390_VXRS_HIGH (s390 vector registers 16-31)",                        NT_S390_VXRS_HIGH},
  {"NT_S390_GS_CB (s390 guarded-storage registers)",                         NT_S390_GS_CB},
  {"NT_S390_GS_BC (s390 guarded-storage broadcast control)",                 NT_S390_GS_BC},
  {"NT_ARM_VFP (arm VFP registers)",                                         NT_ARM_VFP},
  {"NT_ARM_TLS (AArch TLS registers)",                                       NT_ARM_TLS},
  {"NT_ARM_HW_BREAK (AArch hardware breakpoint registers)",                  NT_ARM_HW_BREAK},
  {"NT_ARM_HW_WATCH (AArch hardware watchpoint registers)",                  NT_ARM_HW_WATCH},
  {"NT_ARM_SYSTEM_CALL (AArch system call number)",                          NT_ARM_SYSTEM_CALL},
  {"NT_ARM_SVE (AArch SVE registers)",                                       NT_ARM_SVE},
  {"NT_ARM_PAC_MASK (AArch pointer authentication code masks)",              NT_ARM_PAC_MASK},
  {"NT_ARM_PACA_KEYS (ARM pointer authentication address keys)",             NT_ARM_PACA_KEYS},
  {"NT_ARM_PACG_KEYS (ARM pointer authentication generic keys)",             NT_ARM_PACG_KEYS},
  {"NT_ARM_TAGGED_ADDR_CTRL (AArch tagged address control)",                 NT_ARM_TAGGED_ADDR_CTRL},
  {"NT_ARM_SSVE (AArch64 streaming SVE registers)",                          NT_ARM_SSVE},
  {"NT_ARM_ZA (AArch64 SME ZA register)",                                    NT_ARM_ZA},
  {"NT_ARM_PAC_ENABLED_KEYS (AArch64 pointer authentication enabled keys)",  NT_ARM_PAC_ENABLED_KEYS},
  {"NT_ARC_V2 (ARC HS accumulator/extra registers)",                         NT_ARC_V2},
  {"NT_RISCV_CSR (RISC-V control and status registers)",                     NT_RISCV_CSR},
  {"NT_PSTATUS (pstatus structure)",                                         NT_PSTATUS},
  {"NT_FPREGS (floating point registers)",                                   NT_FPREGS},
  {"NT_PSINFO (psinfo structure)",                                           NT_PSINFO},
  {"NT_LWPSTATUS (lwpstatus_t structure)",                                   NT_LWPSTATUS},
  {"NT_LWPSINFO (lwpsinfo_t structure)",                                     NT_LWPSINFO},
  {"NT_WIN32PSTATUS (win32_pstatus structure)",                              NT_WIN32PSTATUS},
  {"NT_SIGINFO (siginfo_t data)",                                            NT_SIGINFO},
  {"NT_FILE (mapped files)",                                                 NT_FILE},
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

static const char* get_gnuabitab(const int g) {
  static char buff[32];

  for (pconvert_t x = GNUABITAB; 0 != x->text; ++x) {
    if (x->type == g) {
      return x->text;
    }
  }

  snprintf (buff, sizeof (buff), "<unknown: %x>", g);
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

static const char* get_nhdrtype64(const pbuffer_t p, Elf64_Nhdr *n) {
  static char buff[32];

  if (n) {
    Elf32_Ehdr *e = get_ehdr32(p);
    if (ET_CORE == e->e_type) {
      for (pconvert_t x = NHDRTYPECORE; 0 != x->text; ++x) {
        if (x->type == n->n_type) {
          return x->text;
        }
      }
    } else {
      for (pconvert_t x = NHDRTYPE; 0 != x->text; ++x) {
        if (x->type == n->n_type) {
          return x->text;
        }
      }
    }

    snprintf (buff, sizeof (buff), "<unknown: %x>", n->n_type);
    return buff;
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

    snprintf (buff, sizeof (buff), "<unknown: %x>", p->p_type);
    return buff;
  }

  return NULL;
}

int readelf(const pbuffer_t p, const poptions_t o) {
  if (isELF(p)) {
    if (o->action & OPTREADELF_FILEHEADER) {
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
    }

    if (is32(p)) {
      Elf32_Ehdr *e = get_ehdr32(p);
    } else if (is64(p)) {
      Elf64_Ehdr *ehdr = get_ehdr64(p);
      if (o->action & OPTREADELF_FILEHEADER) {
        printf("  Type:                              %s\n",                    get_ehdrtype64(ehdr));

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
      }

      if (o->action & OPTREADELF_SECTIONHEADERS) {
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
            // TBD
          }
          printf("\n");
        }
        printf("Key to Flags:\n");
        printf("  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),\n");
        printf("  L (link order), O (extra OS processing required), G (group), T (TLS),\n");
        printf("  C (compressed), x (unknown), o (OS specific), E (exclude),\n");
        printf("  l (large), p (processor specific)\n");
        printf("\n");
      }

      if (o->action & OPTREADELF_PROGRAMHEADERS) {
        printf("PROGRAM HEADERS:\n");
        printf("  Type            Offset VirtAddr           PhysAddr           FileSiz MemSiz Flg  Align\n");
        for (Elf64_Half i = 0; i < ehdr->e_phnum; ++i) {
          Elf64_Phdr *phdr = get_phdr64byindex(p, i);
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
        }

        printf("\n");

        printf("Section to Segment mapping:\n");
        printf(" Segment Sections...\n");
	for (Elf64_Half i = 0; i < ehdr->e_phnum; ++i) {
	  printf("  %2.2d", i);
	  for (Elf64_Half j = 1; j < ehdr->e_shnum; ++j) {
            // TBD
	  }
	  printf("\n");
	}

	printf("\n");
      }

      if (o->action & OPTREADELF_NOTES) {
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
                for (Elf64_Word i = 0; i < nhdr->n_descsz; ++i) {
                  printf ("%02x", cc[i] & 0xff);
                }
                printf ("\n");
              } else if (NT_GNU_GOLD_VERSION == nhdr->n_type) {
                printf("  Version: ");
                for (Elf64_Word i = 0; i < nhdr->n_descsz && 0 != cc[i]; ++i) {
                  printf ("%c", cc[i] & 0xff);
                }
                printf ("\n");
              } else if (NT_GNU_HWCAP == nhdr->n_type) {
                printf("  Hardware Capabilities: ");
                // TBD
              } else if (NT_GNU_PROPERTY_TYPE_0 == nhdr->n_type) {
                printf("  Properties: ");
                // TBD
              } else if (NT_GNU_ABI_TAG == nhdr->n_type) {
                printf("  OS: %s, ABI: %ld.%ld.%ld\n",
                  get_gnuabitab(getLE(cc, 4)), getLE(cc + 4, 4), getLE(cc + 8, 4), getLE(cc + 12, 4));
              } else {
                printf("  Description Data: ");
                for (Elf64_Word i = 0; i < nhdr->n_descsz; ++i) {
                  printf ("%02x", cc[i] & 0xff);
                }
                printf ("\n");
              }
              printf("\n");
            }
          }
        }
      }

      if (o->actions) {
        paction_t x = o->actions;
        while (x) {
          Elf64_Shdr* shdr = get_shdr64byname(p, x->secname);
	  if (shdr) {
            if (ACT_HEXDUMP == x->action) {
              printf("Hex dump of section '%s':\n", x->secname);

              if (0 != shdr->sh_size && shdr->sh_type != SHT_NOBITS) {
                printf_data(getp(p, shdr->sh_offset, shdr->sh_size), shdr->sh_size, shdr->sh_addr, USE_HEXDUMP);
              } else {
                printf("readelf: Warning: Section '%s' has no data to dump!\n", x->secname);
	      }
	    } else if (ACT_STRDUMP == x->action) {
              printf("String dump of section '%s':\n", x->secname);

              if (0 != shdr->sh_size && shdr->sh_type != SHT_NOBITS) {
                printf_data(getp(p, shdr->sh_offset, shdr->sh_size), shdr->sh_size, shdr->sh_addr, USE_STRDUMP);
              } else {
                printf("readelf: Warning: Section '%s' has no data to dump!\n", x->secname);
	      }
	    }

            printf("\n");
	  } else {
            printf("readelf: Warning: Section '%s' was not dumped because it does not exist!\n", x->secname);
	  }

          x = x->actions;
        }
      }
    }
  }

  return 0;
}


