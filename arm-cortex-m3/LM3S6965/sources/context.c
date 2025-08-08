#include "context.h"
#include "uart.h"
#include "core_cm3.h"

/* 2025-08-09 Sat. Dahun Kim
 * Note: This file is not included in the build, and is provided solely for reference and to aid in understanding the assembly code.
 * For the source files that actually contribute to the firmware binary, please refer to context_asm.s.
 */
#ifndef __CONTEXT_ASM__

void __schedule(void) {
    __pcb_t* next = NULL;
    register __pcb_t* tmp __asm__("r1");
    tmp = __current_running;

    // If there is a running task, save its context
    if (tmp != NULL) {
        __save_proc_context();
        __current_running->process_stack_pointer = (addr_t)__get_PSP();
    }

    // Find the next READY process
    next = __search_ready_proc();

    if (next == NULL) {
        if (tmp != NULL) {
            // If there is a running task, keep running it (fallback)
            __restore_proc_context(__current_running);
            while (1); // (Should never reach here)
        } else {
            // If there is nothing to run, reset the system
            SCB->AIRCR = (0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk;
            while (1) { __NOP(); }
        }
    } else {
        if (__current_running)
            __current_running->process_state = READY;
        __current_running = next;
        __current_running->process_state = RUNNING;
        __restore_proc_context(next);
    }
}

#endif // !__CONTEXT_ASM__