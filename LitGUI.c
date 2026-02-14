// LitGUI.c — keyboard-based GUI

#include "LitGUI.h"

extern void outb(unsigned short port, unsigned char val);
extern unsigned char inb(unsigned short port);
extern void clear_screen();
extern void print_line(const char* s);

// VGA framebuffer
static unsigned char* vga = (unsigned char*)0xA0000;

static int cursor_x = 160;
static int cursor_y = 100;

static int gui_running = 0;

// ------------------------------------------------------------
// VGA mode 13h (320x200x256)
// ------------------------------------------------------------
static void enter_graphics() {
    outb(0x3C2, 0x63);
    outb(0x3D4, 0x00); outb(0x3D5, 0x00);
    outb(0x3D4, 0x01); outb(0x3D5, 0x00);
    outb(0x3D4, 0x02); outb(0x3D5, 0x00);
    outb(0x3D4, 0x03); outb(0x3D5, 0x00);
    outb(0x3D4, 0x04); outb(0x3D5, 0x00);
    outb(0x3D4, 0x05); outb(0x3D5, 0x00);
    outb(0x3D4, 0x06); outb(0x3D5, 0xBF);
    outb(0x3D4, 0x07); outb(0x3D5, 0x1F);
    outb(0x3D4, 0x08); outb(0x3D5, 0x00);
    outb(0x3D4, 0x09); outb(0x3D5, 0x41);
    outb(0x3D4, 0x10); outb(0x3D5, 0x9C);
    outb(0x3D4, 0x11); outb(0x3D5, 0x8E);
    outb(0x3D4, 0x12); outb(0x3D5, 0x8F);
    outb(0x3D4, 0x13); outb(0x3D5, 0x28);
    outb(0x3D4, 0x14); outb(0x3D5, 0x40);
    outb(0x3D4, 0x15); outb(0x3D5, 0x96);
    outb(0x3D4, 0x16); outb(0x3D5, 0xB9);
    outb(0x3D4, 0x17); outb(0x3D5, 0xA3);
}

static void exit_graphics() {
    // Return to text mode 03h
    asm volatile (
        "mov $0x03, %ax\n"
        "int $0x10\n"
    );
}

// ------------------------------------------------------------
// Basic drawing
// ------------------------------------------------------------
void lg_putpixel(int x, int y, uint32_t color) {
    if (x < 0 || y < 0 || x >= 320 || y >= 200) return;
    vga[y * 320 + x] = (unsigned char)color;
}

void lg_clear(uint32_t color) {
    for (int i = 0; i < 320 * 200; i++)
        vga[i] = (unsigned char)color;
}

// ------------------------------------------------------------
// Keyboard cursor
// ------------------------------------------------------------
void lg_move_cursor(int dx, int dy) {
    cursor_x += dx;
    cursor_y += dy;

    if (cursor_x < 0) cursor_x = 0;
    if (cursor_y < 0) cursor_y = 0;
    if (cursor_x > 319) cursor_x = 319;
    if (cursor_y > 199) cursor_y = 199;
}

void lg_draw_cursor() {
    lg_putpixel(cursor_x, cursor_y, 15); // white dot
}

// ------------------------------------------------------------
// GUI entry
// ------------------------------------------------------------
void lg_init() {
    enter_graphics();
    lg_clear(8); // gray background
    gui_running = 1;
}

void lg_shutdown() {
    gui_running = 0;
    exit_graphics();
    clear_screen();
    print_line("Exited GUI mode.");
}

// ------------------------------------------------------------
// Main GUI loop (keyboard only)
// ------------------------------------------------------------
void lg_run() {
    while (gui_running) {
        unsigned char sc = inb(0x60);

        if (sc == 0x48) lg_move_cursor(0, -2); // up
        if (sc == 0x50) lg_move_cursor(0,  2); // down
        if (sc == 0x4B) lg_move_cursor(-2, 0); // left
        if (sc == 0x4D) lg_move_cursor( 2, 0); // right

        if (sc == 0x01) { // ESC
            lg_shutdown();
            return;
        }

        lg_clear(8);
        program_panel_run();
        lg_draw_cursor();
    }
}
