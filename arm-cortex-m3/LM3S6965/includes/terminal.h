#ifndef TERMINAL_H
#define TERMINAL_H

#define TERMINAL_MAXLEN 128

void terminal_print_prompt(void);
void terminal_read_line(char *buf, int maxlen);
void terminal(void);

#endif /* TERMINAL_H */
