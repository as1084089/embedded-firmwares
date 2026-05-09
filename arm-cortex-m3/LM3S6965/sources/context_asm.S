#include "kernel_offsets.h"

.syntax unified
.thumb

.global ctx_save
.type ctx_save, %function
ctx_save:
    cpsid   i                  @ Disable interrupts
    mrs     r0, psp            @ Read current PSP into r0
    stmdb   r0!, {r4-r11}      @ Push r4-r11 onto process stack
    msr     psp, r0            @ Save updated PSP
    bx      lr                 @ Return

.global ctx_restore
.type ctx_restore, %function
ctx_restore:
    ldr     r1, [r0, #PCB_OFFSET_PSP]   @ Load process_stack_pointer from pcb_t*
    msr     psp, r1                     @ Set PSP to this process stack
    ldmia   r1!, {r4-r11}               @ Pop r4-r11 from process stack
    msr     psp, r1                     @ Update PSP after pop
    cpsie   i                           @ Enable interrupts
    ldr     lr, =0xFFFFFFFD             @ Exception return: Thread mode, use PSP
    bx      lr                          @ Return from exception (context switch to process)

.global kernel_schedule
.type   kernel_schedule, %function
kernel_schedule:
    push    {r4, lr}                  @ Save callee-saved r4 and return address

    ldr     r1, =g_current_running    @ r1 = &g_current_running
    ldr     r2, [r1]                  @ r2 = g_current_running (PCB pointer)
    cbz     r2, schedule_search       @ If g_current_running == NULL, skip context save

    bl      ctx_save                  @ Save context (r4-r11 into PSP)
    mrs     r3, psp                   @ r3 = PSP
    ldr     r2, [r1]                  @ r2 = g_current_running
    str     r3, [r2, #PCB_OFFSET_PSP] @ g_current_running->process_stack_pointer = PSP

schedule_search:
    bl      proc_find_ready           @ r0 = next ready process (PCB*)
    ldr     r1, =g_current_running    @ r1 = &g_current_running
    ldr     r2, [r1]                  @ r2 = g_current_running
    cbnz    r0, schedule_found        @ If next process found, jump to switch

    cbz     r2, schedule_reset        @ If g_current_running == NULL, reset system
    mov     r0, r2                    @ Restore context to current process
    bl      ctx_restore               @ (Never returns)
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
    cbz     r2, schedule_running          @ If g_current_running == NULL, skip READY state
    movs    r3, #PROC_STATE_READY         @ READY
    strb    r3, [r2, #PCB_OFFSET_STATE]   @ g_current_running->process_state = READY

schedule_running:
    movs    r2, #PROC_STATE_RUNNING       @ RUNNING
    strb    r2, [r0, #PCB_OFFSET_STATE]   @ next->process_state = RUNNING
    str     r0, [r1]                      @ g_current_running = next

    pop     {r4, lr}                  @ Restore r4 and lr
    b       ctx_restore               @ Switch context to next (never returns)
