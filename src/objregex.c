#include "memuse.h"
#include "objregex.h"

#define REGEXGROUP_MAXSIZE           (10)

bool_t isregex(handle_t p) {
  return ismode(p, MODE_REGEX);
}

handle_t rmalloc(const char *pattern) {
  pre_t p0 = xmalloc(sizeof(re_t), MODE_HEAP);
  if (p0) {
    p0->data = xmalloc(sizeof(regex_t), MODE_HEAP);
    p0->groups = xmalloc(REGEXGROUP_MAXSIZE * sizeof(regmatch_t), MODE_HEAP);

    if (0 == regcomp(p0->data, pattern, REG_EXTENDED)) {
      return setmode(p0, MODE_REGEX);
    }

    xfree(p0->groups);
    xfree(p0->data);
    xfree(p0);
  }

  return NULL;
}

handle_t rfree(handle_t p) {
  if (isregex(p)) {
    pre_t p0 = CAST(pre_t, p);
    if (p0->data) {
      regfree(p0->data);
    }

    xfree(p0->groups);
    xfree(p0->data);
    xfree(p0);
    return NULL;
  }

  return p;
}

bool_t regex_match(handle_t p, const char *match) {
  if (isregex(p)) {
    pre_t p0 = CAST(pre_t, p);
    if (p0->data && p0->groups) {
      return 0 == regexec(p0->data, match, REGEXGROUP_MAXSIZE, p0->groups, 0);
    }
  }

  return FALSE;
}

int regex_getso(handle_t p, const int index) {
  if (isregex(p)) {
    pre_t p0 = CAST(pre_t, p);
    if (p0->groups && 0 <= index && index <= REGEXGROUP_MAXSIZE) {
      return CAST(regmatch_t *, p0->groups)[index].rm_so;
    }
  }

  return -1;
}

int regex_geteo(handle_t p, const int index) {
  if (isregex(p)) {
    pre_t p0 = CAST(pre_t, p);
    if (p0->groups && 0 <= index && index <= REGEXGROUP_MAXSIZE) {
      return CAST(regmatch_t *, p0->groups)[index].rm_eo;
    }
  }

  return -1;
}

size_t regex_getsize(handle_t p, const int index) {
  if (isregex(p)) {
    pre_t p0 = CAST(pre_t, p);
    if (p0->groups && 0 <= index && index <= REGEXGROUP_MAXSIZE) {
      regmatch_t *p1 = CAST(regmatch_t *, p0->groups) + index;
      return p1->rm_eo - p1->rm_so;
    }
  }

  return 0;
}

