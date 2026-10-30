#!/bin/bash

SCRIPTVERSION=1.0
SCRIPTNAME=$(basename "$0")

LSBIN=/usr/bin/ls
CATBIN=/usr/bin/cat
CUTBIN=/usr/bin/cut
FILEBIN=/usr/bin/file
FINDBIN=/usr/bin/find
GREPBIN=/usr/bin/grep
SORTBIN=/usr/bin/sort
EGREPBIN=/usr/bin/egrep
FDISKBIN=/usr/sbin/fdisk
GDISKBIN=/usr/sbin/gdisk

XARGSBIN=/usr/bin/xargs
BINWALKBIN=/usr/bin/binwalk
HEXDUMPBIN=/usr/bin/hexdump
STRINGSBIN=/usr/bin/strings

USERNAME=${SUDO_USER}
PATHNAME=

function zshow() {
  echo -e "\e[00;32m${1}:\e[00m"
  echo -e "${2}\n"
}

function zcheck() {
  if [ "${2}" ]; then
    zshow "${1}" "${2}"
  fi
}

function zdo() {
  xx=$(${1})
  if [ "${xx}" ]; then
    echo -e "\e[00;32m${1}:\e[00m"
    echo -e "${xx}\n"
  fi
}

function show_strings() {
  echo -e "\e[00;33m### STRINGS FILES ########################################\e[00m"

  #${STRINGSBIN} ${PATHNAME}
  #${STRINGSBIN} -n5 ${PATHNAME}
  zdo "${STRINGSBIN} -n16 ${PATHNAME}"
  #${STRINGSBIN} -tx ${PATHNAME}

  echo -e '\n' && echo -e "\e[00;33m### SCAN COMPLETE ########################################\e[00m"
}

function show_this() {
  echo -e "\e[00;33m### SYSTEM FILES #########################################\e[00m"

  zdo "${FILEBIN} ${PATHNAME}"
  zdo "${BINWALKBIN} ${PATHNAME}"
  #${BINWALKBIN} -E ${PATHNAME}
  zdo "${HEXDUMPBIN} -C -n 512 ${PATHNAME}"
  #${HEXDUMPBIN} -C ${PATHNAME} | head
  #zdo "${GDISKBIN} -l ${PATHNAME}"
  zdo "${FDISKBIN} -lu ${PATHNAME}"

  echo -e "\e[00;33m### SCAN COMPLETE ########################################\e[00m"
}

function show_usage() {
  ${CATBIN} << EOF
Usage: ${SCRIPTNAME} v${SCRIPTVERSION}
EXAMPLES:
  ${SCRIPTNAME} -v|--version
  ${SCRIPTNAME} -h|--help
EOF
}

function show_license() {
  ${CATBIN} << EOF
  COPYRIGHT
  MIT License

  Copyright (c) 2026 Kyle Wickens

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
EOF
}

function show_version() {
  echo ${SCRIPTNAME} v${SCRIPTVERSION}
}

parse_args() {
  while [[ $# -gt 0 ]]; do
    case ${1} in
      strings)
        COMMAND=${1}
        shift # past argument
        ;;
      -v|--version)
        COMMAND=version
        shift # past argument
        ;;
      -h|--help)
        COMMAND=help
        shift # past argument
        ;;
      *)
        if [[ $# -eq 1 ]]; then
          PATHNAME=${1}
          shift # past argument
        else
          echo "Unknown option ${1}"
          show_usage
          exit 1
        fi
        ;;
    esac
  done
}

main() {
  parse_args "$@"

  if [ "${COMMAND}" == "help" ]; then
    show_usage
    exit 0
  elif [ "${COMMAND}" == "version" ]; then
    show_version
    echo
    show_license
    exit 0
  fi

  show_version
  if [ "${COMMAND}" == "strings" ]; then
    show_strings
  else
    show_this
  fi
}

main "$@"

