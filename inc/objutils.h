#ifndef __OBJUTILS_H_
#define __OBJUTILS_H_

typedef struct convert_s {
  const char*        text;
  const unsigned int type;
} convert_t, *pconvert_t;

char* strname(char* dst, const char* src);

const char* get_string(const pconvert_t p, const unsigned int type);
const char* get_stringnull(const pconvert_t p, const unsigned int type);

#endif

