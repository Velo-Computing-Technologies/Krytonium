#ifndef ORION_H
#define ORION_H

// Bare-metal standard types (since we don't have <stdint.h>)
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned int size_t;

// Dummy snprintf signature to satisfy the mbedTLS mockup in login.cpp
extern "C" int snprintf(char * s, size_t n, const char * format, ...);

// --- Graphics Core (Implemented in system.cpp) ---
extern uint8_t* vga_graphics_buffer;
void draw_pixel(int x, int y, uint8_t color);
void draw_rect(int x, int y, int width, int height, uint8_t color);
void draw_logo(int start_x, int start_y);

// --- KAN-1: Supabase Login (login.cpp) ---
void render_login_screen();
bool is_authenticated();

// --- KAN-4: Home Screen (main.cpp) ---
void render_home_screen();

// --- KAN-5: App View (app.cpp) ---
void render_app_view();

// --- KAN-6: Terminal (terminal.cpp) ---
void render_terminal();

// --- KAN-7: Default Settings (default.cpp) ---
extern uint8_t theme_bg_color;
extern uint8_t theme_taskbar_color;
extern uint8_t theme_text_color;
void load_defaults();

// --- KAN-8: System Apps (system.cpp) ---
void render_taskbar();
void init_system();

// --- Settings & Info (settings.cpp) ---
void render_settings();

#endif
