#!/usr/bin/env python3
import pexpect
import argparse

PROGRAM_NAME = 'enumerate-ng.py'
VERSION_VALUE = '0.0'

LICENSE_TEXT = '''COPYRIGHT
  MIT License

  Copyright (c) 2025 Kyle Wickens

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
  SOFTWARE.'''

BINBIGLIST='aria2c\|arp\|ash\|awk\|base64\|bash\|busybox\|cat\|chmod\|chown\|cp\|csh\|curl\|cut\|dash\|date\|dd\|diff\|dmsetup\|docker\|ed\|emacs\|env\|expand\|expect\|file\|find\|flock\|fmt\|fold\|ftp\|gawk\|gdb\|gimp\|git\|grep\|head\|ht\|iftop\|ionice\|ip$\|irb\|jjs\|jq\|jrunscript\|ksh\|ld.so\|ldconfig\|less\|logsave\|lua\|make\|man\|mawk\|more\|mv\|mysql\|nano\|nawk\|nc\|netcat\|nice\|nl\|nmap\|node\|od\|openssl\|perl\|pg\|php\|pic\|pico\|python\|readelf\|rlwrap\|rpm\|rpmquery\|rsync\|ruby\|run-parts\|rvim\|scp\|script\|sed\|setarch\|sftp\|sh\|shuf\|socat\|sort\|sqlite3\|ssh$\|start-stop-daemon\|stdbuf\|strace\|systemctl\|tail\|tar\|taskset\|tclsh\|tee\|telnet\|tftp\|time\|timeout\|ul\|unexpand\|uniq\|unshare\|vi\|vim\|watch\|wget\|wish\|xargs\|xxd\|zip\|zsh'


def mk(msg):
  print('\033[33m### ' + msg + ' ' + '#'*(52 - len(msg)) + '\033[00m')


def xx(cmd):
  t = 30
  for x in range(10):
    try:
      p = pexpect.spawn('bash', ['-c', cmd], timeout=t)
      return p.read()

    except pexpect.exceptions.TIMEOUT:
      t += 1000

  return None


def xy(c0, c1=None):
  r = xx(c0)
  if r is None:
    r = xx(c1)

  if r:
    return r.decode('utf-8')

  return None


def xz(args, color, msg, c0, c1=None):
  r = xy(c0, c1)

  if r:
    print(color + f'[-] ' + msg + f':\033[00m\n' + r)
    return r

  return None


def go(args, msg, c0, c1=None):
  if not args.norun:
    return xz(args, f'\033[31m', msg, c0, c1)

  return None


def gx(args, msg, c0, c1=None):
  if not args.norun:
    xz(args, f'\033[33m', msg, c0, c1)

  return None


def po(args, msg, c0, c1=None):
  if args.norun:
    xz(args, f'\033[31m', msg, c0, c1)

  return None


def px(args, msg, c0, c1=None):
  if args.norun:
    xz(args, f'\033[33m', msg, c0, c1)

  return None


def sys_info(args):
  if args.system:
    mk('SYSTEM')
    go(args,
       'Kernel information',
       'uname -a 2>/dev/null')
    go(args,
       'Kernel information (continued)',
       'cat /proc/version 2>/dev/null')
    go(args,
       'Specific release information',
       'cat /etc/*-release 2>/dev/null')
    go(args,
       'Hostname',
       'cat /etc/hostname 2>/dev/null')


