#include "process.h"
#include "uart_print.h"

volatile __pp_t __pp;
__pcb_t __proc_list[__MAX_PROC_NUM];

void __init_process_pool() {
    __pp.process_counter = 0;
    for (uint32_t iter = 0; iter < __MAX_PROC_NUM; iter++) {
        __proc_list[iter].process_id = 0;
        __proc_list[iter].process_stack_pointer = 0;
    }
    __pp.psp_pool_top = &_psp_pool_start;
}

int32_t __init_process_control_block() {
    // if the number of processes are the maximum,
    if (__pp.process_counter >= __MAX_PROC_NUM) {
        return -1;  // cannot initialize new process
    } else {
        __proc_list[__pp.process_counter].process_id = __pp.process_counter;
        __proc_list[__pp.process_counter].process_stack_pointer = __pp.psp_pool_top;
        __pp.process_counter++;
        __pp.psp_pool_top -= __PSP_SIZE;
        return __pp.process_counter - 1;
    }
}

void __init_process_context(addr_t func) {
    int32_t ret = __init_process_control_block();
    if (ret < 0) return;

    uint32_t *psp = (uint32_t*)__proc_list[ret].process_stack_pointer;

    // Exception Stack Frame: r0~r3, r12, lr, pc, xPSR
    *(--psp) = 0x01000000;     // xPSR (T-bit = 1)
    *(--psp) = (uint32_t)func;    // pc
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

    __proc_list[ret].process_stack_pointer = (addr_t)psp;
}
