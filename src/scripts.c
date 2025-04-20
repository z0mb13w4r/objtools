#include "scripts.h"
#include "objutils.h"

static convert_t SCRIPTCOMMANDS[] = {
  {"add8",  ACT_ADD8},
  {"add16", ACT_ADD16},
  {"add32", ACT_ADD32},
  {"not8",  ACT_NOT8},
  {"not16", ACT_NOT16},
  {"not32", ACT_NOT32},
  {"rol8",  ACT_ROL8},
  {"rol16", ACT_ROL16},
  {"rol32", ACT_ROL32},
  {"ror8",  ACT_ROR8},
  {"ror16", ACT_ROR16},
  {"ror32", ACT_ROR32},
  {"rot5",  ACT_ROT5},
  {"rot13", ACT_ROT13},
  {"rot18", ACT_ROT18},
  {"shl8",  ACT_SHL8},
  {"shl16", ACT_SHL16},
  {"shl32", ACT_SHL32},
  {"shr8",  ACT_SHR8},
  {"shr16", ACT_SHR16},
  {"shr32", ACT_SHR32},
  {"sub8",  ACT_SUB8},
  {"sub16", ACT_SUB16},
  {"sub32", ACT_SUB32},
  {"xor8",  ACT_XOR8},
  {"xor16", ACT_XOR16},
  {"xor32", ACT_XOR32},
  {0, 0}
};

static int breakup_script(const pconvert_t p, const char *name, uint64_t *value) {
  MALLOCA(char, tmp, 1024);

  int x = -1;
  if (p && name && value) {
    strncpy(tmp, name, NELEMENTS(tmp));

    const char DELIMITS[] = "(),";
    char* tok = strtok(tmp, DELIMITS);
    if (tok) {
      for (pconvert_t pp = p; 0 != pp->text; ++pp) {
        if (0 == strcmp(pp->text, tok)) {
          x = pp->type;
          break;
        }
      }

      if (-1 != x) {
        tok = strtok(NULL, DELIMITS);
        if (tok) {
          *value = atovalue(tok);
        }
      }
    }
  }

  return x;
}

int insertscript(poptions_t o, const char *script) {
  MALLOCA(char, tmp, 1024);
  strncpy(tmp, script, NELEMENTS(tmp));

  const char DELIMIT = ';';

  char *p0 = tmp;
  char *p1 = strchr(p0, DELIMIT);
  if (p1) {
    while (p1) {
      *p1 = 0;
      uint64_t v = 0;
      const int x = breakup_script(SCRIPTCOMMANDS, p0, &v);
      if (-1 != x) {
        oinsertvalue(o, x, v);
      }

      p0 = p1 + 1;
      p1 = strchr(p0, DELIMIT);
    }

    return 0;
  }

  return -1;
}

