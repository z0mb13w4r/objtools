#!/usr/bin/env python3
import pexpect
import argparse

PROGRAM_NAME = 'strings-ng.py'
VERSION_VALUE = '1.0'

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


BINBIGLIST=r'aria2c\|arp\|ash\|awk\|base64\|bash\|busybox\|cat\|chmod\|chown\|cp\|csh\|curl\|cut\|dash\|date\|dd\|diff\|dmsetup\|docker\|ed\|emacs\|env\|expand\|expect\|find\|flock\|fmt\|fold\|ftp\|gawk\|gdb\|gimp\|git\|grep\|head\|ht\|iftop\|ionice\|ip$\|irb\|jjs\|jq\|jrunscript\|ksh\|ld.so\|ldconfig\|less\|logsave\|lua\|make\|man\|mawk\|more\|mv\|mysql\|nano\|nawk\|nc\|netcat\|nice\|nl\|nmap\|node\|od\|openssl\|perl\|pg\|php\|pic\|pico\|python\|readelf\|rlwrap\|rpm\|rpmquery\|rsync\|ruby\|run-parts\|rvim\|scp\|script\|sed\|setarch\|sftp\|sh\|shuf\|socat\|sort\|sqlite3\|ssh$\|start-stop-daemon\|stdbuf\|strace\|systemctl\|tail\|tar\|taskset\|tclsh\|tee\|telnet\|tftp\|time\|timeout\|ul\|unexpand\|uniq\|unshare\|vi\|vim\|watch\|wget\|wish\|xargs\|xxd\|zip\|zsh'
## file\|
EXEBIGLIST=r'reboot\|poweroff\|halt\|netstat\|systemd\|ftpget\|ftpput\|watchdog\|adb\|adbd\|docker\|dockerd'
DIRBIGLIST=r'/sys\|kernel\|/dev\|/var\|/tmp\|/usr\|/etc\|/bin\|/sbin\|/mnt\|/root\|/boot\|/home\|/media\|/opt\|/proc\|/lib\|/pts\|/data\|/local\|Program Files\|System32\|Users'
INTBIGLIST=r'USER-AGENT\|HOST\|Cookie\|POST\|GET\|url\|http\|https\|udp\|dns\|google'
TXTBIGLIST=r'ptm\|tty\|group\|passwd\|shells\|xterm'
EXTBIGLIST=r'\.gz$\|\.zip$\|\.7z$\|\.pub$\|\.pdf$\|\.doc$\|\.docx$\|\.png$\|\,jpg$\|\.jpeg$\|\.go$\|\.exe$\|\.ps1$\|\.sh$\|\.py$'
SECBIGLIST=r'\.shstrtab\|\.init\|\.text\|\.fini\|.rodata\|\.ctors\|\.dtors\|\.data\|\.bss\|\.noptrdata\|\.elfdata\|\.typelink\|\.itablink\|\.gosymtab\|\.gopclntab'
MGCBINLIST=r'pid\|upx\|abcdefghijklmnopqrstuvw.*012345678\|\.x86\|\.x86_64\|\.arm\|\.arm5\|\.arm6\|\.arm7\|\.mips\|\.mipsel\|\.sh4\|\.ppc\|TeamSpeak\|build\|net\|os\|io\|syscall\|crypto\|unicode\|runtime'


def pk(args):
  if g.version:
    return 'strings --version | head -n 1 | cut -d ")" -f 2 | cut -d " " -f 2'

  elif args.data and args.offset:
    return 'strings --data --radix=x ' + args.name

  elif args.data:
    return 'strings --data ' + args.name

  elif args.offset:
    return 'strings --radix=x ' + args.name

  return 'strings ' + args.name


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
    if msg:
      print(color + f'[-] ' + msg + f':\033[00m')

    print(r, end='')
    return r

  return None


def gz(args, c0, c1=None):
  if not args.norun:
    xz(args, None, None, c0, c1)

  return None