def usr_info(args):
  if args.user:
    mk('USER/GROUP')
    go(args,
       'Current user/group info',
       'id 2>/dev/null')
    go(args,
       'Users that have previously logged onto the system',
       'lastlog 2>/dev/null | tail -n +2 | grep -v "Never" 2>/dev/null')
    go(args,
       'Who else is logged on',
       'w 2>/dev/null')
    go(args,
       'Group memberships',
       'for i in $(cut -d":" -f1 /etc/passwd 2>/dev/null); do id $i; done 2>/dev/null')
    go(args,
       'It looks like we have some admin users',
       'for i in $(cut -d":" -f1 /etc/passwd 2>/dev/null); do id $i | grep "(adm)"; done 2>/dev/null')
    gx(args,
       'It looks like we have password hashes in /etc/passwd!',
       'grep -v "^[^:]*:[x]" /etc/passwd 2>/dev/null')
    go(args,
       'Contents of /etc/passwd',
       'cat /etc/passwd 2>/dev/null')
    gx(args,
       'We can read the shadow file!',
       'cat /etc/shadow 2>/dev/null')
    gx(args,
       'We can read the master.passwd file!',
       'cat /etc/master.passwd 2>/dev/null')
    go(args,
       'Super user account(s)',
       "grep -v -E '^#' /etc/passwd | awk -F: '$3 == 0 {print $1}' 2>/dev/null")
    go(args,
       'Sudoers configuration (condensed)',
       'grep -v -e "^$" /etc/sudoers 2>/dev/null | grep -v "#" 2>/dev/null')

    gx(args,
       'We can sudo without supplying a password!',
       "echo '' | sudo -S -l -k 2>/dev/null")

    if args.password:
      gx(args,
         'We can sudo with supplying a password!',
         "echo '" + args.password + "' | sudo -S -l -k 2>/dev/null")

    gx(args,
       'Possible sudo pwnage!',
        "echo '' | sudo -S -l -k 2>/dev/null | xargs -n 1 2>/dev/null | sed 's/,*$//g' 2>/dev/null | grep -w '" + BINBIGLIST + "' 2>/dev/null")
    go(args,
       'Accounts that have recently used sudo',
       'find /home -name .sudo_as_admin_successful 2>/dev/null')
    go(args,
       "We can read root's home directory!",
       'ls -ahl /root/ 2>/dev/null')
    go(args,
       'Are permissions on /home directories lax',
       'ls -ahl /home/ 2>/dev/null')

    if args.more and args.username:
      go(args,
         'Files not owned by user but writable by group',
         'find / -writable ! -user ' + args.username + ' -type f ! -path "/proc/*" ! -path "/sys/*" -exec ls -al {} \; 2>/dev/null')
      go(args,
         'Files owned by our user',
         'find / -user ' + args.username + ' -type f ! -path "/proc/*" ! -path "/sys/*" -exec ls -al {} \; 2>/dev/null')

    if args.more:
      go(args,
         'Hidden files',
         'find / -name ".*" -type f ! -path "/proc/*" ! -path "/sys/*" -exec ls -al {} \; 2>/dev/null')
      go(args, 'World-readable files within /home', 'find /home/ -perm -4 -type f -exec ls -al {} \; 2>/dev/null')
      if args.username:
        go(args,
           'Home directory contents',
           'ls -ahl /home/' + args.username + '/')

      go(args,
         'SSH keys/host information found in the following locations',
         'find / \( -name "id_dsa*" -o -name "id_rsa*" -o -name "known_hosts" -o -name "authorized_hosts" -o -name "authorized_keys" \) -exec ls -la {} 2>/dev/null \;')

    go(args,
       'Root is allowed to login via SSH',
       'grep "PermitRootLogin " /etc/ssh/sshd_config 2>/dev/null | grep -v "#"')


def env_info(args):
  if args.env:
    mk('ENVIRONMENTAL')
    go(args,
       'Environment information', 'env 2>/dev/null | grep -v "LS_COLORS" 2>/dev/null')
    go(args,
       'SELinux seems to be present',
       'sestatus 2>/dev/null')
    go(args,
       'Path information',
       'echo $PATH 2>/dev/null')
    go(args,
       'Path information (writeable)',
       'ls -ld $(echo $PATH | tr ":" " ")')
    go(args,
       'Available shells',
       'cat /etc/shells 2>/dev/null')
    go(args,
       'Current umask value',
       'umask -S 2>/dev/null & umask 2>/dev/null')
    go(args,
       'umask value as specified in /etc/login.defs',
       'grep -i "^UMASK" /etc/login.defs 2>/dev/null')
    go(args,
       'Password and storage information',
       'grep "^PASS_MAX_DAYS\|^PASS_MIN_DAYS\|^PASS_WARN_AGE\|^ENCRYPT_METHOD" /etc/login.defs 2>/dev/null')


