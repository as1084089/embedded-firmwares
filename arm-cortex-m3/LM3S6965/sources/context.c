#include "context.h"
#include "core_cm3.h"

__attribute__((naked)) void trigger_svcall(void) {
    __asm__ __volatile__ ("SVC #0\n");
}

__attribute__((naked)) void __save_proc_context(void) {
    __asm__ __volatile__ (
        "cpsid  i                   \n"
        "mrs    r0,     psp         \n"
        "stmdb  r0!,    {r4-r11}    \n"
        "msr    psp,    r0          \n"
        "bx     lr                  \n"
    );
}

__attribute__((naked)) void __restore_proc_context(__pcb_t *task) {
    addr_t sp = task->process_stack_pointer;
    __asm__ __volatile__ (
        "msr        psp,        %[psp]  \n"
        "ldmia      %[psp]!,    {r4-r11}\n"
        "msr        psp,        %[psp]  \n"
        "cpsie      i                   \n"
        "ldr        lr,     =0xFFFFFFFD \n"
        "bx         lr                  \n"
        :
        : [psp] "r" (sp)
        : "memory"
    );
}

void __schedule(void) {
    __save_proc_context();
    __current_running->process_stack_pointer = (addr_t)__get_PSP();

    __pcb_t *next = __search_ready_proc();
    if (next == NULL) {
        __restore_proc_context(__current_running);

        while(1);
    }
    __current_running->process_state = READY;
    __current_running = next;
    __current_running->process_state = RUNNING;
    __restore_proc_context(next);
}