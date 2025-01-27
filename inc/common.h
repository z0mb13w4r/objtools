#ifndef __COMMON_H_
#define __COMMON_H_

#include <elf.h>

#define ELFOSABI_OPENVMS       13 /* OpenVMS */
#define ELFOSABI_NSK	       14 /* Hewlett-Packard Non-Stop Kernel */
#define ELFOSABI_AROS	       15 /* AROS */
#define ELFOSABI_FENIXOS       16 /* FenixOS */
#define ELFOSABI_CLOUDABI      17 /* Nuxi CloudABI */
#define ELFOSABI_OPENVOS       18 /* Stratus Technologies OpenVOS */

#define ELFOSABI_C6000_ELFABI  64 /* Bare-metal TMS320C6000 */
#define ELFOSABI_AMDGPU_HSA    64 /* AMD HSA Runtime */
#define ELFOSABI_C6000_LINUX   65 /* Linux TMS320C6000 */
#define ELFOSABI_AMDGPU_PAL    65 /* AMD PAL Runtime */
#define ELFOSABI_ARM_FDPIC     65 /* ARM FDPIC */
#define ELFOSABI_AMDGPU_MESA3D 66 /* AMD Mesa3D Runtime */

/* MSP430 magic number.  Written in the absense of everything.  */
#define EM_MSP430_OLD          0x1059

#define SHT_RELR               19 /* RELR relative relocations */

#define PT_GNU_PROPERTY        (PT_LOOS + 0x474e553) /* GNU property */
#define PT_GNU_SFRAME          (PT_LOOS + 0x474e554) /* SFrame stack trace information */

/* OpenBSD segment types.  */
#define PT_OPENBSD_MUTABLE     (PT_LOOS + 0x5a3dbe5)  /* Like bss, but not immutable.  */
#define PT_OPENBSD_RANDOMIZE   (PT_LOOS + 0x5a3dbe6)  /* Fill with random data.  */
#define PT_OPENBSD_WXNEEDED    (PT_LOOS + 0x5a3dbe7)  /* Program does W^X violations.  */
#define PT_OPENBSD_BOOTDATA    (PT_LOOS + 0x5a41be6)  /* Section for boot arguments.  */

#endif

