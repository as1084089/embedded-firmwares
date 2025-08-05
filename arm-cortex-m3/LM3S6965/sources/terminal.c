#include "terminal.h"
#include "uart.h"
#include "lm3s6965.h"

// Message buffer for log lines
static char log_buffer[TERMINAL_LOG_LINES][TERMINAL_LOG_MSG_MAXLEN + 1];
static int log_count = 0;

// Copies up to n chars from src to dest, pads with '\0' if src is shorter
char* terminal_strncpy(char *dest, const char *src, int n) {
    int i = 0;
    for (; i < n && src[i] != 0; i++)
        dest[i] = src[i];
    for (; i < n; i++)
        dest[i] = 0;
    return dest;
}

// Copies src (including the null terminator) to dest
char* terminal_strcpy(char *dest, const char *src) {
    int i = 0;
    while ((dest[i] = src[i]) != 0) i++;
    return dest;
}

// Returns the length of a null-terminated string
int terminal_strlen(const char *s) {
    int i = 0;
    while (s[i]) i++;
    return i;
}

void terminal_clear_screen(void) {
    uart_print_str("\033[2J\033[1;1H");
}

void terminal_move_cursor(int row, int col) {
    uart_print_char('\033');
    uart_print_char('[');
    uart_print_dec(row); // row: 1~24
    uart_print_char(';');
    uart_print_dec(col); // col: 1~80
    uart_print_char('H');
}

void terminal_draw_log_messages(void) {
    for (int i = 0; i < TERMINAL_LOG_LINES; i++) {
        terminal_move_cursor(i + 1, 1);
        uart_print_str("\033[2K");
        uart_print_str("\033[48;2;39;16;32m");
        uart_print_str("\033[97m");
        if (i < log_count) {
            uart_print_str("You typed: ");
            uart_print_str(log_buffer[i]);
            int filled = terminal_strlen("You typed: ") + terminal_strlen(log_buffer[i]);
            for (int j = filled; j < TERMINAL_SCREEN_WIDTH; j++) {
                uart_print_char(' ');
            }
        } else {
            for (int j = 0; j < TERMINAL_SCREEN_WIDTH; j++) {
                uart_print_char(' ');
            }
        }
        uart_print_str("\033[0m");
    }
}

void terminal_draw_input_prompt(const char *input, int cursor_pos) {
    const char* prompt_host = "qemu@lm3s6965";
    const char* prompt_path1 = ":";
    const char* prompt_path2 = "~";
    const char* prompt_path3 = "$ ";

    terminal_move_cursor(TERMINAL_INPUT_LINE, 1);
    uart_print_str("\033[2K");

    uart_print_str("\033[48;2;39;16;32m");

    // qemu@lm3s6965 (bold green)
    uart_print_str("\033[1;32m");
    uart_print_str(prompt_host);

    // : (white)
    uart_print_str("\033[37m");
    uart_print_str(prompt_path1);

    // ~ (bold blue)
    uart_print_str("\033[1;34m");
    uart_print_str(prompt_path2);

    // $ (bright white, normal weight)
    uart_print_str("\033[97m\033[22m");
    uart_print_str(prompt_path3);

    // Input
    uart_print_str(input);

    int filled = terminal_strlen(prompt_host) + terminal_strlen(prompt_path1)
               + terminal_strlen(prompt_path2) + terminal_strlen(prompt_path3)
               + terminal_strlen(input);
    for (int i = filled; i < TERMINAL_SCREEN_WIDTH; i++) {
        uart_print_char(' ');
    }
    uart_print_str("\033[0m");

    int col = terminal_strlen(prompt_host) + terminal_strlen(prompt_path1)
            + terminal_strlen(prompt_path2) + terminal_strlen(prompt_path3)
            + cursor_pos + 1;
    terminal_move_cursor(TERMINAL_INPUT_LINE, col);
}

void terminal_push_log(const char *msg) {
    if (log_count < TERMINAL_LOG_LINES) {
        terminal_strncpy(log_buffer[log_count], msg, TERMINAL_LOG_MSG_MAXLEN);
        log_buffer[log_count][TERMINAL_LOG_MSG_MAXLEN] = 0;
        log_count++;
    } else {
        for (int i = 1; i < TERMINAL_LOG_LINES; i++) {
            terminal_strcpy(log_buffer[i - 1], log_buffer[i]);
        }
        terminal_strncpy(log_buffer[TERMINAL_LOG_LINES - 1], msg, TERMINAL_LOG_MSG_MAXLEN);
        log_buffer[TERMINAL_LOG_LINES - 1][TERMINAL_LOG_MSG_MAXLEN] = 0;
    }
}

void terminal_read_line(char *buf, int maxlen) {
    int len = 0;
    int cur_idx = 0;
    buf[0] = 0;

    while (1) {
        char c = uart_getc();

        // ANSI escape sequence for arrow keys, delete
        if (c == 0x1B) {
            char seq1 = uart_getc();
            if (seq1 == '[') {
                char seq2 = uart_getc();
                if (seq2 == 'D') { // Left
                    if (cur_idx > 0) cur_idx--;
                } else if (seq2 == 'C') { // Right
                    if (cur_idx < len) cur_idx++;
                } else if (seq2 == '3') { // Delete: ESC [ 3 ~
                    char seq3 = uart_getc();
                    if (seq3 == '~' && cur_idx < len) {
                        for (int i = cur_idx; i < len - 1; i++)
                            buf[i] = buf[i + 1];
                        len--;
                        buf[len] = 0;
                    }
                }
            }
            terminal_draw_input_prompt(buf, cur_idx);
            continue;
        }

        // Backspace or DEL
        if (c == '\b' || c == 127) {
            if (cur_idx > 0) {
                for (int i = cur_idx - 1; i < len - 1; i++)
                    buf[i] = buf[i + 1];
                len--;
                cur_idx--;
                buf[len] = 0;
            }
            terminal_draw_input_prompt(buf, cur_idx);
            continue;
        }

        // Enter: finish line
        if (c == '\r' || c == '\n') {
            buf[len] = 0;
            terminal_draw_input_prompt(buf, len);
            uart_print_str("\r\n");
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
            terminal_draw_input_prompt(buf, cur_idx);
        }
    }
}

void terminal(void) {
    char input_buf[TERMINAL_MAXLEN];

    terminal_clear_screen();
    terminal_draw_log_messages();
    terminal_draw_input_prompt("", 0);

    while (1) {
        input_buf[0] = 0;
        terminal_draw_input_prompt("", 0);
        terminal_read_line(input_buf, TERMINAL_LOG_MSG_MAXLEN);

        if (input_buf[0]) {
            terminal_push_log(input_buf);
            terminal_clear_screen();
            terminal_draw_log_messages();
        }
        terminal_draw_input_prompt("", 0);
    }
}
