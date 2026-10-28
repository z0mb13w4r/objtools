#!/bin/bash

SCRIPTVERSION=1.2
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
PATHNAME=/

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

function zfind() {
  xx=$(${FINDBIN} "${1}" -name "${2}" 2>/dev/null)
  if [ "${xx}" ]; then
    echo -e "${xx}"
  fi
}

function zgrep() {
  xx=$(${GREPBIN} -sirnow "${1}" -e "${2}" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u)
  if [ "${xx}" ]; then
    echo -e "${xx}"
  fi
}

function zls() {
  xx=$(${LSBIN} -al ${1} 2>/dev/null)
  zcheck "ls -al ${1}" "${xx}"
}

function show_this() {
  echo -e "\e[00;33m### SYSTEM FILES #########################################\e[00m"
  zls  "${PATHNAME%?}/etc/"
  zcat "${PATHNAME%?}/etc/hostname"
  zcat "${PATHNAME%?}/etc/hosts"
  zcat "${PATHNAME%?}/etc/issue"
  zcat "${PATHNAME%?}/etc/os-release"
  zcat "${PATHNAME%?}/etc/mtab"
  zcat "${PATHNAME%?}/etc/fstab"
  zcat "${PATHNAME%?}/etc/group"
  zcat "${PATHNAME%?}/etc/passwd"
  zcat "${PATHNAME%?}/etc/shadow"
  zcat "${PATHNAME%?}/etc/inetd.conf"
  zcat "${PATHNAME%?}/etc/inittab"
  zcat "${PATHNAME%?}/etc/mdev.conf"
  zcat "${PATHNAME%?}/etc/profile"
  zcat "${PATHNAME%?}/etc/profile_prjcfg"
  zcat "${PATHNAME%?}/etc/services"
  zcat "${PATHNAME%?}/etc/udhcpd.conf"
  zcat "${PATHNAME%?}/etc/udhcpdw.conf"
  zcat "${PATHNAME%?}/etc/wifiap_wpa2.conf"
  zcat "${PATHNAME%?}/etc/wpa_supplicant.conf"
  zcat "${PATHNAME%?}/etc/bootchartd.conf"
  zcat "${PATHNAME%?}/proc/sys/kernel/hostname"
  zcat "${PATHNAME%?}/proc/sys/kernel/osrelease"
  zcat "${PATHNAME%?}/proc/sys/kernel/ostype"
  zcat "${PATHNAME%?}/proc/version"

  echo -e "\e[00;33m### PASSWORD FILES #######################################\e[00m"
  zfind "${PATHNAME}" "passwd"
  zfind "${PATHNAME}" "shadow"
  zfind "${PATHNAME}" "*.psk"

  echo && echo -e "\e[00;33m### UNIX-MD5 HASHES ######################################\e[00m"
  ${EGREPBIN} -sro '\$1\$\w{8}\S{23}' ${PATHNAME} 2>/dev/null

  echo && echo -e "\e[00;33m### SSL RELATED FILES ####################################\e[00m"
  zls  "${PATHNAME%?}/etc/ssl"

  zfind "${PATHNAME}" "*.crt"
  zfind "${PATHNAME}" "*.pem"
  zfind "${PATHNAME}" "*.cer"
  zfind "${PATHNAME}" "*.p7b"
  zfind "${PATHNAME}" "*.p12"
  zfind "${PATHNAME}" "*.key"

  echo && echo -e "\e[00;33m### SSH RELATED FILES ####################################\e[00m"
  zfind "${PATHNAME}" "authorized_keys"
  zfind "${PATHNAME}" "*authorized_keys*"
  zfind "${PATHNAME}" "host_key"
  zfind "${PATHNAME}" "*host_key*"
  zfind "${PATHNAME}" "id_rsa"
  zfind "${PATHNAME}" "*id_rsa*"
  zfind "${PATHNAME}" "id_dsa"
  zfind "${PATHNAME}" "*id_dsa*"
  zfind "${PATHNAME}" "*.pub"

  echo && echo -e "\e[00;33m### CONFIG RELATED FILES #################################\e[00m"
  zfind "${PATHNAME}" "*.conf"
  zfind "${PATHNAME}" "*.cfg"
  zfind "${PATHNAME}" "*.ini"

  echo && echo -e "\e[00;33m### DATABASE RELATED FILES ###############################\e[00m"
  zfind "${PATHNAME}" "*.db"
  zfind "${PATHNAME}" "*.sqlite"
  zfind "${PATHNAME}" "*.sqlite3"
  zfind "${PATHNAME}" "*.sqlitedb"
  zfind "${PATHNAME}" "*.store"
  zfind "${PATHNAME}" "*.sql"
  zfind "${PATHNAME}" "*.mdb"
  zfind "${PATHNAME}" "*.mdf"
  zfind "${PATHNAME}" "*.myd"
  zfind "${PATHNAME}" "*.dbf"
  zfind "${PATHNAME}" "*.wdb"

  echo && echo -e "\e[00;33m### SHELL SCRIPTS ########################################\e[00m"
  zfind "${PATHNAME}" "*.sh"

  echo && echo -e "\e[00;33m### BINARY FILES #########################################\e[00m"
  zfind "${PATHNAME}" "*.bin"

  echo && echo -e "\e[00;33m### PATTERN IN FILES #####################################\e[00m"
  zgrep "${PATHNAME}" "upgrade"
  zgrep "${PATHNAME}" "admin"
  zgrep "${PATHNAME}" "root"
  zgrep "${PATHNAME}" "password"
  zgrep "${PATHNAME}" "passwd"
  zgrep "${PATHNAME}" "pwd"
  zgrep "${PATHNAME}" "dropbear"
  zgrep "${PATHNAME}" "ssl"
  zgrep "${PATHNAME}" "private key"
  zgrep "${PATHNAME}" "telnet"
  zgrep "${PATHNAME}" "secret"
  zgrep "${PATHNAME}" "pgp"
  zgrep "${PATHNAME}" "gpg"
  zgrep "${PATHNAME}" "token"
  zgrep "${PATHNAME}" "api key"
  zgrep "${PATHNAME}" "oauth"
  zgrep "${PATHNAME}" "AKIA"
  zgrep "${PATHNAME}" "cmd="
  zgrep "${PATHNAME}" "exec="
  zgrep "${PATHNAME}" "command="
  zgrep "${PATHNAME}" "execute="
  zgrep "${PATHNAME}" "AccessKey"
  zgrep "${PATHNAME}" "AccessKeySecret"
  zgrep "${PATHNAME}" "access_key"
  zgrep "${PATHNAME}" "access_token"
  zgrep "${PATHNAME}" "admin_pass"
  zgrep "${PATHNAME}" "admin_user"
  zgrep "${PATHNAME}" "algolia_admin_key"
  zgrep "${PATHNAME}" "algolia_api_key"
  zgrep "${PATHNAME}" "alias_pass"
  zgrep "${PATHNAME}" "alicloud_access_key"
  zgrep "${PATHNAME}" "amazon_secret_access_key"
  zgrep "${PATHNAME}" "amazonaws"
  zgrep "${PATHNAME}" "ansible_vault_password"
  zgrep "${PATHNAME}" "aos_key"
  zgrep "${PATHNAME}" "api_key"
  zgrep "${PATHNAME}" "api_key_secret"
  zgrep "${PATHNAME}" "api_key_sid"
  zgrep "${PATHNAME}" "api_secret"
  zgrep "${PATHNAME}" "api.googlemaps"
  zgrep "${PATHNAME}" "apikey"
  zgrep "${PATHNAME}" "apiSecret"
  zgrep "${PATHNAME}" "app_debug"
  zgrep "${PATHNAME}" "app_id"
  zgrep "${PATHNAME}" "app_key"
  zgrep "${PATHNAME}" "app_log_level"
  zgrep "${PATHNAME}" "app_secret"
  zgrep "${PATHNAME}" "appkey"
  zgrep "${PATHNAME}" "appkeysecret"
  zgrep "${PATHNAME}" "application_key"
  zgrep "${PATHNAME}" "appsecret"
  zgrep "${PATHNAME}" "auth_token"
  zgrep "${PATHNAME}" "authorizationToken"
  zgrep "${PATHNAME}" "authsecret"
  zgrep "${PATHNAME}" "aws_access"
  zgrep "${PATHNAME}" "aws_access_key_id"
  zgrep "${PATHNAME}" "aws_bucket"
  zgrep "${PATHNAME}" "aws_key"
  zgrep "${PATHNAME}" "aws_secret"
  zgrep "${PATHNAME}" "aws_secret_key"
  zgrep "${PATHNAME}" "aws_token"
  zgrep "${PATHNAME}" "AWSSecretKey"
  zgrep "${PATHNAME}" "b2_app_key"
  zgrep "${PATHNAME}" "bashrc password"
  zgrep "${PATHNAME}" "bintray_apikey"
  zgrep "${PATHNAME}" "bintray_gpg_password"
  zgrep "${PATHNAME}" "bintray_key"
  zgrep "${PATHNAME}" "bintraykey"
  zgrep "${PATHNAME}" "bluemix_api_key"
  zgrep "${PATHNAME}" "bluemix_pass"
  zgrep "${PATHNAME}" "browserstack_access_key"
  zgrep "${PATHNAME}" "bucket_password"
  zgrep "${PATHNAME}" "bucketeer_aws_access_key_id"
  zgrep "${PATHNAME}" "bucketeer_aws_secret_access_key"
  zgrep "${PATHNAME}" "built_branch_deploy_key"
  zgrep "${PATHNAME}" "bx_password"
  zgrep "${PATHNAME}" "cache_driver"
  zgrep "${PATHNAME}" "cache_s3_secret_key"
  zgrep "${PATHNAME}" "cattle_access_key"
  zgrep "${PATHNAME}" "cattle_secret_key"
  zgrep "${PATHNAME}" "certificate_password"
  zgrep "${PATHNAME}" "ci_deploy_password"
  zgrep "${PATHNAME}" "client_secret"
  zgrep "${PATHNAME}" "client_zpk_secret_key"
  zgrep "${PATHNAME}" "clojars_password"
  zgrep "${PATHNAME}" "cloud_api_key"
  zgrep "${PATHNAME}" "cloud_watch_aws_access_key"
  zgrep "${PATHNAME}" "cloudant_password"
  zgrep "${PATHNAME}" "cloudflare_api_key"
  zgrep "${PATHNAME}" "cloudflare_auth_key"
  zgrep "${PATHNAME}" "cloudinary_api_secret"
  zgrep "${PATHNAME}" "cloudinary_name"
  zgrep "${PATHNAME}" "codecov_token"
  zgrep "${PATHNAME}" "config"
  zgrep "${PATHNAME}" "conn.login"
  zgrep "${PATHNAME}" "connectionstring"
  zgrep "${PATHNAME}" "consumer_key"
  zgrep "${PATHNAME}" "consumer_secret"
  zgrep "${PATHNAME}" "credentials"
  zgrep "${PATHNAME}" "cypress_record_key"
  zgrep "${PATHNAME}" "database_password"
  zgrep "${PATHNAME}" "database_schema_test"
  zgrep "${PATHNAME}" "datadog_api_key"
  zgrep "${PATHNAME}" "datadog_app_key"
  zgrep "${PATHNAME}" "db_password"
  zgrep "${PATHNAME}" "db_server"
  zgrep "${PATHNAME}" "db_username"
  zgrep "${PATHNAME}" "dbpasswd"
  zgrep "${PATHNAME}" "dbpassword"
  zgrep "${PATHNAME}" "dbuser"
  zgrep "${PATHNAME}" "deploy_password"
  zgrep "${PATHNAME}" "digitalocean_ssh_key_body"
  zgrep "${PATHNAME}" "digitalocean_ssh_key_ids"
  zgrep "${PATHNAME}" "docker_hub_password"
  zgrep "${PATHNAME}" "docker_key"
  zgrep "${PATHNAME}" "docker_pass"
  zgrep "${PATHNAME}" "docker_passwd"
  zgrep "${PATHNAME}" "docker_password"
  zgrep "${PATHNAME}" "dockerhub_password"
  zgrep "${PATHNAME}" "dockerhubpassword"
  zgrep "${PATHNAME}" "dot-files"
  zgrep "${PATHNAME}" "dotfiles"
  zgrep "${PATHNAME}" "droplet_travis_password"
  zgrep "${PATHNAME}" "dynamoaccesskeyid"
  zgrep "${PATHNAME}" "elastica_host"
  zgrep "${PATHNAME}" "elastica_port"
  zgrep "${PATHNAME}" "elasticsearch_password"
  zgrep "${PATHNAME}" "encryption_key"
  zgrep "${PATHNAME}" "encryption_password"
  zgrep "${PATHNAME}" "env.heroku_api_key"
  zgrep "${PATHNAME}" "env.sonatype_password"
  zgrep "${PATHNAME}" "eureka.awssecretkey"

  echo && echo -e "\e[00;33m### WEB SERVERS ##########################################\e[00m"
  zfind "${PATHNAME}" "apache"
  zfind "${PATHNAME}" "lighttpd"
  zfind "${PATHNAME}" "alphapd"
  zfind "${PATHNAME}" "httpd"
  zfind "${PATHNAME}" "GoAhead"
  zfind "${PATHNAME}" "nginx"

  echo && echo -e "\e[00;33m### COMMAND INJECTION FUNCTIONS ##########################\e[00m"
  zgrep "${PATHNAME}" "system"
  zgrep "${PATHNAME}" "exec"
  zgrep "${PATHNAME}" "execl"
  zgrep "${PATHNAME}" "popen"
  zgrep "${PATHNAME}" "execve"
  zgrep "${PATHNAME}" "ShellExecute"
  zgrep "${PATHNAME}" "os.execute"
  zgrep "${PATHNAME}" "subprocess.call"
  zgrep "${PATHNAME}" "eval"
  zgrep "${PATHNAME}" "cmd"

  echo && echo -e "\e[00;33m### BANNED C FUNCTIONS ###################################\e[00m"
  zgrep "${PATHNAME}" "strcpy"
  zgrep "${PATHNAME}" "strcat"
  zgrep "${PATHNAME}" "strncat"
  zgrep "${PATHNAME}" "sprintf"
  zgrep "${PATHNAME}" "vsprintf"
  zgrep "${PATHNAME}" "strlen"
  zgrep "${PATHNAME}" "memcpy"
  zgrep "${PATHNAME}" "memcmp"
  zgrep "${PATHNAME}" "memset"
  zgrep "${PATHNAME}" "fopen"
  zgrep "${PATHNAME}" "gets"
  zgrep "${PATHNAME}" "getwd"
  zgrep "${PATHNAME}" "scanf"

  echo && echo -e "\e[00;33m### IMPORTANT BINARIES ###################################\e[00m"
  ${FINDBIN} ${PATHNAME} -name "ssh" 2>/dev/null | ${XARGSBIN} -r ${LSBIN} -al 2>/dev/null
  ${FINDBIN} ${PATHNAME} -name "sshd" 2>/dev/null | ${XARGSBIN} -r ${LSBIN} -al 2>/dev/null
  ${FINDBIN} ${PATHNAME} -name "scp" 2>/dev/null | ${XARGSBIN} -r ${LSBIN} -al 2>/dev/null
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
  zfind "${PATHNAME}" "*.asp"
  zfind "${PATHNAME}" "*.html"
  zfind "${PATHNAME}" "*.xml"
  zfind "${PATHNAME}" "*.php"
  zfind "${PATHNAME}" "*.js"
  zfind "${PATHNAME}" "*.json"
  zfind "${PATHNAME}" "*.cgi"

  echo && echo -e "\e[00;33m### IP ADDRESSES #########################################\e[00m"
  ${GREPBIN} -sRIEho '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' --exclude-dir='dev' ${PATHNAME} | ${SORTBIN} -u

  echo && echo -e "\e[00;33m### URLS #################################################\e[00m"
  ${GREPBIN} -sRIEoh '(http|https)://[^/"]+' --exclude-dir='dev' ${PATHNAME} | ${SORTBIN} -u
  ${GREPBIN} -sRIEoh '(ftp|ftps|tftp)://[^/"]+' --exclude-dir='dev' ${PATHNAME} | ${SORTBIN} -u

  echo && echo -e "\e[00;33m### EMAILS ###############################################\e[00m"
  ${GREPBIN} -sRIEoh '([[:alnum:]_.-]+@[[:alnum:]_.-]+?\.[[:alpha:].]{2,6})' "$@" --exclude-dir='dev' ${PATHNAME} | ${SORTBIN} -u

  echo && echo -e "\e[00;33m### SCAN COMPLETE ########################################\e[00m"
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
    exit 0
  elif [ "${COMMAND}" == "version" ]; then
    show_version
    echo
    show_license
    exit 0
  fi

  show_version
  show_this
}

main "$@"

