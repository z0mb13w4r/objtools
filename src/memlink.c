#include "memlink.h"

int islink(handle_t p) {
  return ismode(p, MODE_LINK);
}

int islnext(handle_t p) {
  if (islink(p)) {
    pnode_t pp = CAST(pnode_t, p);
    return pp->next && islink(pp->next);
  }

  return 0;
}

int ldo(handle_t p, lcallback_t callback, handle_t args) {
  int r = 0;
  while (islink(p)) {
    if (0 != (r = callback(p, args))) return r;
    p = lnext(p);
  }

  return 0;
}

handle_t lget(handle_t p) {
  if (islink(p)) {
    return CAST(pnode_t, p)->item;
  }

  return NULL;
}

handle_t lgete(handle_t p) {
  if (islink(p)) {
    while (islnext(p)) {
      p = lnext(p);
    }

    if (p) {
      return CAST(pnode_t, p)->item;
    }
  }

  return NULL;
}

handle_t lnext(handle_t p) {
  return islnext(p) ? CAST(pnode_t, p)->next : NULL;
}

handle_t lattach(handle_t p, handle_t item) {
  if (p && item) {
    if (islink(p)) {
      pnode_t p0 = CAST(pnode_t, p);
      pnode_t p1 = lmalloc();
      if (p1) {
        p1->item = item;
        p1->next = p0;
        return p1;
      }
    }
  } else if (item) {
    pnode_t pp = lmalloc();
    if (pp) {
      pp->item = item;
      return pp;
    }
  }

  return NULL;
}

handle_t lattache(handle_t p, handle_t item) {
  if (p && item) {
    if (islink(p)) {
      pnode_t p0 = CAST(pnode_t, p);
      while (islnext(p0)) {
        p0 = lnext(p0);
      }

      if (p0) {
        pnode_t p1 = lmalloc();
        if (p1) {
          p0->next = p1;
          p1->item = item;
          return p;
        }
      }
    }
  } else if (item) {
    pnode_t pp = lmalloc();
    if (pp) {
      pp->item = item;
      return pp;
    }
  }

  return NULL;
}

handle_t ldetach(handle_t p) {
  if (islink(p)) {
    pnode_t p1 = lnext(p);
    xdump(p);
    return p1;
  }

  return NULL;
}

handle_t ldestroy(handle_t p) {
  if (islink(p)) {
    pnode_t p1 = lnext(p);
    xfree(p);
    return p1;
  }

  return NULL;
}

handle_t ldetache(handle_t p) {
  if (islink(p)) {
    pnode_t p0 = CAST(pnode_t, p), p1 = NULL;
    while (islnext(p0)) {
      p1 = p0;
      p0 = lnext(p0);
    }

    if (islnext(p1)) {
      xdump(lnext(p1));
      p1->next = NULL;
    }
  }

  return p;
}

handle_t ldestroye(handle_t p) {
  if (islink(p)) {
    pnode_t p0 = CAST(pnode_t, p), p1 = NULL;
    while (islnext(p0)) {
      p1 = p0;
      p0 = lnext(p0);
    }

    if (islnext(p1)) {
      xfree(lnext(p1));
      p1->next = NULL;
    }
  }

  return p;
}

handle_t lmalloc() {
  pnode_t p = xmalloc(sizeof(node_t));
  return setmode(p, MODE_LINK);
}

handle_t lfree(handle_t p) {
  if (islink(p)) {
    free(p);
    return NULL;
  }

  return p;
}

