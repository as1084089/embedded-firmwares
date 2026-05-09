#include "context.h"
#include "uart.h"
#include "core_cm3.h"

/* 2025-08-09 Sat. Dahun Kim
 * Note: This file is not included in the build, and is provided solely for reference and to aid in understanding the assembly code.
 * For the source files that actually contribute to the firmware binary, please refer to context_asm.s.
 */
#ifndef CONTEXT_USE_ASM

void kernel_schedule(void) {
    pcb_t* next = NULL;
    register pcb_t* tmp __asm__("r1");
    tmp = g_current_running;

    // If there is a running task, save its context
    if (tmp != NULL) {
        ctx_save();
        g_current_running->process_stack_pointer = (addr_t)__get_PSP();
    }

    // Find the next READY process
    next = proc_find_ready();

    if (next == NULL) {
        if (tmp != NULL) {
            // If there is a running task, keep running it (fallback)
            ctx_restore(g_current_running);
            while (1); // (Should never reach here)
        } else {
            // If there is nothing to run, reset the system
            SCB->AIRCR = (0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk;
            while (1) { __NOP(); }
        }
    } else {
        if (g_current_running)
            g_current_running->process_state = READY;
        g_current_running = next;
        g_current_running->process_state = RUNNING;
        ctx_restore(next);
    }
}

#endif // !CONTEXT_USE_ASM
