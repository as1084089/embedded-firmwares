#include "uart.h"
#include "lm3s6965.h"

#define ASCII_DIGIT_OFFSET '0'
#define ASCII_CCHAR_OFFSET 'A'
#define ASCII_SCHAR_OFFSET 'a'

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
    /* Stack-local buffer makes this reentrant: safe to call from both
     * thread context and ISR (e.g., HardFault_Handler) without races. */
    char buf[12];
    int32_t iter = 0;
    if (num == 0) {
        uart_putc('0');
        return;
    }
    /* INT32_MIN cannot be negated in two's complement; handle as unsigned. */
    uint32_t u;
    if (num < 0) {
        uart_putc('-');
        u = (uint32_t)(-(num + 1)) + 1u;
    } else {
        u = (uint32_t)num;
    }
    while (u > 0) {
        buf[iter++] = ASCII_DIGIT_OFFSET + (u % 10);
        u /= 10;
    }
    while (iter--) {
        uart_putc(buf[iter]);
    }
}

void uart_print_hex(uint32_t num) {
    char buf[8];
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
        buf[iter++] = \
            (num % 16) > 9 ? \
            (num % 16) + ASCII_CCHAR_OFFSET - 10 : \
            (num % 16) + ASCII_DIGIT_OFFSET;
        num >>= 4;
    }
    while (iter--) {
        uart_putc(buf[iter]);
    }
}

void uart_println(void) {
    uart_putc('\n');
}
