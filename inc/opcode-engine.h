#ifndef __OPCODE_ENGINE_H_
#define __OPCODE_ENGINE_H_

#include "opcode-examine.h"

#define OPENGINE_GROUP                 (1)
#define OPENGINE_DEBUG                 (2)
#define OPENGINE_EXAMINE               (3)

#define MODE_OCDEBUG                   (MODE_PUT0('O') | MODE_PUT1('C') | MODE_PUT2('D'))
#define MODE_OCENGINE                  (MODE_PUT0('O') | MODE_PUT1('C') | MODE_PUT2('X'))

typedef struct ocdebug_s {
  smode_t    mode;

  int32_t    nfile;
  int32_t    nline;
  int32_t    ncolumn;
  int32_t    discriminator;

  uint64_t   laddr;
  uint64_t   haddr;

  char      *name;
  char      *source;
} ocdebug_t, *pocdebug_t;

typedef struct ocgroups_s {
  uint64_t     vaddr;

  pocdebug_t   debug;
  pocexamine_t examine;
} ocgroups_t, *pocgroups_t;

typedef struct ocengine_s {
  smode_t    mode;
  size_t     cpos, spos, epos;
  size_t     size, sizemax;

  pocgroups_t groups;
} ocengine_t, *pocengine_t;

bool_t isocdebug(handle_t p);
bool_t isocengine(handle_t p);

handle_t emalloc();
handle_t efree(handle_t p);
handle_t eresize(handle_t p, const size_t sizemax);

handle_t odmalloc();
handle_t odfree(handle_t p);

handle_t oecreate_engine(handle_t p);

handle_t oeaskbyaddr(handle_t p, const uint64_t vaddr, const imode_t mode);
handle_t oegetbyaddr(handle_t p, const uint64_t vaddr, const imode_t mode);
handle_t oeseebyaddr(handle_t p, const uint64_t vaddr, const imode_t mode);

#endif

