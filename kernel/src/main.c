#include <neo/kernel.h>

static volatile unsigned short *const VGA_TEXT = (unsigned short *)0xB8000;

static void write_line(const char *msg, unsigned char color, int row) {
    int col = 0;
    while (msg[col] != '\0' && col < 80) {
        VGA_TEXT[row * 80 + col] = ((unsigned short)color << 8) | (unsigned char)msg[col];
        col++;
    }
}

void neo_kernel_main(void) {
    write_line("neoOS kernel bootstrap", 0x0F, 2);
    write_line("target: x86_64/aarch64 scaffold", 0x0A, 3);

    for (;;) {
        __asm__ volatile("hlt");
    }
}
