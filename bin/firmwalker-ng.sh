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
  ${GREPBIN} -sirnow ${PATHNAME} -e "cmd=" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "exec=" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "command=" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "execute=" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "AccessKey" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "AccessKeySecret" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "access_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "access_token" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "admin_pass" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "admin_user" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "algolia_admin_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "algolia_api_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "alias_pass" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "alicloud_access_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "amazon_secret_access_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "amazonaws" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "ansible_vault_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "aos_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "api_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "api_key_secret" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "api_key_sid" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "api_secret" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "api.googlemaps" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "apikey" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "apiSecret" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "app_debug" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "app_id" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "app_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "app_log_level" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "app_secret" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "appkey" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "appkeysecret" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "application_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "appsecret" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "auth_token" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "authorizationToken" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "authsecret" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "aws_access" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "aws_access_key_id" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "aws_bucket" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "aws_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "aws_secret" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "aws_secret_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "aws_token" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "AWSSecretKey" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "b2_app_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "bashrc password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "bintray_apikey" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "bintray_gpg_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "bintray_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "bintraykey" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "bluemix_api_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "bluemix_pass" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "browserstack_access_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "bucket_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "bucketeer_aws_access_key_id" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "bucketeer_aws_secret_access_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "built_branch_deploy_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "bx_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "cache_driver" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "cache_s3_secret_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "cattle_access_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "cattle_secret_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "certificate_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "ci_deploy_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "client_secret" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "client_zpk_secret_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "clojars_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "cloud_api_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "cloud_watch_aws_access_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "cloudant_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "cloudflare_api_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "cloudflare_auth_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "cloudinary_api_secret" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "cloudinary_name" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "codecov_token" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "config" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "conn.login" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "connectionstring" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "consumer_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "consumer_secret" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "credentials" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "cypress_record_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "database_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "database_schema_test" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "datadog_api_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "datadog_app_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "db_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "db_server" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "db_username" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "dbpasswd" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "dbpassword" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "dbuser" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "deploy_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "digitalocean_ssh_key_body" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "digitalocean_ssh_key_ids" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "docker_hub_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "docker_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "docker_pass" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "docker_passwd" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "docker_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "dockerhub_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "dockerhubpassword" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "dot-files" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "dotfiles" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "droplet_travis_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "dynamoaccesskeyid" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "dynamosecretaccesskey" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "elastica_host" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "elastica_port" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "elasticsearch_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "encryption_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "encryption_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "env.heroku_api_key" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "env.sonatype_password" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u
  ${GREPBIN} -sirnow ${PATHNAME} -e "eureka.awssecretkey" 2>/dev/null | ${CUTBIN} -d ':' -f 1,3 | ${SORTBIN} -u

  echo && echo -e "\e[00;33m### WEB SERVERS ##########################################\e[00m"
  zfind "${PATHNAME}" "apache"
  zfind "${PATHNAME}" "lighttpd"
  zfind "${PATHNAME}" "alphapd"
  zfind "${PATHNAME}" "httpd"
  zfind "${PATHNAME}" "GoAhead"
  zfind "${PATHNAME}" "nginx"

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

  show_this
}

main "$@"

