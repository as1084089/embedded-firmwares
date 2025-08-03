#include "process.h"
#include "uart_print.h"

__pp_t __pp;
__pcb_t __proc_list[__MAX_PROC_NUM];
__pcb_t *__current_running;

void __init_process_pool() {
    __current_running = NULL;
    __pp.process_counter = 1;
    for (uint32_t iter = 0; iter < __MAX_PROC_NUM; iter++) {
        __proc_list[iter].process_id = -1;
        __proc_list[iter].process_stack_pointer = 0;
        __proc_list[iter].process_state = WAITING;
    }

    __pp.psp_pool_top = &_psp_pool_start;

    // Idle task initialization
    __pcb_t *taskptr = &__proc_list[0];
    taskptr->process_id = 0;
    taskptr->process_stack_pointer = __pp.psp_pool_top;
    taskptr->process_state = READY;

    __pp.psp_pool_top -= __PSP_SIZE;
}

__pcb_t* __init_process_control_block() {
    // if the number of processes are the maximum,
    if (__pp.process_counter >= __MAX_PROC_NUM) {
        return NULL;  // cannot initialize new process
    } else {
        __pcb_t *res = &__proc_list[__pp.process_counter];
        res->process_id = __pp.process_counter;
        res->process_stack_pointer = __pp.psp_pool_top;
        __pp.process_counter++;
        __pp.psp_pool_top -= __PSP_SIZE;
        return res;
    }
}

void __init_process_context(addr_t func) {
    __pcb_t *ret = __init_process_control_block();
    if (ret == NULL) return;

    ret->process_state = READY;
    uint32_t *psp = (uint32_t*)ret->process_stack_pointer;
    // uart_print_str("sp:= ");
    // uart_print_hex((uint32_t)psp); PR_ENDL;

    // Exception Stack Frame: r0~r3, r12, lr, pc, xPSR
    *(--psp) = 0x01000000;     // xPSR (T-bit = 1)
    *(--psp) = (uint32_t)func; // pc
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

    ret->process_stack_pointer = (addr_t)psp;
}

__pcb_t* __search_ready_proc() {
    for (int32_t iter = 0; iter < __MAX_PROC_NUM; iter++) {
        if (__proc_list[iter].process_state == READY) {
            return &__proc_list[iter];
        }
    }
    // 모두 실패 시
    return NULL;
}

// void __idle_task() {
//     uart_print_char('a'); PR_ENDL;
//     while (1) {}
// }

void __idle_task(void) {
    while (1) {
        uint32_t psp;
        __asm__ volatile ("mrs %0, psp" : "=r"(psp));
        uart_print_str("PSP: ");
        uart_print_hex(psp); PR_ENDL;
    }
}