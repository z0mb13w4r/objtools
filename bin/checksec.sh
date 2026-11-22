#!/bin/bash
#
# The BSD License (http://www.opensource.org/licenses/bsd-license.php)
# specifies the terms and conditions of use for checksec.sh:
#
# Copyright (c) 2009-2011, Tobias Klein.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in
#   the documentation and/or other materials provided with the
#   distribution.
# * Neither the name of Tobias Klein nor the name of trapkit.de may be
#   used to endorse or promote products derived from this software
#   without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
# THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
# DAMAGE.
#
# Name    : checksec.sh
# Version : 1.5
# Author  : Tobias Klein
# Date    : November 2011
# Download: http://www.trapkit.de/tools/checksec.html
# Changes : http://www.trapkit.de/tools/checksec_changes.txt
#
# Description:
#
# Modern Linux distributions offer some mitigation techniques to make it
# harder to exploit software vulnerabilities reliably. Mitigations such
# as RELRO, NoExecute (NX), Stack Canaries, Address Space Layout
# Randomization (ASLR) and Position Independent Executables (PIE) have
# made reliably exploiting any vulnerabilities that do exist far more
# challenging. The checksec.sh script is designed to test what *standard*
# Linux OS and PaX (http://pax.grsecurity.net/) security features are being
# used.
#
# As of version 1.3 the script also lists the status of various Linux kernel
# protection mechanisms.
#
# Credits:
#
# Thanks to Brad Spengler (grsecurity.net) for the PaX support.
# Thanks to Jon Oberheide (jon.oberheide.org) for the kernel support.
# Thanks to Ollie Whitehouse (Research In Motion) for rpath/runpath support.
#
# Others that contributed to checksec.sh (in no particular order):
#
# Simon Ruderich, Denis Scherbakov, Stefan Kuttler, Radoslaw Madej,
# Anthony G. Basile, Martin Vaeth and Brian Davis.
#

IDBIN=/usr/bin/id
PSBIN=/usr/bin/ps
AWKBIN=/usr/bin/awk
CATBIN=/usr/bin/cat
CUTBIN=/usr/bin/cut
SEDBIN=/usr/bin/sed
FILEBIN=/usr/bin/file
FINDBIN=/usr/bin/find
GREPBIN=/usr/bin/grep
HEADBIN=/usr/bin/head
SORTBIN=/usr/bin/sort
ZCATBIN=/usr/bin/zcat
XARGSBIN=/usr/bin/xargs
READLINKBIN=/usr/bin/readlink

READELFBIN=/usr/bin/readelf
#READELFBIN=/usr/local/bin/readelf-ng

# global vars
have_readelf=1
verbose=false

# FORTIFY_SOURCE vars
FS_end=_chk
FS_cnt_total=0
FS_cnt_checked=0
FS_cnt_unchecked=0
FS_chk_func_libc=0
FS_functions=0
FS_libc=0

# version information
version() {
  echo "checksec v1.5, Tobias Klein, www.trapkit.de, November 2011"
  echo "checksec v1.5.1 Kyle Wickens, github.com/z0mb13w4r, November 2026"
  echo
}

# help
help() {
  echo "Usage: checksec [OPTION]"
  echo
  echo "Options:"
  echo
  echo "  --file <executable-file>"
  echo "  --dir <directory> [-v]"
  echo "  --proc <process name>"
  echo "  --proc-all"
  echo "  --proc-libs <process ID>"
  echo "  --kernel"
  echo "  --fortify-file <executable-file>"
  echo "  --fortify-proc <process ID>"
  echo "  --version"
  echo "  --help"
  echo
  echo "For more information, see:"
  echo "  http://www.trapkit.de/tools/checksec.html"
  echo "  https://github.com/z0mb13w4r/objtools"
  echo
}

# check if command exists
command_exists () {
  type $1  > /dev/null 2>&1;
}

# check if directory exists
dir_exists () {
  if [ -d $1 ] ; then
    return 0
  else
    return 1
  fi
}

# check user privileges
root_privs () {
  if [ $(${IDBIN} -u) -eq 0 ] ; then
    return 0
  else
    return 1
  fi
}

# check if input is numeric
isNumeric () {
  echo "$@" | ${GREPBIN} -q -v "[^0-9]"
}

# check if input is a string
isString () {
  echo "$@" | ${GREPBIN} -q -v "[^A-Za-z]"
}

