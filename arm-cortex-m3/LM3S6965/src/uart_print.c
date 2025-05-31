#include "uart_print.h"

extern volatile unsigned int* UART0_DR;

void uart_putc(char c) {
    *UART0_DR = c;
}

void uart_puts(const char *str) {
    while (*str) {
        uart_putc(*str++);
    }
}

void utoa_fast(u32 value, char *buf) {
    char temp[10];  // maximum 10 digits
    i32 i = 0;

    do {
        u32 q = value / 10;
        u32 r = value - q * 10;  // == value % 10
        temp[i++] = '0' + r;
        value = q;
    } while (value);

    // reverse
    while (i--) {
        *buf++ = temp[i];
    }
    *buf = '\0';
}