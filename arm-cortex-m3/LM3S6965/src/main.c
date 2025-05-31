// main.c
#define ASCII_OFFSET 48

#include "types.h"
#include "uart_print.h"

//extern volatile unsigned int* UART0_DR;

int main() {
    const char *hello = "Hello QEMU!\n";
    uart_puts(hello);
    while (1);
    return 0;
}