#ifndef __OPCODE_ENGINE_H_
#define __OPCODE_ENGINE_H_

#include "opcode-examine.h"

#define OPENGINE_GROUP                 (1)
#define OPENGINE_DEBUG                 (2)
#define OPENGINE_SYMBOL                (3)
#define OPENGINE_EXAMINE               (4)

#define OPDEBUG_PE                     U64MASK(0)
#define OPDEBUG_EB                     U64MASK(1)
#define OPDEBUG_CC                     U64MASK(2)
#define OPDEBUG_ISA                    U64MASK(3)
#define OPDEBUG_FILENO                 U64MASK(4)
#define OPDEBUG_LINENO                 U64MASK(5)
#define OPDEBUG_COLUMN                 U64MASK(6)
#define OPDEBUG_DISCRIMINATOR          U64MASK(7)
#define OPDEBUG_LADDR                  U64MASK(8)
#define OPDEBUG_HADDR                  U64MASK(9)
#define OPDEBUG_NAME                   U64MASK(10)
#define OPDEBUG_SOURCE                 U64MASK(11)
#define OPDEBUG_SOURCECODE             U64MASK(12)

#define OPSYMBOL_LADDR                 U64MASK(0)
#define OPSYMBOL_HADDR                 U64MASK(1)
#define OPSYMBOL_NAME                  U64MASK(2)

#define MODE_OCDEBUG                   (MODE_PUT0('O') | MODE_PUT1('C') | MODE_PUT2('D'))
#define MODE_OCENGINE                  (MODE_PUT0('O') | MODE_PUT1('C') | MODE_PUT2('X'))
#define MODE_OCSYMBOL                  (MODE_PUT0('O') | MODE_PUT1('C') | MODE_PUT2('S'))

typedef struct ocdebug_s {
  smode_t   mode;
  imode_t   role;

  bool_t    pe;
  bool_t    eb;

  int32_t   cc;
  int32_t   isa;
  int32_t   nfile;
  int32_t   nline;
  int32_t   ncolumn;
  int32_t   discriminator;

  uint64_t  laddr;
  uint64_t  haddr;

  char     *name;
  char     *source;
  char     *sourcecode;
} ocdebug_t, *pocdebug_t;

typedef struct ocsymbol_s {
  smode_t   mode;
  imode_t   role;

  uint64_t  laddr;
  uint64_t  haddr;

  char     *name;
} ocsymbol_t, *pocsymbol_t;

typedef struct ocgroups_s {
  uint64_t     vaddr;

  pocdebug_t   debug;
  pocsymbol_t  symbol;
  pocexamine_t examine;
} ocgroups_t, *pocgroups_t;

typedef struct ocengine_s {
  smode_t    mode;
  size_t     cpos, spos, epos;
  size_t     size, sizemax;

  pocgroups_t groups;
} ocengine_t, *pocengine_t;

bool_t isodebug(handle_t p);
bool_t isoengine(handle_t p);
bool_t isosymbol(handle_t p);

handle_t emalloc();
handle_t efree(handle_t p);
handle_t eresize(handle_t p, const size_t sizemax);

handle_t odmalloc(const uint64_t vaddr);
handle_t odfree(handle_t p);

handle_t osmalloc(const uint64_t vaddr);
handle_t osfree(handle_t p);

handle_t oecreate_engine(handle_t p);

handle_t oeaskbyaddr(handle_t p, const uint64_t vaddr, const imode_t mode);
handle_t oegetbyaddr(handle_t p, const uint64_t vaddr, const imode_t mode);
handle_t oeseebyaddr(handle_t p, const uint64_t vaddr, const imode_t mode);

#endif

