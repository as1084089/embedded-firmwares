// main.c

#include "types.h"
#include "uart_print.h"

//extern volatile unsigned int* UART0_DR;

int main() {
    const char *hello = "Hello QEMU!";
    uart_print_str(hello); PR_ENDL;
    uart_print_dec(1009); PR_ENDL;
    uart_print_dec(-55); PR_ENDL;
    uart_print_hex(0xaaf998f0); PR_ENDL;
    uart_print_str("End"); PR_ENDL;
    while (1);
    return 0;
}