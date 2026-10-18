#!/bin/bash

SCRIPTVERSION=1.1
SCRIPTNAME=$(basename "$0")

LSBIN=/usr/bin/ls
CATBIN=/usr/bin/cat
CUTBIN=/usr/bin/cut
FINDBIN=/usr/bin/find
GREPBIN=/usr/bin/grep
SORTBIN=/usr/bin/sort
EGREPBIN=/usr/bin/egrep
XARGSBIN=/usr/bin/xargs

USERNAME=${SUDO_USER}

function zshow() {
  echo -e "\e[00;32m${1}:\e[00m"
  echo -e "${2}\n"
}

function zcheck() {
  if [ "${2}" ]; then
    zshow "${1}" "${2}"
  fi
}

function zcat() {
  xx=$(${CATBIN} "${1}" 2>/dev/null)
  zcheck "cat ${1}" "${xx}"
}

function zls() {
  xx=$(${LSBIN} -al ${1} 2>/dev/null)
  zcheck "ls -al ${1}" "${xx}"
}

function show_this() {
  echo -e "\e[00;33m### SYSTEM FILES #########################################\e[00m"
  zls  "${PATHNAME}/etc/"
  zcat "${PATHNAME}/etc/hostname"
  zcat "${PATHNAME}/etc/hosts"
  zcat "${PATHNAME}/etc/issue"
  zcat "${PATHNAME}/etc/os-release"
  zcat "${PATHNAME}/etc/mtab"
  zcat "${PATHNAME}/etc/fstab"
  zcat "${PATHNAME}/etc/group"
  zcat "${PATHNAME}/etc/passwd"
  zcat "${PATHNAME}/etc/shadow"
  zcat "${PATHNAME}/etc/inetd.conf"
  zcat "${PATHNAME}/etc/inittab"
  zcat "${PATHNAME}/etc/mdev.conf"
  zcat "${PATHNAME}/etc/profile"
  zcat "${PATHNAME}/etc/profile_prjcfg"
  zcat "${PATHNAME}/etc/services"
  zcat "${PATHNAME}/etc/udhcpd.conf"
  zcat "${PATHNAME}/etc/udhcpdw.conf"
  zcat "${PATHNAME}/etc/wifiap_wpa2.conf"
  zcat "${PATHNAME}/etc/wpa_supplicant.conf"
  zcat "${PATHNAME}/etc/bootchartd.conf"

  echo -e "\e[00;33m### PASSWORD FILES #######################################\e[00m"
  ${FINDBIN} ${PATHNAME} -name passwd
  ${FINDBIN} ${PATHNAME} -name shadow
  ${FINDBIN} ${PATHNAME} -name *.psk

  echo && echo -e "\e[00;33m### UNIX-MD5 HASHES ######################################\e[00m"
  ${EGREPBIN} -sro '\$1\$\w{8}\S{23}' ${PATHNAME}

  echo && echo -e "\e[00;33m### SSL RELATED FILES ####################################\e[00m"
  ${FINDBIN} ${PATHNAME} -name *.crt
  ${FINDBIN} ${PATHNAME} -name *.pem
  ${FINDBIN} ${PATHNAME} -name *.cer
  ${FINDBIN} ${PATHNAME} -name *.p7b
  ${FINDBIN} ${PATHNAME} -name *.p12
  ${FINDBIN} ${PATHNAME} -name *.key

  echo && echo -e "\e[00;33m### SSH RELATED FILES ####################################\e[00m"
  ${FINDBIN} ${PATHNAME} -name authorized_keys
  ${FINDBIN} ${PATHNAME} -name *authorized_keys*
  ${FINDBIN} ${PATHNAME} -name host_key
  ${FINDBIN} ${PATHNAME} -name *host_key*
  ${FINDBIN} ${PATHNAME} -name id_rsa
  ${FINDBIN} ${PATHNAME} -name *id_rsa*
  ${FINDBIN} ${PATHNAME} -name id_dsa
  ${FINDBIN} ${PATHNAME} -name *id_dsa*
  ${FINDBIN} ${PATHNAME} -name *.pub

  echo && echo -e "\e[00;33m### CONFIG RELATED FILES #################################\e[00m"
  ${FINDBIN} ${PATHNAME} -name *.conf
  ${FINDBIN} ${PATHNAME} -name *.cfg
  ${FINDBIN} ${PATHNAME} -name *.ini

  echo && echo -e "\e[00;33m### DATABASE RELATED FILES ###############################\e[00m"
  ${FINDBIN} ${PATHNAME} -name *.db
  ${FINDBIN} ${PATHNAME} -name *.sqlite
  ${FINDBIN} ${PATHNAME} -name *.sqlite3
  ${FINDBIN} ${PATHNAME} -name *.sqlitedb
  ${FINDBIN} ${PATHNAME} -name *.store
  ${FINDBIN} ${PATHNAME} -name *.sql

  echo && echo -e "\e[00;33m### SHELL SCRIPTS ########################################\e[00m"
  ${FINDBIN} ${PATHNAME} -name *.sh

  echo && echo -e "\e[00;33m### BINARY FILES #########################################\e[00m"
  ${FINDBIN} ${PATHNAME} -name *.bin

  echo && echo -e "\e[00;33m### PATTERN IN FILES #####################################\e[00m"
  ${GREPBIN} -sirnow ${PATHNAME} -e "upgrade" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "admin" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "root" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "passwd" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "pwd" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "dropbear" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "ssl" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "private key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "telnet" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "secret" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "pgp" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "gpg" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "token" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "api key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "oauth" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "AKIA" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u

  echo && echo -e "\e[00;33m### WEB SERVERS ##########################################\e[00m"
  ${FINDBIN} ${PATHNAME} -name apache
  ${FINDBIN} ${PATHNAME} -name lighttpd
  ${FINDBIN} ${PATHNAME} -name alphapd
  ${FINDBIN} ${PATHNAME} -name httpd
  ${FINDBIN} ${PATHNAME} -name GoAhead
  ${FINDBIN} ${PATHNAME} -name nginx

  echo && echo -e "\e[00;33m### COMMAND INJECTION FUNCTIONS ##########################\e[00m"
  ${GREPBIN} -sirnow ${PATHNAME} -e "system" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "exec" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "execl" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "popen" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "execve" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "ShellExecute" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | sort -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "os.execute" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "os.execute" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "subprocess.call" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "eval" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "cmd" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u

  echo && echo -e "\e[00;33m### BANNED C FUNCTIONS ###################################\e[00m"
  ${GREPBIN} -sirnow ${PATHNAME} -e "strcpy" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "strcat" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "strncat" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "sprintf" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "vsprintf" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "strlen" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "memcpy" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "memcmp" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "memset" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "fopen" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "gets" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "getwd" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "scanf" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u

  echo && echo -e "\e[00;33m### IMPORTANT BINARIES ###################################\e[00m"
  ${FINDBIN} ${PATHNAME} -name "ssh" 2>/dev/null | ${XARGSBIN} -r ${LSBIN} -al 2>/dev/null
  ${FINDBIN} ${PATHNAME} -name "sshd" 2>/dev/null | ${XARGSBIN} -r ${LSBIN} -al 2>/dev/null
  ${FINDBIN} ${PATHNAME} -name "scp" 2>/dev/null | ${XARGSBIN} -r ${LSBIN} -al 2>/dev/null
  ${FINDBIN} ${PATHNAME} -name "sftp" 2>/dev/null | ${XARGSBIN} -r ${LSBIN} -al 2>/dev/null
  ${FINDBIN} ${PATHNAME} -name "sftp" 2>/dev/null | ${XARGSBIN} -r ${LSBIN} -al 2>/dev/null
  ${FINDBIN} ${PATHNAME} -name "tftp" 2>/dev/null | ${XARGSBIN} -r ${LSBIN} -al 2>/dev/null
  ${FINDBIN} ${PATHNAME} -name "dropbear" 2>/dev/null | ${XARGSBIN} -r ${LSBIN} -al 2>/dev/null
  ${FINDBIN} ${PATHNAME} -name "busybox" 2>/dev/null | ${XARGSBIN} -r ${LSBIN} -al 2>/dev/null
  ${FINDBIN} ${PATHNAME} -name "telnet" 2>/dev/null | ${XARGSBIN} -r ${LSBIN} -al 2>/dev/null
  ${FINDBIN} ${PATHNAME} -name "telnetd" 2>/dev/null | ${XARGSBIN} -r ${LSBIN} -al 2>/dev/null
  ${FINDBIN} ${PATHNAME} -name "openssl" 2>/dev/null | ${XARGSBIN} -r ${LSBIN} -al 2>/dev/null
  ${FINDBIN} ${PATHNAME} -name "ftpget" 2>/dev/null | ${XARGSBIN} -r ${LSBIN} -al 2>/dev/null
  ${FINDBIN} ${PATHNAME} -name "ftpput" 2>/dev/null | ${XARGSBIN} -r ${LSBIN} -al 2>/dev/null

  echo && echo -e "\e[00;33m### WEBFILES #############################################\e[00m"
  ${FINDBIN} ${PATHNAME} -name "*.asp"
  ${FINDBIN} ${PATHNAME} -name "*.html"
  ${FINDBIN} ${PATHNAME} -name "*.xml"
  ${FINDBIN} ${PATHNAME} -name "*.php"
  ${FINDBIN} ${PATHNAME} -name "*.js"
  ${FINDBIN} ${PATHNAME} -name "*.json"
  ${FINDBIN} ${PATHNAME} -name "*.cgi"

  echo && echo -e "\e[00;33m### IP ADDRESSES #########################################\e[00m"
  ${GREPBIN} -sRIEho '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' --exclude-dir='dev' ${PATHNAME} | ${SORTBIN} -u

  echo && echo -e "\e[00;33m### URLS #################################################\e[00m"
  ${GREPBIN} -sRIEoh '(http|https)://[^/"]+' --exclude-dir='dev' ${PATHNAME} | ${SORTBIN} -u
  ${GREPBIN} -sRIEoh '(ftp|ftps|tftp)://[^/"]+' --exclude-dir='dev' ${PATHNAME} | ${SORTBIN} -u

  echo && echo -e "\e[00;33m### EMAILS ###############################################\e[00m"
  ${GREPBIN} -sRIEoh '([[:alnum:]_.-]+@[[:alnum:]_.-]+?\.[[:alpha:].]{2,6})' "$@" --exclude-dir='dev' ${PATHNAME} | ${SORTBIN} -u
}

function show_usage() {
  ${CATBIN} << EOF
Usage: ${SCRIPTNAME} v${SCRIPTVERSION}
EXAMPLES:
  ${SCRIPTNAME} user
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
      user)
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
      -u|--username)
        USERNAME=${2}
        shift # past argument
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

  show_this
}

main "$@"

