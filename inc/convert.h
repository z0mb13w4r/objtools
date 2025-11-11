#ifndef __CONVERT_H_
#define __CONVERT_H_

#include "options.h"

int dump_rolbrute8(const handle_t o, const unknown_t p, const size_t size, const int32_t step);
int dump_rolbrute16(const handle_t o, const unknown_t p, const size_t size, const int32_t step);
int dump_rorbrute8(const handle_t o, const unknown_t p, const size_t size, const int32_t step);
int dump_rorbrute16(const handle_t o, const unknown_t p, const size_t size, const int32_t step);

int dump_shlbrute8(const handle_t o, const unknown_t p, const size_t size, const int32_t step);
int dump_shlbrute16(const handle_t o, const unknown_t p, const size_t size, const int32_t step);
int dump_shrbrute8(const handle_t o, const unknown_t p, const size_t size, const int32_t step);
int dump_shrbrute16(const handle_t o, const unknown_t p, const size_t size, const int32_t step);

int dump_xorbrute8(const handle_t o, const unknown_t p, const size_t size, const int32_t step);
int dump_xorbrute16(const handle_t o, const unknown_t p, const size_t size, const int32_t step);

#endif

