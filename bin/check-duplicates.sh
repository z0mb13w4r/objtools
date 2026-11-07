#!/bin/bash

SCRIPTVERSION=1.0
SCRIPTNAME=$(basename "$0")

CUTBIN=/usr/bin/cut
FINDBIN=/usr/bin/find
GREPBIN=/usr/bin/grep
SORTBIN=/usr/bin/sort
UNIQBIN=/usr/bin/uniq
MD5SUMBIN=/usr/bin/md5sum

THISNAME=/tmp/md5sum.txt
PATHNAME=/

function show_this() {
  if [ "${2}" == "recursive" ]; then
    ${FINDBIN} . -name '*' -type f -exec ${MD5SUMBIN} {} \; | ${SORTBIN} > ${THISNAME}
  else
    ${MD5SUMBIN} * | ${SORTBIN} > ${THISNAME}
  fi
  ${CUTBIN} -d ' ' -f 1 ${THISNAME} | ${UNIQBIN} -Du | ${GREPBIN} -f - ${THISNAME}
}

function show_usage() {
  ${CATBIN} << EOF
Usage: ${SCRIPTNAME} v${SCRIPTVERSION}
EXAMPLES:
  ${SCRIPTNAME} -r|--recursive
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
      -r|--recursive)
        COMMAND=recursive
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
          if [ "${1: -1}" == "/" ]; then
            PATHNAME=${1}
          else
            PATHNAME=${1}/
          fi
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
  elif [ "${COMMAND}" == "version" ]; then
    show_version
    echo
    show_license
  else
    show_version
    show_this "${PATHNAME}" "${COMMAND}"
  fi
}

main "$@"

