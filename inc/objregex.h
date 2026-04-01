#ifndef __OBJREGEX_H_
#define __OBJREGEX_H_

#include <regex.h>

#include "defines.h"

#define REGEXGROUP_MAXSIZE     (10)

#define MODE_REGEX             (MODE_PUT0('R') | MODE_PUT1('E') | MODE_PUT2('G') | MODE_PUT3('X'))

typedef struct re_s {
  smode_t   mode;

  unknown_t data;
  unknown_t match;
  unknown_t groups;
  unknown_t values[REGEXGROUP_MAXSIZE];

} re_t, *pre_t;


bool_t isregex(handle_t p);

handle_t rmalloc(const char *pattern);
handle_t rfree(handle_t p);

bool_t regex_match(handle_t p, const char *match);

int regex_getso(handle_t p, const int index);
int regex_geteo(handle_t p, const int index);
size_t regex_getsize(handle_t p, const int index);
bool_t regex_isvalue(handle_t p, const int index);
const char* regex_getvalue(handle_t p, const int index);

#endif