def job_info(args):
  if args.tasks:
    mk('JOBS/TASKS')
    go(args,
       'Cron jobs',
       'ls -la /etc/cron* 2>/dev/null')
    go(args,
       'World-writable cron jobs and file contents',
       'find /etc/cron* -perm -0002 -type f -exec ls -la {} \; -exec cat {} 2>/dev/null \;')
    go(args,
       'Crontab contents',
       'cat /etc/crontab 2>/dev/null')
    go(args,
       'Anything interesting in /var/spool/cron/crontabs',
       'ls -la /var/spool/cron/crontabs 2>/dev/null')
    go(args,
       'Anacron jobs and associated file permissions',
       'ls -la /etc/anacrontab 2>/dev/null; cat /etc/anacrontab 2>/dev/null')
    go(args,
       'When were jobs last executed (/var/spool/anacron contents)',
       'ls -la /var/spool/anacron 2>/dev/null')
    go(args,
       'Jobs held by all users',
        'cut -d ":" -f 1 /etc/passwd | xargs -n1 crontab -l -u 2>/dev/null')
    go(args,
       'Systemd timers',
       'systemctl list-timers --all 2>/dev/null')


def net_info(args):
  if args.network:
    mk('NETWORKING')
    go(args,
       'Network and IP info',
       '/sbin/ifconfig -a 2>/dev/null', '/sbin/ip a 2>/dev/null')
    go(args,
       'ARP history',
       'arp -a 2>/dev/null', 'ip n 2>/dev/null')
    go(args,
       'Nameserver(s)',
       'grep "nameserver" /etc/resolv.conf 2>/dev/null')
    go(args,
       'Nameserver(s)',
       'systemd-resolve --status 2>/dev/null')
    go(args,
       'Default route',
       'route 2>/dev/null | grep default',
       'ip r 2>/dev/null | grep default')
    go(args,
       'Listening TCP',
       'netstat -ntpl 2>/dev/null',
       'ss -t -l -n 2>/dev/null')
    go(args,
       'Listening UDP',
       'netstat -nupl 2>/dev/null',
       'ss -u -l -n 2>/dev/null')


def srv_info(args):
  if args.services:
    mk('SERVICES')
    go(args,
       'Running processes',
       'ps aux 2>/dev/null')
    go(args,
       'Process binaries and associated permissions (from above list)',
       "ps aux 2>/dev/null | awk '{print $11}' | xargs -r ls -la 2>/dev/null | awk '!x[$0]++' 2>/dev/null")
    go(args,
       'Contents of /etc/inetd.conf',
       'cat /etc/inetd.conf 2>/dev/null')
    go(args,
       'The related inetd binary permissions',
       "awk '{print $7}' /etc/inetd.conf 2>/dev/null | xargs -r ls -la 2>/dev/null")
    go(args,
       'Contents of /etc/xinetd.conf',
       'cat /etc/xinetd.conf 2>/dev/null')
    go(args,
       '/etc/xinetd.d is included in /etc/xinetd.conf - associated binary permissions are listed below',
       'grep "/etc/xinetd.d" /etc/xinetd.conf 2>/dev/null')
    go(args,
       'The related xinetd binary permissions',
       "awk '{print $7}' /etc/xinetd.conf 2>/dev/null | xargs -r ls -la 2>/dev/null")
    go(args,
       '/etc/init.d/ binary permissions',
       'ls -la /etc/init.d 2>/dev/null')
    gx(args,
       '/etc/init.d/ files not belonging to root',
       'find /etc/init.d/ \! -uid 0 -type f 2>/dev/null | xargs -r ls -la 2>/dev/null')
    go(args,
       '/etc/rc.d/init.d binary permissions',
       'ls -la /etc/rc.d/init.d 2>/dev/null')
    gx(args,
       '/etc/rc.d/init.d files not belonging to root',
       'find /etc/rc.d/init.d \! -uid 0 -type f 2>/dev/null | xargs -r ls -la 2>/dev/null')
    go(args,
       '/usr/local/etc/rc.d binary permissions',
       'ls -la /usr/local/etc/rc.d 2>/dev/null')
    gx(args,
       '/usr/local/etc/rc.d files not belonging to root',
       'find /usr/local/etc/rc.d \! -uid 0 -type f 2>/dev/null | xargs -r ls -la 2>/dev/null')
    go(args,
       '/etc/init/ config file permissions',
       'ls -la /etc/init/ 2>/dev/null')
    gx(args,
       '/etc/init/ config files not belonging to root',
       'find /etc/init \! -uid 0 -type f 2>/dev/null | xargs -r ls -la 2>/dev/null')
    go(args,
       '/lib/systemd/* config file permissions',
       'ls -lthR /lib/systemd/ 2>/dev/null')
    gx(args,
       '/lib/systemd/* config files not belonging to root',
       'find /lib/systemd/ \! -uid 0 -type f 2>/dev/null | xargs -r ls -la 2>/dev/null')


