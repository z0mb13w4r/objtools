#ifndef __OPCODE_ENGINE_H_
#define __OPCODE_ENGINE_H_

#include "opcode-examine.h"

#define OPENGINE_GROUP                 (1)
#define OPENGINE_EXAMINE               (2)

#define MODE_OCENGINE                  (MODE_PUT0('O') | MODE_PUT1('C') | MODE_PUT2('X'))

typedef struct ocgroups_s {
  uint64_t vaddr;

  pocexamine_t examine;
} ocgroups_t, *pocgroups_t;

typedef struct ocengine_s {
  smode_t    mode;
  size_t     cpos, spos, epos;
  size_t     size, sizemax;

  pocgroups_t groups;

} ocengine_t, *pocengine_t;

bool_t isocengine(handle_t p);

handle_t emalloc();
handle_t efree(handle_t p);

handle_t oecreate_engine(handle_t p);

handle_t oegetbyaddr(handle_t p, const uint64_t vaddr, const imode_t mode);
handle_t oeseebyaddr(handle_t p, const uint64_t vaddr, const imode_t mode);

#endif

