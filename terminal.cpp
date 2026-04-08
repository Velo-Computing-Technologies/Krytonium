#include "orion.h"

void render_terminal() {
    // Draw the terminal window
    draw_rect(40, 30, 240, 140, 0); // Black terminal background
    draw_rect(40, 30, 240, 15, 7);  // Gray title bar
    
    // Draw a fake cursor prompt (Green)
    draw_rect(45, 50, 8, 12, 2); 
}
