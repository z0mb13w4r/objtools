#!/usr/bin/env python3
#import glob
import pexpect
#import subprocess

USERNAME=''

def mk(msg):
  print('\033[33m### ' + msg + ' ' + '#'*(52 - len(msg)) + '\033[00m')


def go(m,c0,c1=None):
  #p = pexpect.spawn(c)
  p = pexpect.spawn('bash', ['-c', c0])
  r = p.read()
  if r is None:
    p = pexpect.spawn('bash', ['-c', c1])
    r = p.read()

  if r:
    print(f'\033[31m[-] ' + m + f':\033[00m\n' + r.decode('utf-8'))

  #p = subprocess.Popen(['bash', '-c', '"' + c + '"'], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
  #print(f'\033[31m[-] ' + m + f':\033[00m\n' + p.stdout.read().decode('utf-8'))


def gx(m,c):
  p = pexpect.spawn('bash', ['-c', c])
  r = p.read()
  if r:
    print(f'\033[33m[-] ' + m + f':\033[00m\n' + r.decode('utf-8'))


def sys_info():
  mk('SYSTEM')
  go('Kernel information', 'uname -a 2>/dev/null')
  go('Kernel information (continued)', 'cat /proc/version 2>/dev/null')
  go('Specific release information', 'cat /etc/*-release 2>/dev/null')
  go('Hostname', 'cat /etc/hostname 2>/dev/null')


def usr_info():
  mk('USER/GROUP')
  go('Current user/group info', 'id 2>/dev/null')
  go('Users that have previously logged onto the system', 'lastlog 2>/dev/null | tail -n +2 | grep -v "Never" 2>/dev/null')
  go('Who else is logged on', 'w 2>/dev/null')
  go('Group memberships', 'for i in $(cut -d":" -f1 /etc/passwd 2>/dev/null); do id $i; done 2>/dev/null')
  go('It looks like we have some admin users', 'for i in $(cut -d":" -f1 /etc/passwd 2>/dev/null); do id $i | grep "(adm)"; done 2>/dev/null')
  gx('It looks like we have password hashes in /etc/passwd!', 'grep -v "^[^:]*:[x]" /etc/passwd 2>/dev/null')
  go('Contents of /etc/passwd', 'cat /etc/passwd 2>/dev/null')
  gx('We can read the shadow file!', 'cat /etc/shadow 2>/dev/null')
  gx('We can read the master.passwd file!', 'cat /etc/master.passwd 2>/dev/null')
  go('Super user account(s)', "grep -v -E '^#' /etc/passwd | awk -F: '$3 == 0 {print $1}' 2>/dev/null")
  go('Sudoers configuration (condensed)', 'grep -v -e "^$" /etc/sudoers 2>/dev/null | grep -v "#" 2>/dev/null')
  #gx('We can sudo without supplying a password!', 'echo '' | sudo -S -l -k 2>/dev/null')

  #
  go('Accounts that have recently used sudo', 'find /home -name .sudo_as_admin_successful 2>/dev/null')
  go("We can read root's home directory!", 'ls -ahl /root/ 2>/dev/null')
  go('Are permissions on /home directories lax', 'ls -ahl /home/ 2>/dev/null')
  #go('Files not owned by user but writable by group', 'find / -writable ! -user ' + USERNAME + ' -type f ! -path "/proc/*" ! -path "/sys/*" -exec ls -al {} \; 2>/dev/null')
  #go('Files owned by our user', 'find / -user ' + USERNAME + ' -type f ! -path "/proc/*" ! -path "/sys/*" -exec ls -al {} \; 2>/dev/null')
  #go('Hidden files', 'find / -name ".*" -type f ! -path "/proc/*" ! -path "/sys/*" -exec ls -al {} \; 2>/dev/null')
  #go('World-readable files within /home', 'find /home/ -perm -4 -type f -exec ls -al {} \; 2>/dev/null')
  #go('Home directory contents', 'ls -ahl ~')
  #go('SSH keys/host information found in the following locations', 'find / \( -name "id_dsa*" -o -name "id_rsa*" -o -name "known_hosts" -o -name "authorized_hosts" -o -name "authorized_keys" \) -exec ls -al {}')
  go('Root is allowed to login via SSH', 'grep "PermitRootLogin " /etc/ssh/sshd_config 2>/dev/null | grep -v "#"')


