#ifndef __MEMLINK_H_
#define __MEMLINK_H_

#include "defines.h"

typedef struct node_s {
  modex_t  mode;
  handle_t item;

  struct node_s* next;
} node_t, *pnode_t;

int islink(handle_t p);
int islnext(handle_t p);

handle_t lget(handle_t p);
handle_t lgete(handle_t p);

handle_t lnext(handle_t p);

handle_t lattach(handle_t p, handle_t item);
handle_t lattache(handle_t p, handle_t item);

handle_t ldetach(handle_t p);
handle_t ldetache(handle_t p);

handle_t ldestroy(handle_t p);
handle_t ldestroye(handle_t p);

#endif

