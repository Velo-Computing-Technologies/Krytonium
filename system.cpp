#include "orion.h"
#include "logo.h"

uint8_t* vga_graphics_buffer = (uint8_t*)0xA0000;
uint8_t double_buffer[320 * 200];

void draw_pixel(int x, int y, uint8_t color) {
    if (x >= 0 && x < 320 && y >= 0 && y < 200) {
        double_buffer[(y * 320) + x] = color;
    }
}

void draw_rect(int x, int y, int width, int height, uint8_t color) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            draw_pixel(x + j, y + i, color);
        }
    }
}

void draw_logo(int start_x, int start_y) {
    for (int y = 0; y < 120; y++) {
        for (int x = 0; x < 120; x++) {
            uint8_t color_index = logo_pixels[(y * 120) + x];
            draw_pixel(start_x + x, start_y + y, color_index);
        }
    }
}

void flush_buffer_to_screen() {
    for (int i = 0; i < 320 * 200; i++) {
        vga_graphics_buffer[i] = double_buffer[i];
    }
}

void render_taskbar() {
    draw_rect(0, 180, 320, 20, theme_taskbar_color); // Taskbar
    draw_rect(2, 182, 30, 16, 15);                   // Start Button (White)
}

void init_system() {
    load_defaults();
}
