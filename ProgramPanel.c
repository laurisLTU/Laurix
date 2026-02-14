#include "LitGUI.h"

void program_panel_run() {
    // Draw two simple boxes
    for (int y = 40; y < 80; y++)
        for (int x = 40; x < 140; x++)
            lg_putpixel(x, y, 7); // light gray

    for (int y = 40; y < 80; y++)
        for (int x = 180; x < 280; x++)
            lg_putpixel(x, y, 7);

    // Titles (placeholder)
    // Later: draw bitmap fonts
}
