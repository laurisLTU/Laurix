// notepad.c — Laurix simple text editor (no filesystem)

#include <stdint.h>

extern void clear_screen();
extern void print(const char* s);
extern void print_line(const char* s);
extern int  read_line(char* buf, int max);

#define MAX_TEXT 2048

static char text_buffer[MAX_TEXT];
static int text_len = 0;

static void notepad_show_menu() {
    print_line("Laurix Notepad");
    print_line("----------------------");
    print_line("Commands:");
    print_line("  :new     - clear editor");
    print_line("  :show    - display text");
    print_line("  :exit    - return to shell");
    print_line("----------------------");
}

static void notepad_new() {
    text_len = 0;
    text_buffer[0] = 0;
    print_line("Editor cleared.");
}

static void notepad_show() {
    print_line("----- TEXT BEGIN -----");
    if (text_len == 0)
        print_line("(empty)");
    else
        print_line(text_buffer);
    print_line("----- TEXT END -------");
}

void notepad_run() {
    clear_screen();
    notepad_show_menu();

    char cmd[256];

    while (1) {
        print("notepad> ");
        read_line(cmd, sizeof(cmd));

        if (!cmd[0]) continue;

        // Commands start with ':'
        if (cmd[0] == ':') {
            if (!__builtin_strcmp(cmd+1, "new")) {
                notepad_new();
            }
            else if (!__builtin_strcmp(cmd+1, "show")) {
                notepad_show();
            }
            else if (!__builtin_strcmp(cmd+1, "exit")) {
                return;
            }
            else {
                print_line("Unknown command.");
            }
        }
        else {
            // Append text to buffer
            int i = 0;
            while (cmd[i] && text_len < MAX_TEXT - 2) {
                text_buffer[text_len++] = cmd[i++];
            }
            text_buffer[text_len++] = '\n';
            text_buffer[text_len] = 0;
        }
    }
}
