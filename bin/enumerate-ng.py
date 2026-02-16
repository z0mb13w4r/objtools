#!/usr/bin/env python3
#import glob
import pexpect
#import subprocess

USERNAME=''

def mk(msg):
  print('\033[33m### ' + msg + ' ' + '#'*(52 - len(msg)) + '\033[00m')


def go(m,c):
  #p = pexpect.spawn(c)
  p = pexpect.spawn('bash', ['-c', c])
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


def system_info():
  mk('SYSTEM')
  go('Kernel information', 'uname -a 2>/dev/null')
  go('Kernel information (continued)', 'cat /proc/version 2>/dev/null')
  go('Specific release information', 'cat /etc/*-release 2>/dev/null')
  go('Hostname', 'cat /etc/hostname 2>/dev/null')


def user_info():
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
  go('Hidden files', 'find / -name ".*" -type f ! -path "/proc/*" ! -path "/sys/*" -exec ls -al {} \; 2>/dev/null')
  #go('World-readable files within /home', 'find /home/ -perm -4 -type f -exec ls -al {} \; 2>/dev/null')
  #go('Home directory contents', 'ls -ahl ~')
  #go('SSH keys/host information found in the following locations', 'find / \( -name "id_dsa*" -o -name "id_rsa*" -o -name "known_hosts" -o -name "authorized_hosts" -o -name "authorized_keys" \) -exec ls -al {}')
  go('Root is allowed to login via SSH', 'grep "PermitRootLogin " /etc/ssh/sshd_config 2>/dev/null | grep -v "#"')


def environment_info():
  mk('ENVIRONMENTAL')
  go('Environment information', 'env 2>/dev/null | grep -v "LS_COLORS" 2>/dev/null')

if __name__ == '__main__':
  system_info()
  user_info()
  environment_info()

