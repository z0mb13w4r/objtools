#ifndef __COMMON_H_
#define __COMMON_H_

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>

#include "defines.h"

#define ELFOSABI_OPENVMS       13 /* OpenVMS */
#define ELFOSABI_NSK           14 /* Hewlett-Packard Non-Stop Kernel */
#define ELFOSABI_AROS          15 /* AROS */
#define ELFOSABI_FENIXOS       16 /* FenixOS */
#define ELFOSABI_CLOUDABI      17 /* Nuxi CloudABI */
#define ELFOSABI_OPENVOS       18 /* Stratus Technologies OpenVOS */

#define ELFOSABI_C6000_ELFABI  64 /* Bare-metal TMS320C6000 */
#define ELFOSABI_AMDGPU_HSA    64 /* AMD HSA Runtime */
#define ELFOSABI_C6000_LINUX   65 /* Linux TMS320C6000 */
#define ELFOSABI_AMDGPU_PAL    65 /* AMD PAL Runtime */
#define ELFOSABI_ARM_FDPIC     65 /* ARM FDPIC */
#define ELFOSABI_AMDGPU_MESA3D 66 /* AMD Mesa3D Runtime */

#define SHT_RELR               19 /* RELR relative relocations */

#define PT_GNU_PROPERTY        (PT_LOOS + 0x474e553) /* GNU property */
#define PT_GNU_SFRAME          (PT_LOOS + 0x474e554) /* SFrame stack trace information */

#define PT_GNU_MBIND_NUM       4096
#define PT_GNU_MBIND_LO        (PT_LOOS + 0x474e555)
#define PT_GNU_MBIND_HI        (PT_GNU_MBIND_LO + PT_GNU_MBIND_NUM - 1)

/* OpenBSD segment types.  */
#define PT_OPENBSD_MUTABLE     (PT_LOOS + 0x5a3dbe5) /* Like bss, but not immutable.  */
#define PT_OPENBSD_RANDOMIZE   (PT_LOOS + 0x5a3dbe6) /* Fill with random data.  */
#define PT_OPENBSD_WXNEEDED    (PT_LOOS + 0x5a3dbe7) /* Program does W^X violations.  */
#define PT_OPENBSD_BOOTDATA    (PT_LOOS + 0x5a41be6) /* Section for boot arguments.  */

/* Values used in GNU .note.ABI-tag notes (NT_GNU_ABI_TAG).  */
#define GNU_ABI_TAG_LINUX	0
#define GNU_ABI_TAG_HURD	1
#define GNU_ABI_TAG_SOLARIS	2
#define GNU_ABI_TAG_FREEBSD	3
#define GNU_ABI_TAG_NETBSD	4
#define GNU_ABI_TAG_SYLLABLE	5
#define GNU_ABI_TAG_NACL	6

#define NT_GNU_BUILD_ATTRIBUTE_OPEN     0x100
#define NT_GNU_BUILD_ATTRIBUTE_FUNC     0x101

#define NT_X86_CET                      0x203 /* x86 CET state. */
                                              /* note name must be "LINUX". */
#define NT_ARM_TAGGED_ADDR_CTRL         0x409 /* AArch64 tagged address control (prctl()) */
#define NT_ARM_PAC_ENABLED_KEYS         0x40a /* AArch64 pointer authentication enabled keys (prctl()) */
#define NT_ARM_SSVE                     0x40b /* AArch64 SME streaming SVE registers. */
                                              /* Note: name must be "LINUX". */
#define NT_ARM_ZA                       0x40c /* AArch64 SME ZA register. */
                                              /* Note: name must be "LINUX". */
#define NT_ARC_V2                       0x600 /* ARC HS accumulator/extra registers. */
                                              /* note name must be "LINUX". */
#define NT_RISCV_CSR                    0x900 /* RISC-V Control and Status Registers */
                                              /* note name must be "LINUX". */

#define NT_FPREGS                       12 /* Has a struct fpregset */
#define NT_WIN32PSTATUS                 18 /* Has a struct win32_pstatus */

/* The range 0xff000000 to 0xffffffff is set aside for notes that don't
   originate from any particular operating system.  */
#define NT_GDB_TDESC                    0xff000000 /* Contains copy of GDB's target description XML. */

/* A 4-byte unsigned integer property: A bit is set if it is set in any
   relocatable inputs.  */
#define GNU_PROPERTY_X86_UINT32_OR_LO           0xc0008000
#define GNU_PROPERTY_X86_UINT32_OR_HI           0xc000ffff

/* A 4-byte unsigned integer property: A bit is set if it is set in any
   relocatable inputs and the property is present in all relocatable
   inputs.  */
#define GNU_PROPERTY_X86_UINT32_OR_AND_LO       0xc0010000
#define GNU_PROPERTY_X86_UINT32_OR_AND_HI       0xc0017fff

#define GNU_PROPERTY_X86_FEATURE_2_USED         (GNU_PROPERTY_X86_UINT32_OR_AND_LO + 1)
#define GNU_PROPERTY_X86_FEATURE_2_NEEDED       (GNU_PROPERTY_X86_UINT32_OR_LO + 1)

/* A 4-byte unsigned integer property: A bit is set if it is set in any
   relocatable inputs and the property is present in all relocatable
   inputs.  */
#define GNU_PROPERTY_X86_UINT32_OR_AND_LO       0xc0010000
#define GNU_PROPERTY_X86_UINT32_OR_AND_HI       0xc0017fff

#define GNU_PROPERTY_X86_COMPAT_ISA_1_USED      0xc0000000
#define GNU_PROPERTY_X86_COMPAT_ISA_1_NEEDED    0xc0000001

#define GNU_PROPERTY_X86_COMPAT_2_ISA_1_USED    (GNU_PROPERTY_X86_UINT32_OR_AND_LO + 0)
#define GNU_PROPERTY_X86_COMPAT_2_ISA_1_NEEDED  (GNU_PROPERTY_X86_UINT32_OR_LO + 0)

#endif