def sft_conf(args):
  if args.software:
    mk('SOFTWARE')
    go(args,
       'Sudo version',
       'sudo -V 2>/dev/null | grep "Sudo version" 2>/dev/null')
    go(args,
       'MYSQL version',
       'mysql --version 2>/dev/null')
    gx(args,
       'We can connect to the local MYSQL service with default root/root credentials!',
       'mysqladmin -uroot -proot version 2>/dev/null')
    gx(args,
       "We can connect to the local MYSQL service as 'root' and without a password!",
       'mysqladmin -uroot version 2>/dev/null')
    go(args,
       'Postgres version',
       'psql -V 2>/dev/null')
    gx(args,
       "We can connect to Postgres DB 'template0' as user 'postgres' with no password!",
       "psql -U postgres -w template0 -c 'select version()' 2>/dev/null | grep version")
    gx(args,
       "We can connect to Postgres DB 'template1' as user 'postgres' with no password!",
       "psql -U postgres -w template1 -c 'select version()' 2>/dev/null | grep version")
    gx(args,
       "We can connect to Postgres DB 'template0' as user 'psql' with no password!",
       "psql -U pgsql -w template0 -c 'select version()' 2>/dev/null | grep version")
    gx(args,
       "We can connect to Postgres DB 'template1' as user 'psql' with no password!",
       "psql -U pgsql -w template1 -c 'select version()' 2>/dev/null | grep version")
    go(args,
       'Apache version',
       'apache2 -v 2>/dev/null; httpd -v 2>/dev/null')
    go(args,
       'Apache user configuration',
       "grep -i 'user\|group' /etc/apache2/envvars 2>/dev/null | awk '{sub(/.*\export /,"")}1' 2>/dev/null")
    go(args,
       'Installed Apache modules',
       'apache2ctl -M 2>/dev/null; httpd -M 2>/dev/null')
    go(args,
       'htpasswd found - could contain passwords',
       'find / -name .htpasswd -print -exec cat {} \; 2>/dev/null')
    go(args,
       'www home dir contents',
       'ls -alhR /var/www/ 2>/dev/null; ls -alhR /srv/www/htdocs/ 2>/dev/null; ls -alhR /usr/local/www/apache2/data/ 2>/dev/null; ls -alhR /opt/lampp/htdocs/ 2>/dev/null')