# check file(s)
filecheck() {
  # check for RELRO support
  if ${READELFBIN} -l $1 2>/dev/null | ${GREPBIN} -q 'GNU_RELRO'; then
    if ${READELFBIN} -d $1 2>/dev/null | ${GREPBIN} -q 'BIND_NOW'; then
      echo -n -e '\033[32mFull RELRO   \033[m   '
    else
      echo -n -e '\033[33mPartial RELRO\033[m   '
    fi
  else
    echo -n -e '\033[31mNo RELRO     \033[m   '
  fi

  # check for stack canary support
  if ${READELFBIN} -s $1 2>/dev/null | ${GREPBIN} -q '__stack_chk_fail'; then
    echo -n -e '\033[32mCanary found   \033[m   '
  else
    echo -n -e '\033[31mNo canary found\033[m   '
  fi

  # check for NX support
  if ${READELFBIN} -W -l $1 2>/dev/null | ${GREPBIN} 'GNU_STACK' | ${GREPBIN} -q 'RWE'; then
    echo -n -e '\033[31mNX disabled\033[m   '
  else
    echo -n -e '\033[32mNX enabled \033[m   '
  fi

  # check for PIE support
  if ${READELFBIN} -h $1 2>/dev/null | ${GREPBIN} -q 'Type:[[:space:]]*EXEC'; then
    echo -n -e '\033[31mNo PIE       \033[m   '
  elif ${READELFBIN} -h $1 2>/dev/null | ${GREPBIN} -q 'Type:[[:space:]]*DYN'; then
    if ${READELFBIN} -d $1 2>/dev/null | ${GREPBIN} -q '(DEBUG)'; then
      echo -n -e '\033[32mPIE enabled  \033[m   '
    else
      echo -n -e '\033[33mDSO          \033[m   '
    fi
  else
    echo -n -e '\033[33mNot an ELF file\033[m   '
  fi

  # check for rpath / run path
  if ${READELFBIN} -d $1 2>/dev/null | ${GREPBIN} -q 'rpath'; then
   echo -n -e '\033[31mRPATH    \033[m  '
  else
   echo -n -e '\033[32mNo RPATH \033[m  '
  fi

  if ${READELFBIN} -d $1 2>/dev/null | ${GREPBIN} -q 'runpath'; then
   echo -n -e '\033[31mRUNPATH    \033[m  '
  else
   echo -n -e '\033[32mNo RUNPATH \033[m  '
  fi
}

# check process(es)
proccheck() {
  # check for RELRO support
  if ${READELFBIN} -l $1/exe 2>/dev/null | ${GREPBIN} -q 'Program Headers'; then
    if ${READELFBIN} -l $1/exe 2>/dev/null | ${GREPBIN} -q 'GNU_RELRO'; then
      if ${READELFBIN} -d $1/exe 2>/dev/null | ${GREPBIN} -q 'BIND_NOW'; then
        echo -n -e '\033[32mFull RELRO       \033[m '
      else
        echo -n -e '\033[33mPartial RELRO    \033[m '
      fi
    else
      echo -n -e '\033[31mNo RELRO         \033[m '
    fi
  else
    echo -n -e '\033[31mPermission denied (please run as root)\033[m\n'
    exit 1
  fi

  # check for stack canary support
  if ${READELFBIN} -s $1/exe 2>/dev/null | ${GREPBIN} -q 'Symbol table'; then
    if ${READELFBIN} -s $1/exe 2>/dev/null | ${GREPBIN} -q '__stack_chk_fail'; then
      echo -n -e '\033[32mCanary found         \033[m  '
    else
      echo -n -e '\033[31mNo canary found      \033[m  '
    fi
  else
    if [ "$1" != "1" ] ; then
      echo -n -e '\033[33mPermission denied    \033[m  '
    else
      echo -n -e '\033[33mNo symbol table found\033[m  '
    fi
  fi

  # first check for PaX support
  if ${CATBIN} $1/status 2> /dev/null | ${GREPBIN} -q 'PaX:'; then
    pageexec=( $(${CATBIN} $1/status 2> /dev/null | ${GREPBIN} 'PaX:' | ${CUTBIN} -b6) )
    segmexec=( $(${CATBIN} $1/status 2> /dev/null | ${GREPBIN} 'PaX:' | ${CUTBIN} -b10) )
    mprotect=( $(${CATBIN} $1/status 2> /dev/null | ${GREPBIN} 'PaX:' | ${CUTBIN} -b8) )
    randmmap=( $(${CATBIN} $1/status 2> /dev/null | ${GREPBIN} 'PaX:' | ${CUTBIN} -b9) )
    if [[ "$pageexec" = "P" || "$segmexec" = "S" ]] && [[ "$mprotect" = "M" && "$randmmap" = "R" ]] ; then
      echo -n -e '\033[32mPaX enabled\033[m   '
    elif [[ "$pageexec" = "p" && "$segmexec" = "s" && "$randmmap" = "R" ]] ; then
      echo -n -e '\033[33mPaX ASLR only\033[m '
    elif [[ "$pageexec" = "P" || "$segmexec" = "S" ]] && [[ "$mprotect" = "m" && "$randmmap" = "R" ]] ; then
      echo -n -e '\033[33mPaX mprot off \033[m'
    elif [[ "$pageexec" = "P" || "$segmexec" = "S" ]] && [[ "$mprotect" = "M" && "$randmmap" = "r" ]] ; then
      echo -n -e '\033[33mPaX ASLR off\033[m  '
    elif [[ "$pageexec" = "P" || "$segmexec" = "S" ]] && [[ "$mprotect" = "m" && "$randmmap" = "r" ]] ; then
      echo -n -e '\033[33mPaX NX only\033[m   '
    else
      echo -n -e '\033[31mPaX disabled\033[m  '
    fi
  # fallback check for NX support
  elif ${READELFBIN} -W -l $1/exe 2>/dev/null | ${GREPBIN} 'GNU_STACK' | ${GREPBIN} -q 'RWE'; then
    echo -n -e '\033[31mNX disabled\033[m   '
  else
    echo -n -e '\033[32mNX enabled \033[m   '
  fi

  # check for PIE support
  if ${READELFBIN} -h $1/exe 2>/dev/null | ${GREPBIN} -q 'Type:[[:space:]]*EXEC'; then
    echo -n -e '\033[31mNo PIE               \033[m   '
  elif ${READELFBIN} -h $1/exe 2>/dev/null | ${GREPBIN} -q 'Type:[[:space:]]*DYN'; then
    if ${READELFBIN} -d $1/exe 2>/dev/null | ${GREPBIN} -q '(DEBUG)'; then
      echo -n -e '\033[32mPIE enabled          \033[m   '
    else
      echo -n -e '\033[33mDynamic Shared Object\033[m   '
    fi
  else
    echo -n -e '\033[33mNot an ELF file      \033[m   '
  fi
}

