#!/usr/bin/env python3
import glob
import pexpect

def mk(msg):
  print('\033[33m### ' + msg + ' ' + '#'*(52 - len(msg)) + '\033[00m')


def go(msg,cmd):
  c = pexpect.spawn(cmd)
  print(f'\033[31m[-] ' + msg + f':\033[00m\n' + c.readline().decode('utf-8')) #.rstrip('\n'))


def ls(msg,cmd,params):
  print(f'\033[31m[-] ' + msg + ':\033[00m')
  for param in glob.glob(params):
    c = pexpect.spawn(cmd + ' ' + param)
    print(c.read().decode('utf-8'))


def system_info():
  mk('SYSTEM')
  go('Kernel information', 'uname -a')
  go('Kernel information (continued)', 'cat /proc/version')
  ls('Specific release information', 'cat', '/etc/*-release')
  go('Hostname', 'cat /etc/hostname')


if __name__ == '__main__':
  system_info()

