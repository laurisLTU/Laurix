// console.c — Laurix VGA text console with scrolling

#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((uint16_t*)0xB8000)

static int cursor_x = 0;
static int cursor_y = 0;
static uint8_t color = 0x0F; // white on black

// ------------------------------------------------------------
// Scroll screen up by 1 line
// ------------------------------------------------------------
static void scroll() {
    // If we're not past the last line, do nothing
    if (cursor_y < VGA_HEIGHT)
        return;

    // Move every line up by one
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[(y - 1) * VGA_WIDTH + x] =
                VGA_MEMORY[y * VGA_WIDTH + x];
        }
    }

    // Clear last line
    for (int x = 0; x < VGA_WIDTH; x++) {
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + x] =
            (color << 8) | ' ';
    }

    cursor_y = VGA_HEIGHT - 1;
}

// ------------------------------------------------------------
// Low-level character output
// ------------------------------------------------------------
static void putc(char c) {
    // Newline
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        scroll();
        return;
    }

    // Backspace
    if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            VGA_MEMORY[cursor_y * VGA_WIDTH + cursor_x] =
                (color << 8) | ' ';
        }
        return;
    }

    // Normal character
    VGA_MEMORY[cursor_y * VGA_WIDTH + cursor_x] = (color << 8) | c;
    cursor_x++;

    // Wrap line
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }

    scroll();
}

// ------------------------------------------------------------
// High-level printing
// ------------------------------------------------------------
void print(const char* s) {
    while (*s) {
        putc(*s++);
    }
}

void print_line(const char* s) {
    print(s);
    putc('\n');
}

// ------------------------------------------------------------
// Clear screen
// ------------------------------------------------------------
void clear_screen() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[y * VGA_WIDTH + x] = (color << 8) | ' ';
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

// ------------------------------------------------------------
// Print integer
// ------------------------------------------------------------
void print_int(int v) {
    char buf[16];
    int i = 0;

    if (v == 0) {
        print("0");
        return;
    }

    int neg = 0;
    if (v < 0) {
        neg = 1;
        v = -v;
    }

    while (v > 0) {
        buf[i++] = '0' + (v % 10);
        v /= 10;
    }

    if (neg) buf[i++] = '-';

    while (i--) {
        putc(buf[i]);
    }
}
