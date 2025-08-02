// main.c

#include "types.h"
#include "uart_print.h"

//extern volatile unsigned int* UART0_DR;

extern void print_sysreg();
void set_control_to_use_psp();

int main() {
    const char *hello = "Hello QEMU!";
    uart_print_str(hello); PR_ENDL;
    uart_print_dec(1009); PR_ENDL;
    uart_print_dec(-55); PR_ENDL;
    uart_print_hex(0xaaf998f0); PR_ENDL;
    uart_print_str("End"); PR_ENDL;
    print_sysreg(); PR_ENDL;
    //svcall();
    uart_print_str("After svcall"); PR_ENDL;
    print_sysreg(); PR_ENDL;

    while (1);
    return 0;
}

// void print_sysreg() {
//     u32 __psr = 0;
//     __asm__ __volatile__ (
//         "MRS R0, CONTROL" : "=r"(__psr)
//     );
//     uart_print_hex(__psr);
// }

void set_control_to_use_psp() {
    __u32 __control = 0x00000002;
    __asm__ __volatile__ (
        "MSR CONTROL, R0" :: "r"(__control)
    );
}