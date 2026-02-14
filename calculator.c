#include "calculator.h"

extern void print(const char* s);
extern void print_line(const char* s);
extern int  read_line(char* buf, int max);
extern void print_int(int);

// simple integer parser: supports +, -, *, /
static int parse_int(const char** p) {
    int sign = 1;
    int v = 0;
    const char* s = *p;

    while (*s == ' ') s++;
    if (*s == '-') { sign = -1; s++; }
    else if (*s == '+') { s++; }

    while (*s >= '0' && *s <= '9') {
        v = v * 10 + (*s - '0');
        s++;
    }

    *p = s;
    return sign * v;
}

static int eval_expr(const char* s) {
    const char* p = s;
    int result = parse_int(&p);

    while (1) {
        while (*p == ' ') p++;
        char op = *p;
        if (op != '+' && op != '-' && op != '*' && op != '/') break;
        p++;

        int rhs = parse_int(&p);
        if (op == '+') result += rhs;
        else if (op == '-') result -= rhs;
        else if (op == '*') result *= rhs;
        else if (op == '/') {
            if (rhs != 0) result /= rhs;
        }
    }
    return result;
}

void calculator_run() {
    char line[128];

    print_line("Laurix Calculator");
    print_line("Type expressions like: 1+2*3-4");
    print_line("Empty line to exit.");
    print_line("------------------------");

    while (1) {
        print("> ");
        int n = read_line(line, sizeof(line));
        if (n <= 0) {
            print_line("Exiting Calculator.");
            return;
        }
        int r = eval_expr(line);
        print("= ");
        print_int(r);
        print("\n");
    }
}
