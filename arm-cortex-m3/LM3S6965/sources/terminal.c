#include "terminal.h"
#include "uart.h"

// Prints the Ubuntu/QEMU-style prompt with color and background
void terminal_print_prompt(void) {
    // Background: dark purple, qemu: bold green, path: blue, prompt: white
    uart_print_str("\033[1;32m");            // bold green
    uart_print_str("qemu@lm3s6965");
    uart_print_str("\033[0m"); // reset, bg, bright white
    uart_print_str("\033[37m");
    uart_print_str(":");
    uart_print_str("\033[1;34m");
    uart_print_str("~");
    uart_print_str("\033[97m\033[22m");
    uart_print_str("$ ");
    uart_print_str("\033[0m"); // reset
}

// Reads a single line from UART with editing (supports left/right/del/backspace)
// The input is echoed to the terminal. No line-clearing, no cursor move.
void terminal_read_line(char *buf, int maxlen) {
    int len = 0;
    int cur_idx = 0;
    buf[0] = 0;

    while (1) {
        char c = uart_getc();

        // Handle ANSI escape sequences (arrow keys, delete)
        if (c == 0x1B) {
            char seq1 = uart_getc();
            if (seq1 == '[') {
                char seq2 = uart_getc();
                if (seq2 == 'D') { // Left arrow
                    if (cur_idx > 0) {
                        uart_print_str("\033[1D");
                        cur_idx--;
                    }
                } else if (seq2 == 'C') { // Right arrow
                    if (cur_idx < len) {
                        uart_print_str("\033[1C");
                        cur_idx++;
                    }
                } else if (seq2 == '3') { // Delete (ESC [ 3 ~)
                    char seq3 = uart_getc();
                    if (seq3 == '~' && cur_idx < len) {
                        for (int i = cur_idx; i < len - 1; i++)
                            buf[i] = buf[i + 1];
                        len--;
                        buf[len] = 0;
                        // Redraw from cursor position
                        uart_print_str("\033[s"); // Save cursor pos
                        uart_print_str(&buf[cur_idx]);
                        uart_putc(' ');
                        uart_print_str("\033[u"); // Restore cursor
                    }
                }
            }
            continue;
        }

        // Backspace
        if (c == '\b' || c == 127) {
            if (cur_idx > 0) {
                // Move cursor left
                uart_print_str("\033[1D");
                for (int i = cur_idx - 1; i < len - 1; i++)
                    buf[i] = buf[i + 1];
                len--;
                cur_idx--;
                buf[len] = 0;
                // Redraw from cursor position
                uart_print_str("\033[s");
                uart_print_str(&buf[cur_idx]);
                uart_putc(' ');
                uart_print_str("\033[u");
            }
            continue;
        }

        // Enter: finish line
        if (c == '\r' || c == '\n') {
            buf[len] = 0;
            uart_println();
            break;
        }

        // Printable ASCII
        if (len < maxlen - 1 && c >= 32 && c <= 126) {
            for (int i = len; i > cur_idx; i--)
                buf[i] = buf[i - 1];
            buf[cur_idx] = c;
            len++;
            cur_idx++;
            buf[len] = 0;
            // Echo inserted char and redraw rest of line
            uart_print_str("\033[s");
            uart_print_str(&buf[cur_idx - 1]);
            uart_print_str("\033[u");
            // Move cursor right (since we already printed the char)
            uart_print_str("\033[1C");
        }
    }
}

// Minimal Linux-like shell loop: shows prompt, reads input, prints output, repeats
void terminal(void) {
    char input_buf[TERMINAL_MAXLEN];
    while (1) {
        terminal_print_prompt();
        terminal_read_line(input_buf, TERMINAL_MAXLEN);

        if (input_buf[0]) {
            // Example: just echo the input (replace with command parsing if needed)
            uart_print_str("You typed: ");
            uart_print_str(input_buf);
            uart_println();
        }
    }
}
