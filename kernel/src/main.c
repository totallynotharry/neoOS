#include <neo/kernel.h>

void neo_kernel_main(void) {
    for (;;) {
        __asm__ volatile("hlt");
    }
}
