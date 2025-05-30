// main.c
volatile unsigned int *UART0_DR = (unsigned int *)0x4000C000;

void uart_putc(char c);
void uart_puts(const char *str);

int main() {
    const char *hello = "Hello QEMU!\n";
    uart_puts(hello);
    while (1);
    return 0;
}

void uart_putc(char c) {
    *UART0_DR = c;
}

void uart_puts(const char *str) {
    while (*str) {
        uart_putc(*str++);
    }
}