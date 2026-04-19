#include <neo/kernel.h>
#include <stdint.h>

#define MULTIBOOT2_HEADER_MAGIC 0xe85250d6u
#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36d76289u
#define MULTIBOOT2_TAG_TYPE_END 0u
#define MULTIBOOT2_TAG_TYPE_FRAMEBUFFER 8u

__attribute__((used, section(".multiboot"), aligned(8)))
static const uint32_t multiboot2_header[] = {
    MULTIBOOT2_HEADER_MAGIC,
    0,
    24,
    (uint32_t)(0u - (MULTIBOOT2_HEADER_MAGIC + 0u + 24u)),
    0,
    8
};

struct mb2_tag {
    uint32_t type;
    uint32_t size;
};

struct mb2_framebuffer_tag {
    uint32_t type;
    uint32_t size;
    uint64_t addr;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
    uint8_t fb_type;
    uint16_t reserved;
};

static void fill_framebuffer(struct mb2_framebuffer_tag *fb, uint32_t color) {
    if (fb == 0 || fb->bpp != 32) {
        return;
    }

    uint8_t *base = (uint8_t *)(uintptr_t)fb->addr;
    for (uint32_t y = 0; y < fb->height; y++) {
        uint32_t *row = (uint32_t *)(base + (uint64_t)y * fb->pitch);
        for (uint32_t x = 0; x < fb->width; x++) {
            row[x] = color;
        }
    }
}

static void draw_rect(struct mb2_framebuffer_tag *fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    if (fb == 0 || fb->bpp != 32) {
        return;
    }

    if (x >= fb->width || y >= fb->height) {
        return;
    }

    uint32_t x2 = (x + w > fb->width) ? fb->width : x + w;
    uint32_t y2 = (y + h > fb->height) ? fb->height : y + h;

    uint8_t *base = (uint8_t *)(uintptr_t)fb->addr;
    for (uint32_t py = y; py < y2; py++) {
        uint32_t *row = (uint32_t *)(base + (uint64_t)py * fb->pitch);
        for (uint32_t px = x; px < x2; px++) {
            row[px] = color;
        }
    }
}

static struct mb2_framebuffer_tag *find_framebuffer_tag(uintptr_t mb_info_addr) {
    uint8_t *mb = (uint8_t *)mb_info_addr;
    uint32_t total_size = *(uint32_t *)mb;
    (void)total_size;

    struct mb2_tag *tag = (struct mb2_tag *)(mb + 8);
    while (tag->type != MULTIBOOT2_TAG_TYPE_END) {
        if (tag->type == MULTIBOOT2_TAG_TYPE_FRAMEBUFFER) {
            return (struct mb2_framebuffer_tag *)tag;
        }
        uint32_t next = (tag->size + 7u) & ~7u;
        tag = (struct mb2_tag *)((uint8_t *)tag + next);
    }

    return 0;
}

static volatile uint16_t *const vga = (uint16_t *)0xB8000;

static void vga_write(const char *s, uint8_t color, int row) {
    int col = 0;
    while (s[col] != '\0' && col < 80) {
        vga[row * 80 + col] = ((uint16_t)color << 8) | (uint8_t)s[col];
        col++;
    }
}

void neo_kernel_main(unsigned long magic, unsigned long mb_info_addr) {
    if ((uint32_t)magic == MULTIBOOT2_BOOTLOADER_MAGIC) {
        struct mb2_framebuffer_tag *fb = find_framebuffer_tag((uintptr_t)mb_info_addr);
        if (fb != 0 && fb->bpp == 32) {
            fill_framebuffer(fb, 0x1e1e2e);
            draw_rect(fb, 40, 40, 360, 80, 0x89b4fa);
            draw_rect(fb, 420, 40, 360, 80, 0xa6e3a1);
            draw_rect(fb, 800, 40, 360, 80, 0xf9e2af);

            for (;;) {
                __asm__ volatile("hlt");
            }
        }
    }

    vga_write("neoOS fallback: VGA text path", 0x0F, 0);
    vga_write("Framebuffer tag unavailable", 0x0C, 1);

    for (;;) {
        __asm__ volatile("hlt");
    }
}
