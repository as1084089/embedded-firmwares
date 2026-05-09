#include "process.h"
#include "uart.h"
#include "core_cm3.h"
#include "kernel_offsets.h"
#include <stddef.h>

/* Compile-time guards: keep asm magic numbers in sync with C definitions.
 * If any of these fail, context_asm.S is reading the wrong offset/value. */
_Static_assert(offsetof(pcb_t, process_state)         == PCB_OFFSET_STATE,
               "PCB_OFFSET_STATE diverged from pcb_t::process_state");
_Static_assert(offsetof(pcb_t, process_stack_pointer) == PCB_OFFSET_PSP,
               "PCB_OFFSET_PSP diverged from pcb_t::process_stack_pointer");
_Static_assert(RUNNING == PROC_STATE_RUNNING, "RUNNING enum value diverged");
_Static_assert(READY   == PROC_STATE_READY,   "READY enum value diverged");
_Static_assert(WAITING == PROC_STATE_WAITING, "WAITING enum value diverged");

proc_pool_t g_proc_pool;
pcb_t       g_proc_list[MAX_PROC_NUM];
pcb_t      *g_current_running;

void proc_pool_init() {
    g_current_running = NULL;
    g_proc_pool.process_counter = 0;
    for (uint32_t iter = 0; iter < MAX_PROC_NUM; iter++) {
        g_proc_list[iter].process_id = -1;
        g_proc_list[iter].process_stack_pointer = 0;
        g_proc_list[iter].process_state = WAITING;
    }

    g_proc_pool.psp_pool_top = &_psp_pool_start;
}

pcb_t* pcb_alloc() {
    // if the number of processes are the maximum,
    if (g_proc_pool.process_counter >= MAX_PROC_NUM) {
        return NULL;  // cannot initialize new process
    } else {
        pcb_t *res = &g_proc_list[g_proc_pool.process_counter];
        res->process_id = g_proc_pool.process_counter;
        res->process_stack_pointer = g_proc_pool.psp_pool_top;
        g_proc_pool.process_counter++;
        g_proc_pool.psp_pool_top -= PSP_SIZE;
        return res;
    }
}

void proc_init(addr_t func) {
    pcb_t *ret = pcb_alloc();
    if (ret == NULL) return;

    ret->process_state = READY;
    uint32_t *psp = (uint32_t*)ret->process_stack_pointer;

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

pcb_t* proc_find_ready() {
    int32_t running = -1;
    for (int32_t iter = 0; iter < MAX_PROC_NUM; iter++) {
        if (g_proc_list[iter].process_state == RUNNING) {
            running = iter;
            break;
        }
    }
    for (int32_t iter = running; iter < MAX_PROC_NUM; iter++) {
        if (g_proc_list[iter].process_state == READY) {
            return &g_proc_list[iter];
        }
    }
    for (int32_t iter = 0; iter < running; iter++) {
        if (g_proc_list[iter].process_state == READY) {
            return &g_proc_list[iter];
        }
    }
    // 모두 실패 시
    return NULL;
}

void idle_task(void) {
    while (1) {
        // uart_print_str("idle\n");
    }
}
