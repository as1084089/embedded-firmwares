#include "uart.h"
#include "lm3s6965.h"

#define ASCII_DIGIT_OFFSET '0'
#define ASCII_CCHAR_OFFSET 'A'
#define ASCII_SCHAR_OFFSET 'a'

static char uart_print_buf[32];

static void clear_buf(void) {
    for (int32_t iter = 0; iter < 32; iter++) {
        uart_print_buf[iter] = 0;
    }
}

void uart_putc(char c) {
    UART0->DR = c;
}

char uart_getc(void) {
    while (UART0->FR & (1 << 4));
    return (char)(UART0->DR & 0xFF);
}

void uart_print_str(const char *str) {
    while (*str) {
        uart_putc(*str++);
    }
}

void uart_print_dec(int32_t num) {
    int32_t iter = 0;
    if (num == 0) {
        uart_putc('0');
        return;
    }
    else if (num < 0) {
        uart_putc('-');
        num *= -1;
    }
    while (num > 0) {
        uart_print_buf[iter++] = ASCII_DIGIT_OFFSET + (num % 10);
        num /= 10;
    }
    while (iter--) {
        uart_putc(uart_print_buf[iter]);
    }
    clear_buf();
}

void uart_print_hex(uint32_t num) {
    int32_t iter = 0;
    if (num == 0) {
        uart_putc('0');
        uart_putc('x');
        uart_putc('0');
        return;
    }
    uart_putc('0');
    uart_putc('x');
    while (num > 0) {
        uart_print_buf[iter++] = \
            (num % 16) > 9 ? \
            (num % 16) + ASCII_CCHAR_OFFSET - 10 : \
            (num % 16) + ASCII_DIGIT_OFFSET;
        num >>= 4;
    }
    while (iter--) {
        uart_putc(uart_print_buf[iter]);
    }
    clear_buf();
}

void uart_println(void) {
    uart_putc('\n');
}