#ifndef TERMINAL_H
#define TERMINAL_H

#define TERMINAL_MAXLEN 128
#define TERMINAL_SCREEN_WIDTH 80
#define TERMINAL_SCREEN_HEIGHT 24
#define TERMINAL_INPUT_LINE (TERMINAL_SCREEN_HEIGHT)
#define TERMINAL_LOG_LINES (TERMINAL_SCREEN_HEIGHT - 1)
#define TERMINAL_LOG_MSG_MAXLEN (TERMINAL_SCREEN_WIDTH - 12)

void terminal_clear_screen(void);
void terminal_move_cursor(int row, int col);
void terminal_draw_log_messages(void);
void terminal_draw_input_prompt(const char *input, int cursor_pos);
void terminal_push_log(const char *msg);
void terminal_read_line(char *buf, int maxlen);
void terminal(void);

// Optionally export utility functions (strncpy, strcpy, strlen)
char* terminal_strncpy(char *dest, const char *src, int n);
char* terminal_strcpy(char *dest, const char *src);
int terminal_strlen(const char *s);

#endif /* TERMINAL_H */
