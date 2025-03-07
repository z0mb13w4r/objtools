#ifndef __OPCODE_H_
#define __OPCODE_H_

#include <bfd.h>
#include <dis-asm.h>
#include "capstone/capstone.h"

#include "buffer.h"

#define MODE_OPCODE                (MODE_PUT0('O') | MODE_PUT1('P') | MODE_PUT2('C'))
#define MODE_OPWRAP                (MODE_PUT0('W') | MODE_PUT1('R') | MODE_PUT2('P'))
#define MODE_OPCBFUNC              (MODE_PUT0('C') | MODE_PUT1('B') | MODE_PUT2('F'))
#define MODE_OPSECTION             (MODE_PUT0('S') | MODE_PUT1('E') | MODE_PUT2('C'))

#define OPCODE_BFD                 (0)
#define OPCODE_SYMBOLS             (1)
#define OPCODE_SYMBOLS_DYNAMIC     (2)
#define OPCODE_DISASSEMBLER        (3)
#define OPCODE_MAXITEMS            (4)

#define ocgetbfd(x)                CAST(bfd*, ocget(x, OPCODE_BFD))
#define ocgetsec(x)                CAST(asection*, ocget(x, MODE_OPSECTION))

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

int isopcode(handle_t p);
int isopsection(handle_t p);

int isobject(handle_t p);
int isarchive(handle_t p);
int iscoredump(handle_t p);

handle_t ocmalloc();
handle_t ocfree(handle_t p);

handle_t ocopen(const char* name);
int occlose(handle_t p);

unknown_t ocget(handle_t p, const imode_t mode);

size_t ocget_maxsectionnamesize(handle_t p);

uint64_t ocget_flags(handle_t p);

uint64_t ocget_size(handle_t p);
uint64_t ocget_position(handle_t p);
uint64_t ocget_alignment(handle_t p);
uint64_t ocget_archsize(handle_t p);

uint64_t ocget_saddress(handle_t p);
uint64_t ocget_lmaddress(handle_t p);
uint64_t ocget_vmaddress(handle_t p);

uint64_t ocget_opb(handle_t p, handle_t s);
uint64_t ocget_soffset(handle_t p, handle_t s);
uint64_t ocget_eoffset(handle_t p, handle_t s);

const char* ocget_name(handle_t p);
const char* ocget_fileformat(handle_t p);

void occonfig(const char* name, const char* target);

int ocdo_sections(handle_t p, opcbfunc_t cbfunc, unknown_t param);

int ocdisassemble_open(handle_t p, handle_t o);
int ocdisassemble_close(handle_t p);

int ocdisassemble_run(handle_t p, handle_t s);

#endif