def ver_info(args):
  print(PROGRAM_NAME + ' v' + VERSION_VALUE)
  print('strings  v' + xy(pk(g)))
  print(LICENSE_TEXT)


def all_info(args):
  gz(args, pk(args))


def dir_info(args):
  if args.directories:
    mk('DIRECTORIES')
    gz(args, pk(args) + ' | grep -w "' + DIRBIGLIST + '" | grep -v "' + BINBIGLIST + '" | grep -v "' + EXEBIGLIST + '"')
    print('')


def ext_info(args):
  if args.extensions:
    mk('POSSSIBLE EXTENSIONS')
    gz(args, pk(args) + ' | grep "' + EXTBIGLIST + '"')
    #gz(args, pk(args) + ' | grep -w "' + TXTBIGLIST + '"')
    #gz(args, pk(args) + ' | grep -E "[[:alnum:]]+"')
    print('')


def sys_info(args):
  if args.privilege:
    mk('POSSIBLE PRIVILEGE ESCALATION')
    gz(args, pk(args) + ' | grep -w "' + BINBIGLIST + '"')
    print('')


def sec_info(args):
  if args.sections:
    mk('POSSIBLE SECTIONS')
    gz(args, pk(args) + ' | grep -w "' + SECBIGLIST + '"')
    print('')


def mgc_info(args):
  if args.magic:
    mk('POSSIBLE MAGIC STRINGS')
    gz(args, pk(args) + ' | grep -wi "' + MGCBINLIST + '" | grep -v "' + DIRBIGLIST + '"')
    print('')


def bin_info(args):
  if args.binaries:
    mk('BINARIES')
    gz(args, pk(args) + ' | grep -w "' + EXEBIGLIST + '"')
    print('')


def int_info(args):
  if args.internet:
    mk('INTERNET')
    gz(args, pk(args) + ' | grep -w "' + INTBIGLIST + '"')
    gz(args, pk(args) + ' | egrep "[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}" | grep -v "' + INTBIGLIST + '"')
    print('')


if __name__ == '__main__':
  p = argparse.ArgumentParser(
    prog=PROGRAM_NAME,
    description="""Used to scan binaries for high value strings for binary and
                   malware analysis.""")
  p.add_argument('name', nargs='?')           # positional argument
  #p.add_argument('-k', '--keyword', help='keyword search.')
  p.add_argument('-V', '--verbose', action='store_true', help='more verbose checks.')
  p.add_argument('-v', '--version', action='store_true', help='verion number of ' + PROGRAM_NAME)
  p.add_argument('-a', '--all', action='store_true', help='display all the strings.')
  p.add_argument('-b', '--binaries', action='store_true', help='search for possible binaries.')
  p.add_argument('-p', '--privilege', action='store_true', help='search for possible privilege escalation information.')
  p.add_argument('-e', '--extensions', action='store_true', help='search for possible extensions.')
  p.add_argument('-d', '--directories', action='store_true', help='search for possible directories.')
  p.add_argument('-i', '--internet', action='store_true', help='search for possible internet triggers.')
  p.add_argument('-s', '--sections', action='store_true', help='search for possible sections.')
  p.add_argument('-m', '--magic', action='store_true', help='search for possible magic strings.')
  p.add_argument('-x', '--data', action='store_true', help='only print strings from initialized, loaded data sections in the file.')
  p.add_argument('-o', '--offset', action='store_true', help='print the offset within the file before each string.')
  p.add_argument('--norun', action='store_true', help=argparse.SUPPRESS)
  g = p.parse_args()

  if not(g.privilege or g.binaries or g.extensions or g.directories or g.internet or g.sections or g.magic):
    g.binaries = g.privilege = g.extensions = g.directories = g.internet = g.sections = g.magic = True

  if g.version:
    ver_info(g)

  elif not g.name:
    p.print_help()

  elif g.all:
    all_info(g)

  else:
    dir_info(g)
    bin_info(g)
    sys_info(g)
    sec_info(g)
    ext_info(g)
    int_info(g)
    mgc_info(g)

