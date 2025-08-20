#ifndef __OPCODE_H_
#define __OPCODE_H_

#include <bfd.h>
#include <elf.h>
#include <dis-asm.h>
#include "capstone/capstone.h"

#include "buffer.h"

#define MODE_OPDHDR                    'D'
#define MODE_OPEHDR                    'E'
#define MODE_OPSHDR                    'S'
#define MODE_OPPHDR                    'P'
#define MODE_OPCBKF                    'C'

#define MODE_OPPE                      'p'
#define MODE_OPBFD                     'b'
#define MODE_OPELF                     'e'

#define MODE_BIT8                      '1'
#define MODE_BIT16                     '2'
#define MODE_BIT32                     '4'
#define MODE_BIT64                     '8'
#define MODE_BITNN                     '0'

#define MODE_OPWRAP                    'W'

#define MODE_OPCODE                    (MODE_PUT0('O') | MODE_PUT1('P') | MODE_PUT2('C'))

#define MODE_OPDHDRWRAP                (MODE_PUT0(MODE_OPDHDR) | MODE_PUT3(MODE_OPWRAP))
#define MODE_OPEHDRWRAP                (MODE_PUT0(MODE_OPEHDR) | MODE_PUT3(MODE_OPWRAP))
#define MODE_OCSHDRWRAP                (MODE_PUT0(MODE_OPSHDR) | MODE_PUT3(MODE_OPWRAP))
#define MODE_OPPHDRWRAP                (MODE_PUT0(MODE_OPPHDR) | MODE_PUT3(MODE_OPWRAP))

#define MODE_OCSHDRPE                  (MODE_OCSHDRWRAP | MODE_PUT1(MODE_OPPE) | MODE_PUT2(MODE_BITNN))
#define MODE_OCPHDRPE                  (MODE_OPPHDRWRAP | MODE_PUT1(MODE_OPPE) | MODE_PUT2(MODE_BITNN))

#define MODE_OCSHDR                    (MODE_OCSHDRWRAP | MODE_PUT1(MODE_OPBFD) | MODE_PUT2(MODE_BITNN))
#define MODE_OCPHDR                    (MODE_OPPHDRWRAP | MODE_PUT1(MODE_OPBFD) | MODE_PUT2(MODE_BITNN))

#define MODE_OPCBFUNC                  (MODE_PUT0(MODE_OPCBKF) | MODE_PUT1('B') | MODE_PUT2('F') | MODE_PUT3(MODE_OPWRAP))

#define MODE_OCDHDR32                  (MODE_OPDHDRWRAP | MODE_PUT1(MODE_OPELF) | MODE_PUT2(MODE_BIT32))
#define MODE_OCDHDR64                  (MODE_OPDHDRWRAP | MODE_PUT1(MODE_OPELF) | MODE_PUT2(MODE_BIT64))
#define MODE_OCEHDR32                  (MODE_OPEHDRWRAP | MODE_PUT1(MODE_OPELF) | MODE_PUT2(MODE_BIT32))
#define MODE_OCEHDR64                  (MODE_OPEHDRWRAP | MODE_PUT1(MODE_OPELF) | MODE_PUT2(MODE_BIT64))
#define MODE_OCPHDR32                  (MODE_OPPHDRWRAP | MODE_PUT1(MODE_OPELF) | MODE_PUT2(MODE_BIT32))
#define MODE_OCPHDR64                  (MODE_OPPHDRWRAP | MODE_PUT1(MODE_OPELF) | MODE_PUT2(MODE_BIT64))
#define MODE_OCSHDR32                  (MODE_OCSHDRWRAP | MODE_PUT1(MODE_OPELF) | MODE_PUT2(MODE_BIT32))
#define MODE_OCSHDR64                  (MODE_OCSHDRWRAP | MODE_PUT1(MODE_OPELF) | MODE_PUT2(MODE_BIT64))

#define OPCODE_BFD                     (0)
#define OPCODE_DWARF                   (1)
#define OPCODE_SYMBOLS                 (2)
#define OPCODE_SYMBOLS_DYNAMIC         (3)
#define OPCODE_SYMBOLS_DYNAMICRELOC    (4)
#define OPCODE_DISASSEMBLER            (5)
#define OPCODE_OUTDATA                 (6)
#define OPCODE_ENGINE                  (7)
#define OPCODE_MAXITEMS                (8)
#define OPCODE_THIS                    (OPCODE_MAXITEMS + 1)
#define OPCODE_PARAM1                  (OPCODE_MAXITEMS + 2)
#define OPCODE_PARAM2                  (OPCODE_MAXITEMS + 3)
#define OPCODE_RAWDATA                 (OPCODE_MAXITEMS + 4)
#define OPCODE_RAWSYMBOLS              (OPCODE_MAXITEMS + 5)
#define OPCODE_RAWSYMBOLS_DYNAMIC      (OPCODE_MAXITEMS + 6)
#define OPCODE_RAWSYMBOLS_DYNAMICRELOC (OPCODE_MAXITEMS + 7)
#define OPCODE_DWARF_DEBUG             (OPCODE_MAXITEMS + 8)
#define OPCODE_DWARF_ERROR             (OPCODE_MAXITEMS + 9)
#define OPCODE_DWARF_SRCFILES          (OPCODE_MAXITEMS + 10)
#define OPCODE_DWARF_STATISTICS        (OPCODE_MAXITEMS + 11)

