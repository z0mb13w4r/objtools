#include "printf.h"
#include "externs.h"
#include "ocdwarf-aranges.h"

int ocdwarf_debug_aranges(handle_t p, handle_t s, handle_t d) {
  int n = 0;
  printf_text("Length", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
  printf_text("Version", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
  printf_text("Offset into .debug_info", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
  printf_text("Pointer Size", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
  printf_text("Segment Size", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
  printf_text("Address Length", USE_LT | USE_TAB | USE_EOL);

  return n;
}