def ask_info(args):
  if args.useful:
    mk('INTERESTING FILES')
    go(args,
       'Useful file locations',
       'which nc 2>/dev/null; which netcat 2>/dev/null; which wget 2>/dev/null; which nmap 2>/dev/null; which gcc 2>/dev/null; which curl 2>/dev/null')
    go(args,
       'Installed compilers',
       "dpkg --list 2>/dev/null | grep compiler | grep -v decompiler 2>/dev/null && yum list installed 'gcc*' 2>/dev/null | grep gcc 2>/dev/null")
    go(args,
       'Can we read/write sensitive files',
       'ls -la /etc/passwd 2>/dev/null; ls -la /etc/group 2>/dev/null; ls -la /etc/profile 2>/dev/null; ls -la /etc/shadow 2>/dev/null; ls -la /etc/master.passwd 2>/dev/null')

    suid = xy("find / -perm -4000 -type f 2>/dev/null | tr '\n' ' '")
    go(args, 'SUID files', 'find ' + suid + ' -perm -4000 -type f -exec ls -la {} 2>/dev/null \;')
    gx(args,
       'Possibly interesting SUID files',
       'find ' + suid + ' -perm -4000 -type f -exec ls -la {} \; 2>/dev/null | grep -w "' + BINBIGLIST + '" 2>/dev/null')
    gx(args, 'World-writable SUID files', 'find ' + suid + ' -perm -4002 -type f -exec ls -la {} 2>/dev/null \;')
    gx(args,
       'World-writable SUID files owned by root',
       'find ' + suid + ' -uid 0 -perm -4002 -type f -exec ls -la {} 2>/dev/null \;')

    sgid = xy("find / -perm -2000 -type f 2>/dev/null | tr '\n' ' '")
    go(args, 'SGID files', 'find ' + sgid + ' -perm -2000 -type f -exec ls -la {} 2>/dev/null \;')
    gx(args,
       'Possibly interesting SGID files',
       'find ' + sgid + ' -perm -2000 -type f  -exec ls -la {} \; 2>/dev/null | grep -w "' + BINBIGLIST + '" 2>/dev/null')
    gx(args,
       'World-writable SGID files',
       'find ' + sgid + ' -perm -2002 -type f -exec ls -la {} 2>/dev/null \;')
    gx(args,
       'World-writable SGID files owned by root',
       'find ' + sgid + ' -uid 0 -perm -2002 -type f -exec ls -la {} 2>/dev/null \;')
    caps = gx(args, 'Files with POSIX capabilities set', 'getcap -r / 2>/dev/null || /sbin/getcap -r / 2>/dev/null')

    gx(args, 'Users with specific POSIX capabilities', "grep -v '^#\|none\|^$' /etc/security/capability.conf 2>/dev/null")

    x0 = gx(args, 'Users with specific POSIX capabilities', "grep -v '^#\|none\|^$' /etc/security/capability.conf 2>/dev/null")
    if x0:
      x1 = gx(args, 'Capabilities associated with the current user', "echo -e '" + x0 + "' | grep '" + args.username + "' | awk '{print $1}' 2>/dev/null")
      if x1:
        x2 = gx(args,
          'Files with the same capabilities associated with the current user (You may want to try abusing those capabilties)',
          'echo -e "' + x1 + '" | while read -r cap ; do echo -e "' + caps + '" | grep "$cap" ; done 2>/dev/null')
        if x2:
          x3 = gx(args,
                  'Permissions of files with the same capabilities associated with the current user',
                  'echo -e "' + x2 + '" | awk "{print $1}" | while read -r f; do ls -la $f ;done 2>/dev/null')
          if x3:
            gx(args,
               'User/Group writable files with the same capabilities associated with the current user',
               'echo -e "' + x2 + '" | awk "{print $1}" | while read -r f; do find $f -writable -exec ls -la {} + ;done 2>/dev/null')

    gx(args, 'Private SSH keys found!', 'grep -rl "PRIVATE KEY-----" /home 2>/dev/null')
    gx(args, 'AWS secret keys found!', 'grep -rli "aws_secret_access_key" /home 2>/dev/null')
    gx(args, 'Git credentials saved on the machine!', 'find / -name ".git-credentials" 2>/dev/null')
    go(args, 'World-writable files (excluding /proc and /sys)', 'find / ! -path "*/proc/*" ! -path "/sys/*" -perm -2 -type f -exec ls -la {} 2>/dev/null \;')
    go(args, 'Plan file permissions and contents', 'find /home -iname *.plan -exec ls -la {} \; -exec cat {} 2>/dev/null \;')
    go(args, 'Plan file permissions and contents', 'find /usr/home -iname *.plan -exec ls -la {} \; -exec cat {} 2>/dev/null \;')
    gx(args, 'rhost config file(s) and file contents', 'find /home -iname *.rhosts -exec ls -la {} 2>/dev/null \; -exec cat {} 2>/dev/null \;')
    gx(args, 'rhost config file(s) and file contents', 'find /usr/home -iname *.rhosts -exec ls -la {} 2>/dev/null \; -exec cat {} 2>/dev/null \;')
    gx(args, 'Hosts.equiv file and contents', 'find /etc -iname hosts.equiv -exec ls -la {} 2>/dev/null \; -exec cat {} 2>/dev/null \;')
    go(args, 'NFS config details', 'ls -la /etc/exports 2>/dev/null; cat /etc/exports 2>/dev/null')
    go(args, 'NFS displaying partitions and filesystems - you need to check if exotic filesystems', 'cat /etc/fstab 2>/dev/null')
    gx(args, 'Looks like there are credentials in /etc/fstab!', "grep username /etc/fstab 2>/dev/null")
    gx(args, '/etc/fstab contains a credentials file!', "grep cred /etc/fstab 2>/dev/null")
    #
    go(args, 'All *.conf files in /etc (recursive 1 level)', 'find /etc/ -maxdepth 1 -name *.conf -type f -exec ls -la {} \; 2>/dev/null')
    go(args, "Current user's history files", 'ls -la ~/.*_history 2>/dev/null')
    gx(args, "Root's history files are accessible!", 'ls -la /root/.*_history 2>/dev/null')
    go(args, 'Location and contents (if accessible) of .bash_history file(s)', 'find /home -name .bash_history -print -exec cat {} 2>/dev/null \;')
    go(args, 'Location and Permissions (if accessible) of .bak file(s)', 'find / -name *.bak -type f 2</dev/null')
    go(args, 'Any interesting mail in /var/mail', 'ls -la /var/mail 2>/dev/null')
    gx(args, 'We can read /var/mail/root!', 'head /var/mail/root 2>/dev/null')