#define OPCODE_NULLADDR                CAST(uint64_t, -1)

#define ocgetbfd(x)                    CAST(bfd*, ocget(x, OPCODE_BFD))
#define ocgetshdr(x)                   CAST(asection*, ocget(x, MODE_OCSHDR))

#define ocgetehdr32(x)                 CAST(Elf32_Ehdr*, ocget(x, MODE_OCEHDR32))
#define ocgetehdr64(x)                 CAST(Elf64_Ehdr*, ocget(x, MODE_OCEHDR64))
#define ocgetphdr32(x)                 CAST(Elf32_Phdr*, ocget(x, MODE_OCPHDR32))
#define ocgetphdr64(x)                 CAST(Elf64_Phdr*, ocget(x, MODE_OCPHDR64))
#define ocgetshdr32(x)                 CAST(Elf32_Shdr*, ocget(x, MODE_OCPSDR32))
#define ocgetshdr64(x)                 CAST(Elf64_Shdr*, ocget(x, MODE_OCPSDR64))

#define MALLOCSWRAP(x,y,z,v)           MALLOCSMODE(x,y,z); (p##y)->param1 = v
#define MALLOCSWRAPEX(x,y,z,v,w)       MALLOCSMODE(x,y,z); (p##y)->param1 = v; (p##y)->param2 = w

#define MALLOCSCBFUNC(x,y,z,a,b,c)     MALLOCSMODE(x,y,z); (p##y)->param = a; (p##y)->cbfunc = b; (p##y)->handle = c

typedef void (*opcbfunc_t)(handle_t p, handle_t item, unknown_t param);

typedef struct opcode_s {
  smode_t    mode;
  imode_t    action;
  imode_t    ocdump;
  handle_t   data;
  unknown_t  items[OPCODE_MAXITEMS];
  uint64_t   saddress; /* --start-address */
  uint64_t   eaddress; /* --stop-address */
  uint64_t   prev_nline;
  uint64_t   prev_discriminator;
  char       prev_name[160];

  union {
    csh                cs;
    disassembler_ftype ocfunc;
  };
  union {
    char inpname[PATH_MAX];
    char inpname0[PATH_MAX];
  };
  union {
    char outname[PATH_MAX];
    char inpname1[PATH_MAX];
  };
} opcode_t, *popcode_t;

typedef struct opwrap_s {
  smode_t    mode;
  union {
    unknown_t param1;
    uint64_t  value1;
  };
  union {
    unknown_t param2;
    uint64_t  value2;
  };
} opwrap_t, *popwrap_t;

typedef struct opfunc_s {
  smode_t    mode;
  handle_t   handle;
  unknown_t  param;
  opcbfunc_t cbfunc;
} opfunc_t, *popfunc_t;

bool_t isopcode(handle_t p);
bool_t isopwrap(handle_t p);

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
bool_t    ochas(handle_t p, const imode_t mode);

size_t ocget_maxsectionnamesize(handle_t p);

bool_t   ochas_phdr(handle_t p);
bool_t   ochas_shdr(handle_t p);
bool_t   ocuse_vaddr(handle_t p, const uint64_t vaddr);

bool_t   ocis32(handle_t p);
bool_t   ocis64(handle_t p);

uint64_t ocget_type(handle_t p);
uint64_t ocget_flags(handle_t p);
uint64_t ocget_value(handle_t p);

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

unknown_t ocget_data(handle_t p);

const char* ocget_symbol(handle_t p, uint64_t vaddr, char **name,
                     uint64_t *nline, uint64_t *ncolumn, uint64_t *discriminator, char **source,
                     uint64_t *laddr, uint64_t *haddr, uint64_t *offset);

const char* ocget_name(handle_t p);
const char* ocget_fileformat(handle_t p);

void occonfig(const char* name, const char* target);

int ocdo_dynamics(handle_t p, opcbfunc_t cbfunc, unknown_t param);
int ocdo_programs(handle_t p, opcbfunc_t cbfunc, unknown_t param);
int ocdo_sections(handle_t p, opcbfunc_t cbfunc, unknown_t param);

int ocdisassemble_open(handle_t p, handle_t o);
int ocdisassemble_close(handle_t p);

int ocdisassemble_run(handle_t p, handle_t s);
int ocdisassemble_raw(handle_t p, handle_t s, unknown_t data, const size_t size, const uint64_t vaddr);

#endif

