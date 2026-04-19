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
    40,
    (uint32_t)(0u - (MULTIBOOT2_HEADER_MAGIC + 0u + 40u)),

    /* Framebuffer request tag (type=5, flags=0, size=20). */
    5u | (0u << 16),
    20u,
    1024u,
    768u,
    32u,

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

static struct mb2_framebuffer_tag *find_framebuffer_tag(uintptr_t mb_info_addr) {
    uint8_t *mb = (uint8_t *)mb_info_addr;
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

static void put_pixel(struct mb2_framebuffer_tag *fb, uint32_t x, uint32_t y, uint32_t rgb) {
    if (x >= fb->width || y >= fb->height) {
        return;
    }

    uint8_t *base = (uint8_t *)(uintptr_t)fb->addr + (uint64_t)y * fb->pitch + (uint64_t)x * (fb->bpp / 8u);

    if (fb->bpp == 32) {
        *(uint32_t *)base = rgb;
    } else if (fb->bpp == 24) {
        base[0] = (uint8_t)(rgb & 0xFFu);
        base[1] = (uint8_t)((rgb >> 8) & 0xFFu);
        base[2] = (uint8_t)((rgb >> 16) & 0xFFu);
    }
}

static void fill_rect(struct mb2_framebuffer_tag *fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    uint32_t x2 = x + w;
    uint32_t y2 = y + h;
    if (x2 > fb->width) {
        x2 = fb->width;
    }
    if (y2 > fb->height) {
        y2 = fb->height;
    }

    for (uint32_t py = y; py < y2; py++) {
        for (uint32_t px = x; px < x2; px++) {
            put_pixel(fb, px, py, color);
        }
    }
}

static void fill_circle(struct mb2_framebuffer_tag *fb, int cx, int cy, int r, uint32_t color) {
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x * x + y * y <= r * r) {
                int px = cx + x;
                int py = cy + y;
                if (px >= 0 && py >= 0) {
                    put_pixel(fb, (uint32_t)px, (uint32_t)py, color);
                }
            }
        }
    }
}

static void draw_gradient_bg(struct mb2_framebuffer_tag *fb) {
    for (uint32_t y = 0; y < fb->height; y++) {
        uint32_t r = 18u + (20u * y) / (fb->height ? fb->height : 1u);
        uint32_t g = 26u + (35u * y) / (fb->height ? fb->height : 1u);
        uint32_t b = 46u + (60u * y) / (fb->height ? fb->height : 1u);
        uint32_t color = (r << 16) | (g << 8) | b;
        fill_rect(fb, 0, y, fb->width, 1, color);
    }
}

static void draw_window(struct mb2_framebuffer_tag *fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    fill_rect(fb, x, y, w, h, 0xdce0e8);
    fill_rect(fb, x, y, w, 34, 0xc4c9d4);

    fill_circle(fb, (int)x + 18, (int)y + 17, 6, 0xff5f57);
    fill_circle(fb, (int)x + 36, (int)y + 17, 6, 0xfebb2e);
    fill_circle(fb, (int)x + 54, (int)y + 17, 6, 0x28c840);

    fill_rect(fb, x + 12, y + 50, w - 24, h - 62, 0xf4f6fb);
}

static void draw_top_bar(struct mb2_framebuffer_tag *fb) {
    fill_rect(fb, 0, 0, fb->width, 28, 0x1a1d28);
    fill_rect(fb, 10, 8, 8, 8, 0xffffff);
    fill_rect(fb, fb->width > 80 ? fb->width - 80 : 0, 7, 65, 14, 0x2d3342);
}

static void draw_dock(struct mb2_framebuffer_tag *fb) {
    uint32_t dock_h = 88;
    uint32_t y = (fb->height > dock_h + 12) ? fb->height - dock_h - 12 : 0;
    uint32_t dock_w = fb->width > 680 ? 680 : (fb->width - 20);
    uint32_t x = (fb->width - dock_w) / 2;

    fill_rect(fb, x, y, dock_w, dock_h, 0x3c4256);

    uint32_t icon_x = x + 24;
    uint32_t icon_y = y + 20;
    uint32_t sizes[8] = {44, 50, 58, 66, 58, 50, 44, 38};
    uint32_t colors[8] = {0x89b4fa, 0xf38ba8, 0xa6e3a1, 0xf9e2af, 0xcba6f7, 0x94e2d5, 0xfab387, 0xb4befe};

    for (int i = 0; i < 8; i++) {
        uint32_t s = sizes[i];
        uint32_t iy = icon_y + (66 - s);
        fill_rect(fb, icon_x, iy, s, s, colors[i]);
        icon_x += s + 12;
    }
}

static void draw_desktop(struct mb2_framebuffer_tag *fb) {
    draw_gradient_bg(fb);
    draw_top_bar(fb);

    uint32_t w1 = fb->width > 540 ? 520 : fb->width - 40;
    uint32_t h1 = fb->height > 380 ? 340 : fb->height / 2;
    draw_window(fb, 40, 54, w1, h1);

    uint32_t w2 = fb->width > 460 ? 420 : fb->width - 40;
    uint32_t h2 = fb->height > 330 ? 280 : fb->height / 2;
    uint32_t x2 = fb->width > w2 + 60 ? fb->width - w2 - 40 : 20;
    uint32_t y2 = fb->height > h2 + 180 ? 110 : 40;
    draw_window(fb, x2, y2, w2, h2);

    draw_dock(fb);
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
        if (fb != 0 && (fb->bpp == 32 || fb->bpp == 24)) {
            draw_desktop(fb);
            for (;;) {
                __asm__ volatile("hlt");
            }
        }
    }

    vga_write("neoOS: framebuffer unavailable", 0x0F, 0);
    vga_write("No MB2 framebuffer tag. Check VM graphics settings.", 0x0C, 1);

    for (;;) {
        __asm__ volatile("hlt");
    }
}
