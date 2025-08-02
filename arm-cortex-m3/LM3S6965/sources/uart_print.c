#include "uart_print.h"

extern volatile unsigned int* UART0_DR;

__attribute__ ((section(".bss")))
char uart_print_buf[10];

void clear_buf() {
    for (__i32 iter = 0; iter < 10; iter++) {
        uart_print_buf[iter] = 0;
    }
}

void uart_print_char(char c) {
    *UART0_DR = c;
}

void uart_print_str(const char *str) {
    while (*str) {
        uart_print_char(*str++);
    }
}

void uart_print_dec(__i32 num) {
    __i32 iter = 0;
    if (num == 0) {
        uart_print_char('0');
        return;
    }
    else if (num < 0) {
        uart_print_char('-');
        num *= -1;
    }
    while (num > 0) {
        uart_print_buf[iter++] = ASCII_DIGIT_OFFSET + (num % 10);
        num /= 10;
    }
    while (iter--) {
        uart_print_char(uart_print_buf[iter]);
    }
    clear_buf();
}

/* 2025.06.01 am 02:34 / dahun / bug-fix
 * Declaration of string literal in the fuction makes the compiler save link register using push instruction
 * at the beginning of the function and pop it at the end of the function, and it seems to lead to the panic.
 * So replaced the string and `uart_print_str()` function usage with the combination of `uart_print_char()`.
 */
void uart_print_hex(__u32 num) {
    __i32 iter = 0;
    if (num == 0) {
        uart_print_char('0');
        uart_print_char('x');
        uart_print_char('0');
        return;
    }
    uart_print_char('0');
    uart_print_char('x');
    while (num > 0) {
        uart_print_buf[iter++] = \
            (num % 16) > 9 ? \
            (num % 16) + ASCII_CCHAR_OFFSET - 10 : \
            (num % 16) + ASCII_DIGIT_OFFSET;
        num >>= 4;
    }
    while (iter--) {
        uart_print_char(uart_print_buf[iter]);
    }
    clear_buf();
}

void uart_println() {
    uart_print_char('\n');
}