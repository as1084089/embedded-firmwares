.syntax unified
.thumb

.global __save_proc_context
.type __save_proc_context, %function
__save_proc_context:
    cpsid   i                  @ Disable interrupts
    mrs     r0, psp            @ Read current PSP into r0
    stmdb   r0!, {r4-r11}      @ Push r4-r11 onto process stack
    msr     psp, r0            @ Save updated PSP
    bx      lr                 @ Return

.global __restore_proc_context
.type __restore_proc_context, %function
__restore_proc_context:
    ldr     r1, [r0, #8]       @ Load process_stack_pointer from __pcb_t* (offset 8)
    msr     psp, r1            @ Set PSP to this process stack
    ldmia   r1!, {r4-r11}      @ Pop r4-r11 from process stack
    msr     psp, r1            @ Update PSP after pop
    cpsie   i                  @ Enable interrupts
    ldr     lr, =0xFFFFFFFD    @ Exception return: Thread mode, use PSP
    bx      lr                 @ Return from exception (context switch to process)

.global __schedule
.type   __schedule, %function
__schedule:
    push    {r4, lr}                  @ Save callee-saved r4 and return address

    ldr     r1, =__current_running    @ r1 = &__current_running
    ldr     r2, [r1]                  @ r2 = __current_running (PCB pointer)
    cbz     r2, schedule_search       @ If __current_running == NULL, skip context save

    bl      __save_proc_context       @ Save context (r4-r11 into PSP)
    mrs     r3, psp                   @ r3 = PSP
    ldr     r2, [r1]                  @ r2 = __current_running
    str     r3, [r2, #8]              @ __current_running->process_stack_pointer = PSP

schedule_search:
    bl      __search_ready_proc       @ r0 = next ready process (PCB*)
    ldr     r1, =__current_running    @ r1 = &__current_running
    ldr     r2, [r1]                  @ r2 = __current_running
    cbnz    r0, schedule_found        @ If next process found, jump to switch

    cbz     r2, schedule_reset        @ If __current_running == NULL, reset system
    mov     r0, r2                    @ Restore context to current process
    bl      __restore_proc_context    @ (Never returns)
schedule_loop:
    b       schedule_loop             @ Infinite loop (should never return)

schedule_reset:
    ldr     r3, =0xE000ED0C           @ SCB->AIRCR address
    ldr     r2, =0x05FA0004           @ SYSRESETREQ (system reset request value)
    str     r2, [r3]                  @ Trigger system reset
schedule_nop:
    nop                               @ Wait for reset
    b       schedule_nop

schedule_found:
    cbz     r2, schedule_running      @ If __current_running == NULL, skip READY state
    movs    r3, #1                    @ 1 = READY
    strb    r3, [r2, #4]              @ __current_running->process_state = READY

schedule_running:
    movs    r2, #0                    @ 0 = RUNNING
    strb    r2, [r0, #4]              @ next->process_state = RUNNING
    str     r0, [r1]                  @ __current_running = next

    pop     {r4, lr}                  @ Restore r4 and lr
    b       __restore_proc_context    @ Switch context to next (never returns)
