#include <stdint.h>
#include "keyboard.h"

#define KBD_DATA 0x60
#define KBD_STATUS 0x64

static uint8_t shift = 0;
static uint8_t caps = 0;

static char keymap[128] = {
    0,27,'1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0,'*',0,' ',
};

static char keymap_shift[128] = {
    0,27,'!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
    'A','S','D','F','G','H','J','K','L',':','"','~', 0,'|',
    'Z','X','C','V','B','N','M','<','>','?', 0,'*',0,' ',
};

static inline uint8_t inb(uint16_t port) {
    uint8_t r;
    __asm__ volatile("inb %1, %0" : "=a"(r) : "dN"(port));
    return r;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "dN"(port));
}

static char buffer[128];
static int buf_head = 0, buf_tail = 0;

static void buffer_push(char c) {
    buffer[buf_head] = c;
    buf_head = (buf_head + 1) % 128;
}

int keyboard_read_char() {
    if (buf_tail == buf_head) return -1;
    char c = buffer[buf_tail];
    buf_tail = (buf_tail + 1) % 128;
    return c;
}

void keyboard_irq() {
    uint8_t sc = inb(KBD_DATA);

    // Key release
    if (sc & 0x80) {
        sc &= 0x7F;
        if (sc == 0x2A || sc == 0x36) shift = 0;
        return;
    }

    // Key press
    if (sc == 0x2A || sc == 0x36) { shift = 1; return; }
    if (sc == 0x3A) { caps ^= 1; return; }

    char c = shift ? keymap_shift[sc] : keymap[sc];

    if (caps && c >= 'a' && c <= 'z')
        c -= 32;

    if (c)
        buffer_push(c);
}

void keyboard_init() {
    // Enable IRQ1 (keyboard)
    outb(0x21, inb(0x21) & ~0x02);
}
