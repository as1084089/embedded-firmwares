#ifndef D33974CD_AF5C_4EFB_A13A_825C08365082
#define D33974CD_AF5C_4EFB_A13A_825C08365082

#include "types.h"
#include "process.h"
#include "uart_print.h"

#pragma once

typedef struct proc_context {
    // Exception Stack Frame
    uint32_t r_PSR;
    uint32_t r_PC;
    uint32_t r_LR;
    uint32_t r_R12;
    uint32_t r_R3;
    uint32_t r_R2;
    uint32_t r_R1;
    uint32_t r_R0;

    // Software Saved
    uint32_t r_R11;
    uint32_t r_R10;
    uint32_t r_R9;
    uint32_t r_R8;
    uint32_t r_R7;
    uint32_t r_R6;
    uint32_t r_R5;
    uint32_t r_R4;
} __context_t;

void trigger_svcall(void);
void __save_proc_context(void);
void __restore_proc_context(__pcb_t *);
void __schedule(void);

#endif /* D33974CD_AF5C_4EFB_A13A_825C08365082 */
