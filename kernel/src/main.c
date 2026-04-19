#include <neo/kernel.h>

static volatile unsigned short *const vga = (unsigned short *)0xB8000;

static void vga_write(const char *s, unsigned char color, int row) {
    int col = 0;
    while (s[col] != '\0' && col < 80) {
        vga[row * 80 + col] = ((unsigned short)color << 8) | (unsigned char)s[col];
        col++;
    }
}

void neo_kernel_main(void) {
    vga_write("neoOS: kernel loaded", 0x0F, 0);
    vga_write("If you can read this, boot reached kernel entry.", 0x0A, 1);

    for (;;) {
        __asm__ volatile("hlt");
    }
}
