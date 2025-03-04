#ifndef __OPCODE_H_
#define __OPCODE_H_

#include "buffer.h"

#define MODE_OPCODE                (MODE_PUT0('O') | MODE_PUT1('P') | MODE_PUT2('C'))

#define OPCODE_BFD                 (0)
#define OPCODE_SYMBOLS             (1)
#define OPCODE_SYMBOLS_DYNAMIC     (2)
#define OPCODE_MAXITEMS            (3)

typedef struct opcode_s {
  smode_t   mode;
  handle_t  data;
  unknown_t items[OPCODE_MAXITEMS];
} opcode_t, *popcode_t;

int isopcode(handle_t p);
int isobject(handle_t p);
int isarchive(handle_t p);
int iscoredump(handle_t p);

handle_t ocmalloc();
handle_t ocfree(handle_t p);

handle_t ocopen(const char* name);
int occlose(handle_t p);

unknown_t ocget(handle_t p, const imode_t mode);

void occonfig(const char* name, const char* target);

#endif

