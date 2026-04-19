#include <neo/kernel.h>
#include <stdint.h>

__attribute__((used, section(".multiboot"), aligned(8)))
static const uint32_t multiboot2_header[] = {
    0xe85250d6, /* magic */
    0,          /* architecture i386 */
    24,         /* header length */
    (uint32_t)(0u - (0xe85250d6u + 0u + 24u)), /* checksum */
    0,          /* end tag (type+flags) */
    8           /* end tag size */
};

static volatile unsigned short *const vga = (unsigned short *)0xB8000;

static void vga_write(const char *s, unsigned char color, int row) {
    int col = 0;
    while (s[col] != '\0' && col < 80) {
        vga[row * 80 + col] = ((unsigned short)color << 8) | (unsigned char)s[col];
        col++;
    }
}

void neo_kernel_main(void) {
    vga_write("neoOS: multiboot2 kernel loaded", 0x0F, 0);
    vga_write("Display path: VGA text mode", 0x0A, 1);

    for (;;) {
        __asm__ volatile("hlt");
    }
}
