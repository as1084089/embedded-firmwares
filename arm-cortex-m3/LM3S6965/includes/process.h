#ifndef E865FA23_0901_4261_B942_65BE45656930
#define E865FA23_0901_4261_B942_65BE45656930
#pragma once

#include "types.h"

#define __PSP_SIZE 4096
#define __MAX_PROC_NUM 8

extern uint32_t _psp_pool_start;

typedef enum    enum_process_state {
    RUNNING = 0,
    READY,
    WAITING
} __ps_t;

typedef struct  process_pool {
    uint32_t    process_counter;
    addr_t      psp_pool_top;
} __pp_t;

typedef struct  process_control_block {
    int32_t     process_id;
    __ps_t      process_state;  // It could be the states such as 'create', 'ready', 'running', 'waiting'
    // u32 reg_pc;         // register `program counter`
    addr_t      process_stack_pointer;        // register `process stack pointer` which points the saved context
    // u32 parent_pid;
} __pcb_t;

extern __pp_t __pp;
extern __pcb_t __proc_list[__MAX_PROC_NUM];
extern __pcb_t *__current_running;

void        __init_process_pool();
__pcb_t*    __init_process_control_block();
void        __init_process_context(addr_t func);
__pcb_t*    __search_ready_proc();
void        __idle_task();

static inline void __start() {
    __current_running = &__proc_list[0];
    __current_running->process_state = RUNNING;

    void* sp = __current_running->process_stack_pointer;
    __asm__ volatile (
        "msr psp, %[psp]\n"
        "movs r0, #2\n"
        "msr control, r0\n"
        "isb\n"
        :: [psp] "r" (sp) : "memory", "r0"
    );

    __idle_task();
}

#endif /* E865FA23_0901_4261_B942_65BE45656930 */
