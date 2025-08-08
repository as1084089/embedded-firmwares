#ifndef UART_H
#define UART_H

#include <types.h>

#define PR_ENDL uart_println()

// UART low-level functions
void uart_putc(char c);
char uart_getc(void);

// UART print fuctions
void uart_print_str(const char *str);
void uart_print_dec(int32_t num);
void uart_print_hex(uint32_t num);
void uart_println(void);

#endif /* UART_H */