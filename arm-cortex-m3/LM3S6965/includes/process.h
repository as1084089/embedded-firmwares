#ifndef E865FA23_0901_4261_B942_65BE45656930
#define E865FA23_0901_4261_B942_65BE45656930
#pragma once

#include "types.h"

#define PSP_SIZE       4096
#define MAX_PROC_NUM   8

extern uint32_t _psp_pool_start;

typedef enum    enum_process_state {
    RUNNING = 0,
    READY,
    WAITING
} proc_state_t;

typedef struct  process_pool {
    uint32_t    process_counter;
    addr_t      psp_pool_top;
} proc_pool_t;

typedef struct  process_control_block {
    int32_t       process_id;
    proc_state_t  process_state;  // It could be the states such as 'create', 'ready', 'running', 'waiting'
    // u32 reg_pc;         // register `program counter`
    addr_t        process_stack_pointer;        // register `process stack pointer` which points the saved context
    // u32 parent_pid;
} pcb_t;

extern proc_pool_t g_proc_pool;
extern pcb_t       g_proc_list[MAX_PROC_NUM];
extern pcb_t      *g_current_running;

void   proc_pool_init();
pcb_t* pcb_alloc();
void   proc_init(addr_t func);
pcb_t* proc_find_ready();
void   idle_task();

static inline void kernel_start() {
    g_current_running = &g_proc_list[0];
    g_current_running->process_state = RUNNING;

    void* sp = g_current_running->process_stack_pointer;
    __asm__ __volatile__ (
        "msr psp, %[psp]\n"
        "movs r0, #3\n"
        "msr control, r0\n"
        "isb\n"
        :: [psp] "r" (sp) : "memory", "r0"
    );

    idle_task();
}

#endif /* E865FA23_0901_4261_B942_65BE45656930 */
