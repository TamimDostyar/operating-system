#include "../utils/vga.h"

void kmain(void) {
  vga_init();
  vga_write(" Mr. Tamim Dostyar!\n");
  for (;;)
    ;
}