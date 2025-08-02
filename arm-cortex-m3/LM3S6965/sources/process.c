#include "process.h"
#include "uart_print.h"

volatile __pp_t __pp;
__pcb_t __proc_list[__MAX_PROC_NUM];

void __init_process_pool() {
    __pp.process_counter = 0;
    for (__u32 iter = 0; iter < __MAX_PROC_NUM; iter++) {
        __proc_list[iter].process_id = 0;
        __proc_list[iter].process_stack_pointer = 0;
    }
    __pp.psp_pool_top = &_psp_pool_start;
}

__i32 __init_process_control_block() {
    // if the number of processes are the maximum,
    if (__pp.process_counter >= __MAX_PROC_NUM) {
        return -1;  // cannot initialize new process
    } else {
        __proc_list[__pp.process_counter].process_id = __pp.process_counter;
        uart_print_str("__init_pcb"); PR_ENDL;
        __proc_list[__pp.process_counter].process_stack_pointer = __pp.psp_pool_top;
        uart_print_hex((__u32)__proc_list[__pp.process_counter].process_stack_pointer); PR_ENDL;
        __pp.process_counter++;
        __pp.psp_pool_top -= __PSP_SIZE;
        uart_print_hex((__u32)__pp.psp_pool_top); PR_ENDL;
        return __pp.process_counter - 1;
    }
}

// void __init_process_context(__addr func) {
//     __i32 ret = __init_process_control_block();
//     if (!ret) return;
//     else {
//         __asm__ __volatile__ (
//             "msr psp, %[psp]\n"
//             "isb\n"
//             "mov r0, #3\n"
//             "msr control, r0\n"
//             "mov r0, #0\n"      // psr
//             "mov r1, %[pc]\n"   // pc
//             "mov r2, #4\n"      // lr
//             "mov r3, #0\n"      // r12
//             "mov r4, #0\n"      // r3
//             "mov r5, #0\n"      // r2
//             "mov r6, #0\n"      // r1
//             "mov r7, #0\n"      // r0
//             "push {r0-r7}\n"    // Exception Stack Frame
//             "push {r4-r7}\n"    // r4, r5, r6, r7
//             "push {r4-r7}\n"    // r8, r9, r10, r11
//             "isb\n"
//             :
//             : [psp] "r" (__proc_list[ret].process_stack_pointer), [pc] "r" (func)
//         );
//     }
// }

void __init_process_context(__addr func) {
    __i32 ret = __init_process_control_block();
    if (ret < 0) return;

    __u32 *psp = (__u32*)__proc_list[ret].process_stack_pointer;

    // __asm__ __volatile__ (
    //     "mov r0, #3\n"
    //     "msr control, r0\n"
    //     "isb\n"
    // );

    // Exception Stack Frame: r0~r3, r12, lr, pc, xPSR
    *(--psp) = 0x01000000;     // xPSR (T-bit = 1)
    *(--psp) = (__u32)func;    // pc
    uart_print_str("main pc"); PR_ENDL;
    uart_print_hex(*psp); PR_ENDL;
    *(--psp) = 0xFFFFFFFD;     // lr (exception return → thread, PSP)
    *(--psp) = 0x00000000;     // r12
    *(--psp) = 0x00000000;     // r3
    *(--psp) = 0x00000000;     // r2
    *(--psp) = 0x00000000;     // r1
    *(--psp) = 0x00000000;     // r0

    // r4~r11 (software saved)
    *(--psp) = 0x00000000; // r11
    *(--psp) = 0x00000000; // r10
    *(--psp) = 0x00000000; // r9
    *(--psp) = 0x00000000; // r8
    *(--psp) = 0x00000000; // r7
    *(--psp) = 0x00000000; // r6
    *(--psp) = 0x00000000; // r5
    *(--psp) = 0x00000000; // r4

    __proc_list[ret].process_stack_pointer = (__addr)psp;
}
