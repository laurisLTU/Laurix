// kernel.c — Laurix main kernel + shell

#include <stdint.h>
#include "LitGUI.h"

extern void clear_screen();
extern void print(const char* s);
extern void print_line(const char* s);
extern int  read_line(char* buf, int max);

extern void calculator_run();
extern void notepad_run();

// simple strcmp
static int my_strcmp(const char* a, const char* b) {
    while (*a && *b && *a == *b) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

static void show_banner() {
    print_line("Laurix OS");
    print_line("Built-in shell");
    print_line("Commands: Help, Dir Apps, Calculator, Notepad, GUI");
    print_line("--------------------------------------------------");
}

static void show_help() {
    print_line("Commands:");
    print_line("  Help       - show this help");
    print_line("  Dir Apps   - list available apps");
    print_line("  Calculator - run calculator");
    print_line("  Notepad    - run text editor");
    print_line("  GUI        - enter graphics mode (LitGUI)");
}

static void show_apps() {
    print_line("Available apps:");
    print_line("  Calculator");
    print_line("  Notepad");
    print_line("  GUI (graphics mode)");
}

void kernel_main(void) {
    clear_screen();
    show_banner();

    char line[128];

    for (;;) {
        print("> ");
        int n = read_line(line, sizeof(line));
        if (n <= 0)
            continue;

        if (my_strcmp(line, "Help") == 0) {
            show_help();
        }
        else if (my_strcmp(line, "Dir Apps") == 0) {
            show_apps();
        }
        else if (my_strcmp(line, "Calculator") == 0) {
            calculator_run();
        }
        else if (my_strcmp(line, "Notepad") == 0) {
            notepad_run();
        }
        else if (my_strcmp(line, "GUI") == 0) {
            lg_init();
            lg_run();
            // when lg_run returns, you’re back in text mode
            show_banner();
        }
        else {
            print_line("Unknown command. Type 'Help'.");
        }
    }
}
