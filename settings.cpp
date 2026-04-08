#include "orion.h"

void render_settings() {
    draw_rect(60, 20, 200, 150, 7);  // Light gray background
    draw_rect(60, 20, 200, 15, 1);   // Blue title bar
    draw_rect(70, 50, 180, 40, 15);  // System Info Box
    draw_rect(70, 100, 180, 40, 15); // Dependencies Box
}
