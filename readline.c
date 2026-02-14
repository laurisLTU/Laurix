// readline.c

#include <stdint.h>

extern uint8_t inb(uint16_t port);
extern void print(const char* s);

static int shift = 0;

static const char keymap[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ',
    /* rest zero */
};

static const char keymap_shift[128] = {
    0,  27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
    'A','S','D','F','G','H','J','K','L',':','"','~', 0, '|',
    'Z','X','C','V','B','N','M','<','>','?', 0, '*', 0, ' ',
    /* rest zero */
};

int read_line(char* buf, int max) {
    int len = 0;

    while (1) {
        // wait for key
        uint8_t status = inb(0x64);
        if (!(status & 1)) continue;

        uint8_t sc = inb(0x60);

        // shift press
        if (sc == 0x2A || sc == 0x36) { shift = 1; continue; }
        // shift release
        if (sc == 0xAA || sc == 0xB6) { shift = 0; continue; }

        // key release: ignore
        if (sc & 0x80) continue;

        char c = shift ? keymap_shift[sc] : keymap[sc];
        if (!c) continue;

        // backspace
        if (c == '\b') {
            if (len > 0) {
                len--;
                print("\b \b"); // erase last char on screen
            }
            continue;
        }

        // enter
        if (c == '\n') {
            buf[len] = 0;
            print("\n");
            return len;
        }

        // normal char
        if (len < max - 1) {
            buf[len++] = c;
            char s[2] = { c, 0 };
            print(s);
        }
    }
}
