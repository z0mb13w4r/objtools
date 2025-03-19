#ifndef __COMMON_H_
#define __COMMON_H_

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>

#include "defines.h"

#define SHT_RELR               19 /* RELR relative relocations */

#define PT_GNU_MBIND_NUM       4096
#define PT_GNU_MBIND_LO        (PT_LOOS + 0x474e555)
#define PT_GNU_MBIND_HI        (PT_GNU_MBIND_LO + PT_GNU_MBIND_NUM - 1)

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

