#include "printf.h"
#include "readelf.h"
#include "elfcode.h"

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

    snprintf(buff, sizeof (buff), "<unknown: %x>", n->n_type);
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

        void *rr = get64s(p, shdr);
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
  return 0;
}

static int dump_versionsym64(const pbuffer_t p, const poptions_t o, Elf64_Shdr *shdr) {
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
  printf("\n");

        //unsigned short* cc = getp(p, shdr->sh_offset, cnt);
  for (size_t j = 0; j < cnt; j += 4) {
    printf_nice(j, USE_LHEX16);
    printf(": ");
          //for (size_t k = 0; k < 4; ++k) {
          //  switch (cc[j + k]) {
          //  case 0:
          //    printf("   0 (*local*)    ");
          //    break;
          //  case 1:
          //    printf("   1 (*global*)   ");
          //    break;
          //  }
          //}
    printf("\n");
  }
        //Elf64_Sym *sym = get64s(p, get_shdr64byindex(p, shdr->sh_link));
// TBD
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
          printf(":   Name: %s", get_name64byoffset(p, shdr->sh_link, va->vna_name));

          printf("  Flags:");
          if (0 == va->vna_flags)              printf(" none");
          if (va->vna_flags & VER_FLG_BASE)    printf(" BASE");
          if (va->vna_flags & VER_FLG_WEAK)    printf(" WEAK");
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
        dump_versionsym64(p, o, shdr);
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
              // TBD
              unsigned int x = getLE(cc + i, 4);
              unsigned int y = getLE(cc + i + 4, 4);
              if ((nhdr->n_descsz - i + 1) < x) {
                printf("<corrupt type (%#x) datasz: %#x>\n", x, y);
                break;
              } else {
//printf("A %x %x\n", x, y);
                if (x >= GNU_PROPERTY_LOPROC && x <= GNU_PROPERTY_HIPROC) {
//printf("B\n");
                  if (ehdr->e_machine == EM_X86_64 || ehdr->e_machine == EM_IAMCU || ehdr->e_machine == EM_386) {
//printf("C\n");
                    printf("%s", get_gnuproperty64(x));
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
          printf_data(cc, nhdr->n_descsz, 0, USE_STR);
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

