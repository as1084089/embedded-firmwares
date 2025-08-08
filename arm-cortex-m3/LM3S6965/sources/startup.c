#include "core_cm3.h"
#include "types.h"
#include "uart.h"
#include "process.h"
#include "context.h"

// 함수 선언
void Reset_Handler(void);
void Default_Handler(void);
void HardFault_Handler(void);
void SVCall_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void __systick_init(uint32_t ticks) {
    SysTick->LOAD   = ticks - 1;
    SysTick->VAL    = 0;
    SysTick->CTRL   = SysTick_CTRL_CLKSOURCE_Msk
                    | SysTick_CTRL_TICKINT_Msk
                    | SysTick_CTRL_ENABLE_Msk;
}

const char* ps_state_str(__ps_t state) {
    switch (state) {
        case 0: return "READY   ";
        case 1: return "RUNNING ";
        case 2: return "WAITING ";
        default: return "UNKNOWN ";
    }
}

void print_proc_list(void) {
    uart_println();
    uart_print_str(" ID |  STATE   |    PSP      | CURRENT "); PR_ENDL;
    uart_print_str("------------------------------------------"); PR_ENDL;
    for (int i = 0; i < __MAX_PROC_NUM; i++) {
        uart_print_str(" ");
        uart_print_dec(__proc_list[i].process_id);
        uart_print_str("  | ");

        uart_print_str(ps_state_str(__proc_list[i].process_state));
        uart_print_str("| ");
        uart_print_hex((uint32_t)__proc_list[i].process_stack_pointer);
        uart_print_str(" | ");

        if (&__proc_list[i] == __current_running) {
            uart_print_str("<- current");
        } PR_ENDL;
    }
    uart_print_str("------------------------------------------"); PR_ENDL;
    uart_println();
}

extern int hello(void);
extern void terminal(void);
extern uint32_t __get_PSP(void);

// 외부 심볼 (링커 스크립트에서 제공)
extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _stack_start;

// 인터럽트 벡터 테이블
__attribute__ ((section(".isr_vector")))
const uint32_t VectorTable[] = {
    (uint32_t)&_stack_start,   // 초기 스택 포인터
    (uint32_t)Reset_Handler, // 리셋 핸들러
    (uint32_t)Default_Handler, // NMI
    (uint32_t)HardFault_Handler, // Hard Fault
    (uint32_t)Default_Handler, // Memory Management Fault
    (uint32_t)Default_Handler, // Bus Fault
    (uint32_t)Default_Handler, // Usage Fault
    0, 0, 0, 0,                // Reserved
    (uint32_t)SVCall_Handler, // SVCall
    (uint32_t)Default_Handler, // Debug monitor
    0,                         // Reserved
    (uint32_t)PendSV_Handler, // PendSV
    (uint32_t)SysTick_Handler, // SysTick
    // 여기에 추가 인터럽트 핸들러들 작성 가능
};

// 리셋 핸들러
void Reset_Handler(void)
{
    // .data 섹션 복사 (플래시 → RAM)
    uint32_t *src = &_etext;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    // .bss 섹션 0으로 초기화
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    __init_psp_pool();
    __init_process_context(terminal);

    __systick_init(50000);

    __start();
    while (1);
}

void HardFault_Handler(void) {
    uint32_t* psp = (uint32_t*)__current_running->process_stack_pointer;

    uart_print_str("HardFault!\n");

    print_proc_list();

    uart_print_hex((uint32_t)psp); PR_ENDL;

    for (int i = 0; i < 16; i++) {
        uart_print_str("psp["); uart_print_hex(i); uart_print_str("] = ");
        uart_print_hex(psp[i]); PR_ENDL;
    }

    while (1);
}

// 디폴트 핸들러 (사용자 정의되지 않은 인터럽트 진입 시)
void Default_Handler(void)
{
    uart_print_str("entered default handler\n"); PR_ENDL;
    return;
}

void SVCall_Handler(void)
{
    uart_print_str("entered SVCall handler\n"); PR_ENDL;
    return;
}

__attribute__((naked)) void PendSV_Handler(void) {
    __schedule();
}

void SysTick_Handler(void) {
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
