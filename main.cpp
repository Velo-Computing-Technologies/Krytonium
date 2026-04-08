#include "orion.h"
#include "background.h"

static inline uint8_t port_byte_in(uint16_t port) {
    uint8_t result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

static inline void port_byte_out(uint16_t port, uint8_t data) {
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

int mouse_x = 160, mouse_y = 100;
uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];

void init_ps2_mouse() {
    port_byte_out(0x64, 0xA8);
    port_byte_out(0x64, 0x20);
    uint8_t status = port_byte_in(0x60) | 2;
    port_byte_out(0x64, 0x60);
    port_byte_out(0x60, status);
    port_byte_out(0x64, 0xD4);
    port_byte_out(0x60, 0xF6);
    port_byte_in(0x60); 
    port_byte_out(0x64, 0xD4);
    port_byte_out(0x60, 0xF4);
    port_byte_in(0x60); 
}

void poll_input() {
    uint8_t status = port_byte_in(0x64);
    if (status & 0x01) {
        if (status & 0x20) { 
            mouse_byte[mouse_cycle++] = port_byte_in(0x60);
            if (mouse_cycle == 3) {
                mouse_cycle = 0;
                mouse_x += mouse_byte[1];
                mouse_y -= mouse_byte[2];
                if (mouse_x < 0) mouse_x = 0;
                if (mouse_x > 318) mouse_x = 318;
                if (mouse_y < 0) mouse_y = 0;
                if (mouse_y > 198) mouse_y = 198;
            }
        } else {
            port_byte_in(0x60); // Read keyboard so buffer doesn't fill
        }
    }
}

void render_home_screen() {
    // Draw Background from Google Images
    for (int i = 0; i < 320 * 200; i++) {
        double_buffer[i] = bg_pixels[i];
    }
    
    render_taskbar();
    draw_logo(100, 30);

    // Draw Mouse Cursor
    draw_pixel(mouse_x, mouse_y, 15);
    draw_pixel(mouse_x+1, mouse_y, 15);
    draw_pixel(mouse_x, mouse_y+1, 15);
    draw_pixel(mouse_x+1, mouse_y+1, 15);
}

extern "C" void kernel_main() {
    init_system();
    init_ps2_mouse();

    while (true) {
        poll_input();
        render_home_screen(); // Or render_login_screen() if you want to test login first
        flush_buffer_to_screen();
    }
}
