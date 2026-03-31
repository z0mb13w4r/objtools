#ifndef __OBJREGEX_H_
#define __OBJREGEX_H_

#include <regex.h>

#include "defines.h"

#define MODE_REGEX             (MODE_PUT0('R') | MODE_PUT1('E') | MODE_PUT2('G') | MODE_PUT3('X'))

typedef struct re_s {
  smode_t   mode;

  unknown_t data;
  unknown_t groups;

} re_t, *pre_t;


bool_t isregex(handle_t p);

handle_t rmalloc(const char *pattern);
handle_t rfree(handle_t p);

bool_t regex_match(handle_t p, const char *match);

#endif