# check mapped libraries
libcheck() {
  libs=( $(${AWKBIN} '{ print $6 }' /proc/$1/maps | ${GREPBIN} '/' | ${SORTBIN} -u | ${XARGSBIN} ${FILEBIN} | ${GREPBIN} ELF | ${AWKBIN} '{ print $1 }' | ${SEDBIN} 's/:/ /') )

  printf "\n* Loaded libraries (file information, # of mapped files: ${#libs[@]}):\n\n"

  for element in $(seq 0 $((${#libs[@]} - 1)))
  do
    echo "  ${libs[$element]}:"
    echo -n "    "
    filecheck ${libs[$element]}
    printf "\n\n"
  done
}

# check for system-wide ASLR support
aslrcheck() {
  # PaX ASLR support
  if !(${CATBIN} /proc/1/status 2> /dev/null | ${GREPBIN} -q 'Name:') ; then
    echo -n -e ':\033[33m insufficient privileges for PaX ASLR checks\033[m\n'
    echo -n -e '  Fallback to standard Linux ASLR check'
  fi

  if ${CATBIN} /proc/1/status 2> /dev/null | ${GREPBIN} -q 'PaX:'; then
    printf ": "
    if ${CATBIN} /proc/1/status 2> /dev/null | ${GREPBIN} 'PaX:' | ${GREPBIN} -q 'R'; then
      echo -n -e '\033[32mPaX ASLR enabled\033[m\n\n'
    else
      echo -n -e '\033[31mPaX ASLR disabled\033[m\n\n'
    fi
  else
    # standard Linux 'kernel.randomize_va_space' ASLR support
    # (see the kernel file 'Documentation/sysctl/kernel.txt' for a detailed description)
    printf " (kernel.randomize_va_space): "
    if /sbin/sysctl -a 2>/dev/null | ${GREPBIN} -q 'kernel.randomize_va_space = 1'; then
      echo -n -e '\033[33mOn (Setting: 1)\033[m\n\n'
      printf "  Description - Make the addresses of mmap base, stack and VDSO page randomized.\n"
      printf "  This, among other things, implies that shared libraries will be loaded to \n"
      printf "  random addresses. Also for PIE-linked binaries, the location of code start\n"
      printf "  is randomized. Heap addresses are *not* randomized.\n\n"
    elif /sbin/sysctl -a 2>/dev/null | ${GREPBIN} -q 'kernel.randomize_va_space = 2'; then
      echo -n -e '\033[32mOn (Setting: 2)\033[m\n\n'
      printf "  Description - Make the addresses of mmap base, heap, stack and VDSO page randomized.\n"
      printf "  This, among other things, implies that shared libraries will be loaded to random \n"
      printf "  addresses. Also for PIE-linked binaries, the location of code start is randomized.\n\n"
    elif /sbin/sysctl -a 2>/dev/null | ${GREPBIN} -q 'kernel.randomize_va_space = 0'; then
      echo -n -e '\033[31mOff (Setting: 0)\033[m\n'
    else
      echo -n -e '\033[31mNot supported\033[m\n'
    fi
    printf "  See the kernel file 'Documentation/sysctl/kernel.txt' for more details.\n\n"
  fi
}

# check cpu nx flag
nxcheck() {
  if ${GREPBIN} -q nx /proc/cpuinfo; then
    echo -n -e '\033[32mYes\033[m\n\n'
  else
    echo -n -e '\033[31mNo\033[m\n\n'
  fi
}

# check for kernel protection mechanisms
kernelcheck() {
  printf "  Description - List the status of kernel protection mechanisms. Rather than\n"
  printf "  inspect kernel mechanisms that may aid in the prevention of exploitation of\n"
  printf "  userspace processes, this option lists the status of kernel configuration\n"
  printf "  options that harden the kernel itself against attack.\n\n"
  printf "  Kernel config: "

  if [ -f /proc/config.gz ] ; then
    kconfig="${ZCATBIN} /proc/config.gz"
    printf "\033[32m/proc/config.gz\033[m\n\n"
  elif [ -f /boot/config-`uname -r` ] ; then
    kconfig="${CATBIN} /boot/config-`uname -r`"
    printf "\033[33m/boot/config-`uname -r`\033[m\n\n"
    printf "  Warning: The config on disk may not represent running kernel config!\n\n";
  elif [ -f "${KBUILD_OUTPUT:-/usr/src/linux}"/.config ] ; then
    kconfig="${CATBIN} ${KBUILD_OUTPUT:-/usr/src/linux}/.config"
    printf "\033[33m%s\033[m\n\n" "${KBUILD_OUTPUT:-/usr/src/linux}/.config"
    printf "  Warning: The config on disk may not represent running kernel config!\n\n";
  else
    printf "\033[31mNOT FOUND\033[m\n\n"
    exit 0
  fi

  printf "  GCC stack protector support:            "
  if $kconfig | ${GREPBIN} -qi 'CONFIG_CC_STACKPROTECTOR=y'; then
    printf "\033[32mEnabled\033[m\n"
  else
    printf "\033[31mDisabled\033[m\n"
  fi

  printf "  Strict user copy checks:                "
  if $kconfig | ${GREPBIN} -qi 'CONFIG_DEBUG_STRICT_USER_COPY_CHECKS=y'; then
    printf "\033[32mEnabled\033[m\n"
  else
    printf "\033[31mDisabled\033[m\n"
  fi

  printf "  Enforce read-only kernel data:          "
  if $kconfig | ${GREPBIN} -qi 'CONFIG_DEBUG_RODATA=y'; then
    printf "\033[32mEnabled\033[m\n"
  else
    printf "\033[31mDisabled\033[m\n"
  fi
  printf "  Restrict /dev/mem access:               "
  if $kconfig | ${GREPBIN} -qi 'CONFIG_STRICT_DEVMEM=y'; then
    printf "\033[32mEnabled\033[m\n"
  else
    printf "\033[31mDisabled\033[m\n"
  fi

  printf "  Restrict /dev/kmem access:              "
  if $kconfig | ${GREPBIN} -qi 'CONFIG_DEVKMEM=y'; then
    printf "\033[31mDisabled\033[m\n"
  else
    printf "\033[32mEnabled\033[m\n"
  fi

  printf "\n"
  printf "* grsecurity / PaX: "

  if $kconfig | ${GREPBIN} -qi 'CONFIG_GRKERNSEC=y'; then
    if $kconfig | ${GREPBIN} -qi 'CONFIG_GRKERNSEC_HIGH=y'; then
      printf "\033[32mHigh GRKERNSEC\033[m\n\n"
    elif $kconfig | ${GREPBIN} -qi 'CONFIG_GRKERNSEC_MEDIUM=y'; then
      printf "\033[33mMedium GRKERNSEC\033[m\n\n"
    elif $kconfig | ${GREPBIN} -qi 'CONFIG_GRKERNSEC_LOW=y'; then
      printf "\033[31mLow GRKERNSEC\033[m\n\n"
    else
      printf "\033[33mCustom GRKERNSEC\033[m\n\n"
    fi

    printf "  Non-executable kernel pages:            "
    if $kconfig | ${GREPBIN} -qi 'CONFIG_PAX_KERNEXEC=y'; then
      printf "\033[32mEnabled\033[m\n"
    else
      printf "\033[31mDisabled\033[m\n"
    fi

    printf "  Prevent userspace pointer deref:        "
    if $kconfig | ${GREPBIN} -qi 'CONFIG_PAX_MEMORY_UDEREF=y'; then
      printf "\033[32mEnabled\033[m\n"
    else
      printf "\033[31mDisabled\033[m\n"
    fi

    printf "  Prevent kobject refcount overflow:      "
    if $kconfig | ${GREPBIN} -qi 'CONFIG_PAX_REFCOUNT=y'; then
      printf "\033[32mEnabled\033[m\n"
    else
      printf "\033[31mDisabled\033[m\n"
    fi

    printf "  Bounds check heap object copies:        "
    if $kconfig | ${GREPBIN} -qi 'CONFIG_PAX_USERCOPY=y'; then
      printf "\033[32mEnabled\033[m\n"
    else
      printf "\033[31mDisabled\033[m\n"
    fi

    printf "  Disable writing to kmem/mem/port:       "
    if $kconfig | ${GREPBIN} -qi 'CONFIG_GRKERNSEC_KMEM=y'; then
      printf "\033[32mEnabled\033[m\n"
    else
      printf "\033[31mDisabled\033[m\n"
    fi

    printf "  Disable privileged I/O:                 "
    if $kconfig | ${GREPBIN} -qi 'CONFIG_GRKERNSEC_IO=y'; then
      printf "\033[32mEnabled\033[m\n"
    else
      printf "\033[31mDisabled\033[m\n"
    fi

    printf "  Harden module auto-loading:             "
    if $kconfig | ${GREPBIN} -qi 'CONFIG_GRKERNSEC_MODHARDEN=y'; then
      printf "\033[32mEnabled\033[m\n"
    else
      printf "\033[31mDisabled\033[m\n"
    fi

    printf "  Hide kernel symbols:                    "
    if $kconfig | ${GREPBIN} -qi 'CONFIG_GRKERNSEC_HIDESYM=y'; then
      printf "\033[32mEnabled\033[m\n"
    else
      printf "\033[31mDisabled\033[m\n"
    fi
  else
    printf "\033[31mNo GRKERNSEC\033[m\n\n"
    printf "  The grsecurity / PaX patchset is available here:\n"
    printf "    http://grsecurity.net/\n"
  fi

  printf "\n"
  printf "* Kernel Heap Hardening: "

  if $kconfig | ${GREPBIN} -qi 'CONFIG_KERNHEAP=y'; then
    if $kconfig | ${GREPBIN} -qi 'CONFIG_KERNHEAP_FULLPOISON=y'; then
      printf "\033[32mFull KERNHEAP\033[m\n\n"
    else
      printf "\033[33mPartial KERNHEAP\033[m\n\n"
    fi
  else
    printf "\033[31mNo KERNHEAP\033[m\n\n"
    printf "  The KERNHEAP hardening patchset is available here:\n"
    printf "    https://www.subreption.com/kernheap/\n\n"
  fi
}

# --- FORTIFY_SOURCE subfunctions (start) ---

# is FORTIFY_SOURCE supported by libc?
FS_libc_check() {
  printf "* FORTIFY_SOURCE support available (libc)    : "

  if [ "${#FS_chk_func_libc[@]}" != "0" ] ; then
    printf "\033[32mYes\033[m\n"
  else
    printf "\033[31mNo\033[m\n"
    exit 1
  fi
}

# was the binary compiled with FORTIFY_SOURCE?
FS_binary_check() {
  printf "* Binary compiled with FORTIFY_SOURCE support: "

  for FS_elem_functions in $(seq 0 $((${#FS_functions[@]} - 1)))
  do
    if [[ ${FS_functions[$FS_elem_functions]} =~ _chk ]] ; then
      printf "\033[32mYes\033[m\n"
      return
    fi
  done
  printf "\033[31mNo\033[m\n"
  exit 1
}

FS_comparison() {
  echo
  printf " ------ EXECUTABLE-FILE ------- . -------- LIBC --------\n"
  printf " FORTIFY-able library functions | Checked function names\n"
  printf " -------------------------------------------------------\n"

  for FS_elem_libc in $(seq 0 $((${#FS_chk_func_libc[@]} - 1)))
  do
    for FS_elem_functions in $(seq 0 $((${#FS_functions[@]} - 1)))
    do
      FS_tmp_func=${FS_functions[$FS_elem_functions]}
      FS_tmp_libc=${FS_chk_func_libc[$FS_elem_libc]}

      if [[ $FS_tmp_func =~ ^$FS_tmp_libc$ ]] ; then
        printf " \033[31m%-30s\033[m | __%s%s\n" $FS_tmp_func $FS_tmp_libc $FS_end
        let FS_cnt_total++
        let FS_cnt_unchecked++
      elif [[ $FS_tmp_func =~ ^$FS_tmp_libc(_chk) ]] ; then
        printf " \033[32m%-30s\033[m | __%s%s\n" $FS_tmp_func $FS_tmp_libc $FS_end
        let FS_cnt_total++
        let FS_cnt_checked++
      fi

    done
  done
}

FS_summary() {
  echo
  printf "SUMMARY:\n\n"
  printf "* Number of checked functions in libc                : ${#FS_chk_func_libc[@]}\n"
  printf "* Total number of library functions in the executable: ${#FS_functions[@]}\n"
  printf "* Number of FORTIFY-able functions in the executable : %s\n" $FS_cnt_total
  printf "* Number of checked functions in the executable      : \033[32m%s\033[m\n" $FS_cnt_checked
  printf "* Number of unchecked functions in the executable    : \033[31m%s\033[m\n" $FS_cnt_unchecked
  echo
}

# --- FORTIFY_SOURCE subfunctions (end) ---

if !(command_exists ${READELFBIN}) ; then
  printf "\033[31mWarning: '${READELFBIN}' not found! Downgrading enhanced readelf-ng to standard readelf.\033[m\n\n"

  READELFBIN=/usr/bin/readelf
  if !(command_exists ${READELFBIN}) ; then
    printf "\033[31mWarning: '${READELFBIN}' not found! It's required for most checks.\033[m\n\n"
    have_readelf=0
  fi
fi

# parse command-line arguments
case "$1" in

 --version)
  version
  exit 0
  ;;

 --help)
  help
  exit 0
  ;;

 --dir)
  if [ "$3" = "-v" ] ; then
    verbose=true
  fi
  if [ $have_readelf -eq 0 ] ; then
    exit 1
  fi
  if [ -z "$2" ] ; then
    printf "\033[31mError: Please provide a valid directory.\033[m\n\n"
    exit 1
  fi
  # remove trailing slashes
  tempdir=`echo $2 | ${SEDBIN} -e "s/\/*$//"`
  if [ ! -d $tempdir ] ; then
    printf "\033[31mError: The directory '$tempdir' does not exist.\033[m\n\n"
    exit 1
  fi
  cd $tempdir
  printf "RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE\n"
  for N in [A-Za-z]*; do
    if [ "$N" != "[A-Za-z]*" ]; then
      # read permissions?
      if [ ! -r $N ]; then
        printf "\033[31mError: No read permissions for '$tempdir/$N' (run as root).\033[m\n"
      else
        # ELF executable?
        out=`${FILEBIN} $N`
        if [[ ! $out =~ ELF ]] ; then
           if [ "$verbose" = "true" ] ; then
             printf "\033[34m*** Not an ELF file: $tempdir/"
             ${FILEBIN} $N
             printf "\033[m"
           fi
        else
          filecheck $N
          if [ `${FINDBIN} $tempdir/$N \( -perm -004000 -o -perm -002000 \) -type f -print` ]; then
            printf "\033[37;41m%s%s\033[m" $2 $N
          else
            printf "%s%s" $tempdir/ $N
          fi
          echo
        fi
      fi
    fi
  done
  exit 0
  ;;

 --file)
  if [ $have_readelf -eq 0 ] ; then
    exit 1
  fi
  if [ -z "$2" ] ; then
    printf "\033[31mError: Please provide a valid file.\033[m\n\n"
   exit 1
  fi
  # does the file exist?
  if [ ! -e $2 ] ; then
    printf "\033[31mError: The file '$2' does not exist.\033[m\n\n"
    exit 1
  fi
  # read permissions?
  if [ ! -r $2 ] ; then
    printf "\033[31mError: No read permissions for '$2' (run as root).\033[m\n\n"
    exit 1
  fi
  # ELF executable?
  out=`${FILEBIN} $2`
  if [[ ! $out =~ ELF ]] ; then
    printf "\033[31mError: Not an ELF file: "
    ${FILEBIN} $2
    printf "\033[m\n"
    exit 1
  fi
  printf "RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE\n"
  filecheck $2
  if [ `${FINDBIN} $2 \( -perm -004000 -o -perm -002000 \) -type f -print` ] ; then
    printf "\033[37;41m%s%s\033[m" $2 $N
  else
    printf "%s" $2
  fi
  echo
  exit 0
  ;;

 --proc-all)
  if [ $have_readelf -eq 0 ] ; then
    exit 1
  fi
  cd /proc
  printf "* System-wide ASLR"
  aslrcheck
  printf "* Does the CPU support NX: "
  nxcheck
  printf "         COMMAND    PID RELRO             STACK CANARY           NX/PaX        PIE\n"
  for N in [1-9]*; do
    if [ $N != $$ ] && ${READLINKBIN} -q $N/exe > /dev/null; then
      printf "%16s" `${HEADBIN} -1 $N/status | ${CUTBIN} -b 7-`
      printf "%7d " $N
      proccheck $N
      echo
    fi
  done
  if [ ! -e /usr/bin/id ] ; then
    printf "\n\033[33mNote: If you are running 'checksec.sh' as an unprivileged user, you\n"
    printf "      will not see all processes. Please run the script as root.\033[m\n\n"
  else
    if !(root_privs) ; then
      printf "\n\033[33mNote: You are running 'checksec.sh' as an unprivileged user.\n"
      printf "      Too see all processes, please run the script as root.\033[m\n\n"
    fi
  fi
  exit 0
  ;;

 --proc)
  if [ $have_readelf -eq 0 ] ; then
    exit 1
  fi
  if [ -z "$2" ] ; then
    printf "\033[31mError: Please provide a valid process name.\033[m\n\n"
    exit 1
  fi
  if !(isString "$2") ; then
     printf "\033[31mError: Please provide a valid process name.\033[m\n\n"
     exit 1
  fi
  cd /proc
  printf "* System-wide ASLR"
  aslrcheck
  printf "* Does the CPU support NX: "
  nxcheck
  printf "         COMMAND    PID RELRO             STACK CANARY           NX/PaX        PIE\n"
  for N in `${PSBIN} -Ao pid,comm | ${GREPBIN} $2 | ${CUTBIN} -b1-6`; do
    if [ -d $N ] ; then
      printf "%16s" `${HEADBIN} -1 $N/status | ${CUTBIN} -b 7-`
      printf "%7d " $N
      # read permissions?
      if [ ! -r $N/exe ] ; then
        if !(root_privs) ; then
          printf "\033[31mNo read permissions for '/proc/$N/exe' (run as root).\033[m\n\n"
          exit 1
        fi
        if [ ! `${READLINKBIN} $N/exe` ] ; then
          printf "\033[31mPermission denied. Requested process ID belongs to a kernel thread.\033[m\n\n"
          exit 1
        fi
        exit 1
      fi
      proccheck $N
      echo
    fi
  done
  exit 0
  ;;

 --proc-libs)
  if [ $have_readelf -eq 0 ] ; then
    exit 1
  fi
  if [ -z "$2" ] ; then
    printf "\033[31mError: Please provide a valid process ID.\033[m\n\n"
    exit 1
  fi
  if !(isNumeric "$2") ; then
     printf "\033[31mError: Please provide a valid process ID.\033[m\n\n"
     exit 1
  fi
  cd /proc
  printf "* System-wide ASLR"
  aslrcheck
  printf "* Does the CPU support NX: "
  nxcheck
  printf "* Process information:\n\n"
  printf "         COMMAND    PID RELRO             STACK CANARY           NX/PaX        PIE\n"
  N=$2
  if [ -d $N ] ; then
    printf "%16s" `${HEADBIN} -1 $N/status | ${CUTBIN} -b 7-`
    printf "%7d " $N
    # read permissions?
    if [ ! -r $N/exe ] ; then
      if !(root_privs) ; then
        printf "\033[31mNo read permissions for '/proc/$N/exe' (run as root).\033[m\n\n"
        exit 1
      fi
      if [ ! `${READLINKBIN} $N/exe` ] ; then
        printf "\033[31mPermission denied. Requested process ID belongs to a kernel thread.\033[m\n\n"
        exit 1
      fi
      exit 1
    fi
    proccheck $N
    echo
    libcheck $N
  fi
  exit 0
  ;;

 --kernel)
  cd /proc
  printf "* Kernel protection information:\n\n"
  kernelcheck
  exit 0
  ;;

 --fortify-file)
  if [ $have_readelf -eq 0 ] ; then
    exit 1
  fi
  if [ -z "$2" ] ; then
    printf "\033[31mError: Please provide a valid file.\033[m\n\n"
   exit 1
  fi
  # does the file exist?
  if [ ! -e $2 ] ; then
    printf "\033[31mError: The file '$2' does not exist.\033[m\n\n"
    exit 1
  fi
  # read permissions?
  if [ ! -r $2 ] ; then
    printf "\033[31mError: No read permissions for '$2' (run as root).\033[m\n\n"
    exit 1
  fi
  # ELF executable?
  out=`${FILEBIN} $2`
  if [[ ! $out =~ ELF ]] ; then
    printf "\033[31mError: Not an ELF file: "
    ${FILEBIN} $2
    printf "\033[m\n"
    exit 1
  fi
  if [ -e /lib/libc.so.6 ] ; then
    FS_libc=/lib/libc.so.6
  elif [ -e /lib64/libc.so.6 ] ; then
    FS_libc=/lib64/libc.so.6
  elif [ -e /lib/i386-linux-gnu/libc.so.6 ] ; then
    FS_libc=/lib/i386-linux-gnu/libc.so.6
  elif [ -e /lib/x86_64-linux-gnu/libc.so.6 ] ; then
    FS_libc=/lib/x86_64-linux-gnu/libc.so.6
  else
    printf "\033[31mError: libc not found.\033[m\n\n"
    exit 1
  fi

  FS_chk_func_libc=( $(${READELFBIN} -s $FS_libc | ${GREPBIN} _chk@@ | ${AWKBIN} '{ print $8 }' | ${CUTBIN} -c 3- | ${SEDBIN} -e 's/_chk@.*//') )
  FS_functions=( $(${READELFBIN} -s $2 | ${AWKBIN} '{ print $8 }' | ${SEDBIN} 's/_*//' | ${SEDBIN} -e 's/@.*//') )

  FS_libc_check
  FS_binary_check
  FS_comparison
  FS_summary

  exit 0
  ;;

 --fortify-proc)
  if [ $have_readelf -eq 0 ] ; then
    exit 1
  fi
  if [ -z "$2" ] ; then
    printf "\033[31mError: Please provide a valid process ID.\033[m\n\n"
    exit 1
  fi
  if !(isNumeric "$2") ; then
     printf "\033[31mError: Please provide a valid process ID.\033[m\n\n"
     exit 1
  fi
  cd /proc
  N=$2
  if [ -d $N ] ; then
    # read permissions?
    if [ ! -r $N/exe ] ; then
      if !(root_privs) ; then
        printf "\033[31mNo read permissions for '/proc/$N/exe' (run as root).\033[m\n\n"
        exit 1
      fi
      if [ ! `${READLINKBIN} $N/exe` ] ; then
        printf "\033[31mPermission denied. Requested process ID belongs to a kernel thread.\033[m\n\n"
        exit 1
      fi
      exit 1
    fi
    if [ -e /lib/libc.so.6 ] ; then
      FS_libc=/lib/libc.so.6
    elif [ -e /lib64/libc.so.6 ] ; then
      FS_libc=/lib64/libc.so.6
    elif [ -e /lib/i386-linux-gnu/libc.so.6 ] ; then
      FS_libc=/lib/i386-linux-gnu/libc.so.6
    elif [ -e /lib/x86_64-linux-gnu/libc.so.6 ] ; then
      FS_libc=/lib/x86_64-linux-gnu/libc.so.6
    else
      printf "\033[31mError: libc not found.\033[m\n\n"
      exit 1
    fi
    printf "* Process name (PID)                         : %s (%d)\n" `${HEADBIN} -1 $N/status | ${CUTBIN} -b 7-` $N
    FS_chk_func_libc=( $(${READELFBIN} -s $FS_libc | ${GREPBIN} _chk@@ | ${AWKBIN} '{ print $8 }' | ${CUTBIN} -c 3- | ${SEDBIN} -e 's/_chk@.*//') )
    FS_functions=( $(${READELFBIN} -s $2/exe | ${AWKBIN} '{ print $8 }' | ${SEDBIN} 's/_*//' | ${SEDBIN} -e 's/@.*//') )

    FS_libc_check
    FS_binary_check
    FS_comparison
    FS_summary
  fi
  exit 0
  ;;

 *)
  if [ "$#" != "0" ] ; then
    printf "\033[31mError: Unknown option '$1'.\033[m\n\n"
  fi
  help
  exit 1
  ;;
esac