if __name__ == '__main__':
  p = argparse.ArgumentParser(prog='enumerate-ng', description='Used for enumeration and privilege escalation to harden devices or understanding how an adversary obtained persistence, privilege escalation, or lateral movement within a network.')
  p.add_argument('-u', '--username', help='username for checks.')
  p.add_argument('-p', '--password', help='user password for sudo checks (insecure).')
  p.add_argument('-d', '--path', default='', help='path to the firmware image.')
  p.add_argument('-m', '--more', action='store_true', help='more thorough tests.')
  p.add_argument('-v', '--version', action='store_true', help='verion number of ' + PROGRAM_NAME)
  p.add_argument('--system', action='store_true', help='system information.')
  p.add_argument('--user', action='store_true', help='user & group information.')
  p.add_argument('--env', action='store_true', help='environmental information.')
  p.add_argument('--tasks', action='store_true', help='jobs & tasks information.')
  p.add_argument('--network', action='store_true', help='networking information.')
  p.add_argument('--services', action='store_true', help='services information.')
  p.add_argument('--software', action='store_true', help='software information.')
  p.add_argument('--useful', action='store_true', help='interesting files information.')
  p.add_argument('--norun', action='store_true', help='development switch.')
  g = p.parse_args()

  if not(g.system or g.user or g.env or g.tasks or g.network or g.services or g.software or g.useful):
    g.system = g.user = g.env = g.tasks = g.network = g.services = g.software = g.useful = True

  if g.version:
    print(PROGRAM_NAME + ' v' + VERSION_VALUE + '\n')
    print(LICENSE_TEXT)

  else:
    if not g.username:
      g.username = xy("whoami 2>/dev/null | tr -d '\n'")

    sys_info(g)
    usr_info(g)
    env_info(g)
    job_info(g)
    net_info(g)
    srv_info(g)
    sft_conf(g)
    ask_info(g)

