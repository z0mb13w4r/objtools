#ifndef __OPCODE_ENGINE_H_
#define __OPCODE_ENGINE_H_

#include "opcode-examine.h"

handle_t oecreate_engine(handle_t p);

handle_t oegetbyaddr(handle_t p, const uint64_t vaddr, const imode_t mode);
handle_t oeseebyaddr(handle_t p, const uint64_t vaddr, const imode_t mode);

#endif

