#ifndef __OCDWARF_MACROINFO_H_
#define __OCDWARF_MACROINFO_H_

#include "ocdwarf.h"

int ocdwarf_debug_macroinfo0a(handle_t p, handle_t s, handle_t d, const uint32_t offset1, const uint32_t offset2,
                              const uint16_t level, const uint16_t version, const uint16_t count, uint16_t size);
int ocdwarf_debug_macroinfo1a(handle_t p, handle_t s, handle_t d, const uint16_t idx, const uint8_t pick, const uint32_t offset,
                              const uint16_t level, const uint16_t version);
int ocdwarf_debug_macroinfo2a(handle_t p, handle_t s, handle_t d, const uint16_t idx, const uint8_t pick,
                              const uint16_t lineno, const uint16_t fileno, const char* name);
int ocdwarf_debug_macroinfo3a(handle_t p, handle_t s, handle_t d, const uint16_t idx, const uint8_t pick);
int ocdwarf_debug_macroinfo4a(handle_t p, handle_t s, handle_t d, const uint16_t idx, const uint8_t pick, const uint16_t lineno,
                              const uint32_t offset, const char* name);

#endif

