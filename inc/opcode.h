#ifndef __OPCODE_H_
#define __OPCODE_H_

#include <bfd.h>
#include <elf.h>
#include <dis-asm.h>
#include "capstone/capstone.h"

#include "buffer.h"

#define MODE_OPCODE                (MODE_PUT0('O') | MODE_PUT1('P') | MODE_PUT2('C'))
#define MODE_OPWRAP                (MODE_PUT0('W') | MODE_PUT1('R') | MODE_PUT2('P'))

#define MODE_OCSHDR                (MODE_PUT0('S') | MODE_PUT1('H') | MODE_PUT2('R'))
#define MODE_OCPHDR                (MODE_PUT0('P') | MODE_PUT1('H') | MODE_PUT2('R'))
#define MODE_OPCBFUNC              (MODE_PUT0('C') | MODE_PUT1('B') | MODE_PUT2('F'))

#define MODE_OCEHDR32              (MODE_PUT0('E') | MODE_PUT1('H') | MODE_PUT2(0x32))
#define MODE_OCEHDR64              (MODE_PUT0('E') | MODE_PUT1('H') | MODE_PUT2(0x64))
#define MODE_OCPHDR32              (MODE_PUT0('P') | MODE_PUT1('H') | MODE_PUT2(0x32))
#define MODE_OCPHDR64              (MODE_PUT0('P') | MODE_PUT1('H') | MODE_PUT2(0x64))
#define MODE_OCSHDR32              (MODE_PUT0('S') | MODE_PUT1('H') | MODE_PUT2(0x32))
#define MODE_OCSHDR64              (MODE_PUT0('S') | MODE_PUT1('H') | MODE_PUT2(0x64))

#define OPCODE_BFD                 (0)
#define OPCODE_SYMBOLS             (1)
#define OPCODE_SYMBOLS_DYNAMIC     (2)
#define OPCODE_DISASSEMBLER        (3)
#define OPCODE_MAXITEMS            (4)
#define OPCODE_RAWDATA             (OPCODE_MAXITEMS + 1)

#define ocgetbfd(x)                CAST(bfd*, ocget(x, OPCODE_BFD))
#define ocgetshdr(x)               CAST(asection*, ocget(x, MODE_OCSHDR))

#define ocgetehdr32(x)             CAST(Elf32_Ehdr*, ocget(x, MODE_OCEHDR32))
#define ocgetehdr64(x)             CAST(Elf64_Ehdr*, ocget(x, MODE_OCEHDR64))
#define ocgetphdr32(x)             CAST(Elf32_Phdr*, ocget(x, MODE_OCPHDR32))
#define ocgetphdr64(x)             CAST(Elf64_Phdr*, ocget(x, MODE_OCPHDR64))
#define ocgetshdr32(x)             CAST(Elf32_Shdr*, ocget(x, MODE_OCPSDR32))
#define ocgetshdr64(x)             CAST(Elf64_Shdr*, ocget(x, MODE_OCPSDR64))

#define MALLOCSWRAP(x,y,z,v)       MALLOCSMODE(x,y,z); (p##y)->item = v
#define MALLOCSCBFUNC(x,y,z,a,b,c) MALLOCSMODE(x,y,z); (p##y)->param = a; (p##y)->cbfunc = b; (p##y)->handle = c

typedef void (*opcbfunc_t)(handle_t p, handle_t item, unknown_t param);

typedef struct opcode_s {
  smode_t    mode;
  imode_t    action;
  handle_t   data;
  unknown_t  items[OPCODE_MAXITEMS];
  uint64_t   saddress; /* --start-address */
  uint64_t   eaddress; /* --stop-address */
  union {
    csh                cs;
    disassembler_ftype ocfunc;
  };
} opcode_t, *popcode_t;

typedef struct opwrap_s {
  smode_t    mode;
  union {
    unknown_t item;
    uint64_t  value;
  };
} opwrap_t, *popwrap_t;

typedef struct opfunc_s {
  smode_t    mode;
  handle_t   handle;
  unknown_t  param;
  opcbfunc_t cbfunc;
} opfunc_t, *popfunc_t;

bool_t isopcode(handle_t p);

bool_t isopshdr(handle_t p);
bool_t isopphdr(handle_t p);

bool_t isopehdrNN(handle_t p);
bool_t isopphdrNN(handle_t p);
bool_t isopshdrNN(handle_t p);

bool_t isobject(handle_t p);
bool_t isarchive(handle_t p);
bool_t iscoredump(handle_t p);
bool_t isattached(handle_t p);

handle_t ocmalloc();
handle_t ocfree(handle_t p);

handle_t ocopen(const char* name);
handle_t ocattach(handle_t p);
int occlose(handle_t p);

unknown_t ocget(handle_t p, const imode_t mode);

size_t ocget_maxsectionnamesize(handle_t p);

bool_t   ochas_phdr(handle_t p);
bool_t   ochas_shdr(handle_t p);

uint64_t ocget_type(handle_t p);
uint64_t ocget_flags(handle_t p);

uint64_t ocget_size(handle_t p);
uint64_t ocget_memsize(handle_t p);
uint64_t ocget_archsize(handle_t p);
uint64_t ocget_position(handle_t p);
uint64_t ocget_alignment(handle_t p);

uint64_t ocget_offset(handle_t p);
uint64_t ocget_paddress(handle_t p);
uint64_t ocget_saddress(handle_t p);
uint64_t ocget_lmaddress(handle_t p);
uint64_t ocget_vmaddress(handle_t p);

uint64_t ocget_opb(handle_t p, handle_t s);
uint64_t ocget_soffset(handle_t p, handle_t s);
uint64_t ocget_eoffset(handle_t p, handle_t s);

const char* ocget_name(handle_t p);
const char* ocget_fileformat(handle_t p);

void occonfig(const char* name, const char* target);

int ocdo_programs(handle_t p, opcbfunc_t cbfunc, unknown_t param);
int ocdo_sections(handle_t p, opcbfunc_t cbfunc, unknown_t param);

int ocdisassemble_open(handle_t p, handle_t o);
int ocdisassemble_close(handle_t p);

int ocdisassemble_run(handle_t p, handle_t s);
int ocdisassemble_raw(handle_t p, handle_t s, unknown_t data, const size_t size, const uint64_t vaddr);

#endif

