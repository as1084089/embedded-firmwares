#include "lm3s6965.h"
#include "uart_print.h"

#define MAXLEN 128
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24
#define INPUT_LINE (SCREEN_HEIGHT)
#define LOG_LINES (SCREEN_HEIGHT - 1)
#define LOG_MSG_MAXLEN (SCREEN_WIDTH - 12) // "You typed: " + msg = 80

// Message buffer for log lines
char log_buffer[LOG_LINES][LOG_MSG_MAXLEN + 1];
int log_count = 0; // Current # of log messages

// Copies up to n chars from src to dest, pads with '\0' if src is shorter
char* strncpy(char *dest, const char *src, int n) {
    int i = 0;
    for (; i < n && src[i] != 0; i++)
        dest[i] = src[i];
    for (; i < n; i++)
        dest[i] = 0;
    return dest;
}

// Copies src (including the null terminator) to dest
char* strcpy(char *dest, const char *src) {
    int i = 0;
    while ((dest[i] = src[i]) != 0) i++;
    return dest;
}

// Returns the length of a null-terminated string
int strlen(const char *s) {
    int i = 0;
    while (s[i]) i++;
    return i;
}

// Clears the entire terminal screen and moves cursor to (1, 1)
void clear_screen() {
    uart_print_str("\033[2J\033[1;1H");
}

// Moves the cursor to a specific (row, col)
void move_cursor(int row, int col) {
    uart_print_char('\033');
    uart_print_char('[');
    uart_print_dec(row); // row: 1~24
    uart_print_char(';');
    uart_print_dec(col); // col: 1~80
    uart_print_char('H');
}

// Draws all current log messages in the upper 23 lines
void draw_log_messages() {
    for (int i = 0; i < LOG_LINES; i++) {
        move_cursor(i + 1, 1);
        uart_print_str("\033[2K"); // Clear current line
        if (i < log_count) {
            uart_print_str("You typed: ");
            uart_print_str(log_buffer[i]);
        }
    }
}

// Redraws the input field at the bottom (24th row)
void draw_input_prompt(const char *input, int cursor_pos) {
    move_cursor(INPUT_LINE, 1);
    uart_print_str("\033[2K"); // Clear input line
    uart_print_str("Input> ");
    uart_print_str(input);

    // Move cursor to the correct position in the input field
    int col = (int)strlen("Input> ") + cursor_pos + 1;
    move_cursor(INPUT_LINE, col);
}

// Scrolls up the log (FIFO)
void push_log(const char *msg) {
    if (log_count < LOG_LINES) {
        strncpy(log_buffer[log_count], msg, LOG_MSG_MAXLEN);
        log_buffer[log_count][LOG_MSG_MAXLEN] = 0;
        log_count++;
    } else {
        // Remove top, shift all up
        for (int i = 1; i < LOG_LINES; i++) {
            strcpy(log_buffer[i - 1], log_buffer[i]);
        }
        strncpy(log_buffer[LOG_LINES - 1], msg, LOG_MSG_MAXLEN);
        log_buffer[LOG_LINES - 1][LOG_MSG_MAXLEN] = 0;
    }
}

char uart_read_char(void) {
    while (UART0->FR & (1 << 4));
    return (char)(UART0->DR & 0xFF);
}

void uart_read_line(char *buf, int maxlen) {
    int len = 0;
    int cur_idx = 0;
    buf[0] = 0;

    while (1) {
        char c = uart_read_char();

        // ANSI escape sequence for arrow keys, delete
        if (c == 0x1B) {
            char seq1 = uart_read_char();
            if (seq1 == '[') {
                char seq2 = uart_read_char();
                if (seq2 == 'D') { // Left
                    if (cur_idx > 0) cur_idx--;
                } else if (seq2 == 'C') { // Right
                    if (cur_idx < len) cur_idx++;
                } else if (seq2 == '3') { // Delete: ESC [ 3 ~
                    char seq3 = uart_read_char();
                    if (seq3 == '~' && cur_idx < len) {
                        for (int i = cur_idx; i < len - 1; i++)
                            buf[i] = buf[i + 1];
                        len--;
                        buf[len] = 0;
                    }
                }
            }
            draw_input_prompt(buf, cur_idx);
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
            draw_input_prompt(buf, cur_idx);
            continue;
        }

        // Enter: finish line
        if (c == '\r' || c == '\n') {
            buf[len] = 0;
            draw_input_prompt(buf, len);
            uart_print_str("\r\n"); // Optional, move to next line (not needed if you always redraw screen)
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
            draw_input_prompt(buf, cur_idx);
        }
    }
}

void uart_input(void) {
    char input_buf[MAXLEN];

    clear_screen();
    draw_log_messages();
    draw_input_prompt("", 0);

    while (1) {
        input_buf[0] = 0;
        draw_input_prompt("", 0);
        uart_read_line(input_buf, LOG_MSG_MAXLEN);

        if (input_buf[0]) {
            push_log(input_buf);      // Add message to log FIFO
            clear_screen();
            draw_log_messages();
        }
        draw_input_prompt("", 0);
    }
}
