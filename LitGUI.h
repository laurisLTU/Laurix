// LitGUI.h — Laurix simple GUI (keyboard-controlled)

#ifndef LITGUI_H
#define LITGUI_H

#include <stdint.h>

void lg_init();        // enter graphics mode
void lg_run();         // main GUI loop
void lg_shutdown();    // exit graphics mode

void lg_clear(uint32_t color);
void lg_putpixel(int x, int y, uint32_t color);

// Keyboard-controlled cursor
void lg_move_cursor(int dx, int dy);
void lg_draw_cursor();

// Program panel launcher
void program_panel_run();

#endif