def env_info():
  mk('ENVIRONMENTAL')
  go('Environment information', 'env 2>/dev/null | grep -v "LS_COLORS" 2>/dev/null')
  go('SELinux seems to be present', 'sestatus 2>/dev/null')
  go('Path information', 'echo $PATH 2>/dev/null')
  go('Path information (writeable)', 'ls -ld $(echo $PATH | tr ":" " ")')
  go('Available shells', 'cat /etc/shells 2>/dev/null')
  go('Current umask value', 'umask -S 2>/dev/null & umask 2>/dev/null')
  go('umask value as specified in /etc/login.defs', 'grep -i "^UMASK" /etc/login.defs 2>/dev/null')
  go('Password and storage information', 'grep "^PASS_MAX_DAYS\|^PASS_MIN_DAYS\|^PASS_WARN_AGE\|^ENCRYPT_METHOD" /etc/login.defs 2>/dev/null')


def job_info():
  mk('JOBS/TASKS')
  go('Cron jobs', 'ls -la /etc/cron* 2>/dev/null')
  go('World-writable cron jobs and file contents', 'find /etc/cron* -perm -0002 -type f -exec ls -la {} \; -exec cat {} 2>/dev/null \;')
  go('Crontab contents', 'cat /etc/crontab 2>/dev/null')
  go('Anything interesting in /var/spool/cron/crontabs', 'ls -la /var/spool/cron/crontabs 2>/dev/null')
  go('Anacron jobs and associated file permissions', 'ls -la /etc/anacrontab 2>/dev/null; cat /etc/anacrontab 2>/dev/null')
  go('When were jobs last executed (/var/spool/anacron contents)', 'ls -la /var/spool/anacron 2>/dev/null')
  go('Jobs held by all users', 'cut -d ":" -f 1 /etc/passwd | xargs -n1 crontab -l -u 2>/dev/null')
  go('Systemd timers', 'systemctl list-timers --all 2>/dev/null')


def net_info():
  mk('NETWORKING')
  go('Network and IP info', '/sbin/ifconfig -a 2>/dev/null', '/sbin/ip a 2>/dev/null')
  go('ARP history', 'arp -a 2>/dev/null', 'ip n 2>/dev/null')
  go('Nameserver(s)', 'grep "nameserver" /etc/resolv.conf 2>/dev/null')
  go('Nameserver(s)', 'systemd-resolve --status 2>/dev/null')
  go('Default route', 'route 2>/dev/null | grep default', 'ip r 2>/dev/null | grep default')
  go('Listening TCP', 'netstat -ntpl 2>/dev/null', 'ss -t -l -n 2>/dev/null')
  go('Listening UDP', 'netstat -nupl 2>/dev/null', 'ss -u -l -n 2>/dev/null')


def srv_info():
  mk('SERVICES')
  go('Running processes', 'ps aux 2>/dev/null')
  go('Process binaries and associated permissions (from above list)', "ps aux 2>/dev/null | awk '{print $11}' | xargs -r ls -la 2>/dev/null | awk '!x[$0]++' 2>/dev/null")
  go('Contents of /etc/inetd.conf', 'cat /etc/inetd.conf 2>/dev/null')
  go('The related inetd binary permissions', "awk '{print $7}' /etc/inetd.conf 2>/dev/null | xargs -r ls -la 2>/dev/null")
  go('Contents of /etc/xinetd.conf', 'cat /etc/xinetd.conf 2>/dev/null')
  go('/etc/xinetd.d is included in /etc/xinetd.conf - associated binary permissions are listed below', 'grep "/etc/xinetd.d" /etc/xinetd.conf 2>/dev/null')
  go('The related xinetd binary permissions', "awk '{print $7}' /etc/xinetd.conf 2>/dev/null | xargs -r ls -la 2>/dev/null")


if __name__ == '__main__':
  sys_info()
  usr_info()
  env_info()
  job_info()
  net_info()
  srv_info()

