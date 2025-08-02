#ifndef E865FA23_0901_4261_B942_65BE45656930
#define E865FA23_0901_4261_B942_65BE45656930
#pragma once

#include "types.h"

#define __PSP_SIZE 4096
#define __MAX_PROC_NUM 8

extern uint32_t _psp_pool_start;

typedef struct process_pool {
    uint32_t   process_counter;
    addr_t  psp_pool_top;
} __pp_t;

typedef struct process_control_block {
    uint32_t   process_id;
    //u32 process_state;  // It could be the states such as 'create', 'ready', 'running', 'waiting', 'terminated'
    // u32 reg_pc;         // register `program counter`
    addr_t  process_stack_pointer;        // register `process stack pointer` which points the saved context
    // u32 parent_pid;
} __pcb_t;

extern volatile __pp_t __pp;
extern __pcb_t __proc_list[__MAX_PROC_NUM];

void __init_process_pool();
int32_t __init_process_control_block();
void __init_process_context(addr_t func);

#endif /* E865FA23_0901_4261_B942_65BE45656930 */
