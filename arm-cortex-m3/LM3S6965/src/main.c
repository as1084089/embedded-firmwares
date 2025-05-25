// main.c
volatile unsigned int *UART0_DR = (unsigned int *)0x4000C000;

int main() {
    const char *hello = "Hello QEMU!\n";
    while (*hello) {
        *UART0_DR = *hello++;
    }
    while (1);
    return 0;
}
