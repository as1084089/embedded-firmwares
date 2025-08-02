#include "core_cm3.h"
#include "types.h"
#include "uart_print.h"
#include "process.h"
#include "context.h"

// 함수 선언
void Reset_Handler(void);
void Default_Handler(void);
void HardFault_Handler(void);
void SVCall_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void systick_init(__u32 ticks) {
    SysTick->LOAD   = ticks - 1;
    SysTick->VAL    = 0;
    SysTick->CTRL   = SysTick_CTRL_CLKSOURCE_Msk
                    | SysTick_CTRL_TICKINT_Msk
                    | SysTick_CTRL_ENABLE_Msk;
}

void print_sysreg() {
    __u32 __psr = 0;
    __asm__ __volatile__ (
        "MRS R0, PSR" : "=r"(__psr)
    );
    uart_print_hex(__psr);
}

extern int main(void);
// extern __pcb_t __proc_list[];
// extern volatile __pp_t __pp;

// 외부 심볼 (링커 스크립트에서 제공)
extern __u32 _etext;
extern __u32 _sdata;
extern __u32 _edata;
extern __u32 _sbss;
extern __u32 _ebss;
extern __u32 _stack_start;

// 인터럽트 벡터 테이블
__attribute__ ((section(".isr_vector")))
const __u32 VectorTable[] = {
    (__u32)&_stack_start,   // 초기 스택 포인터
    (__u32)Reset_Handler, // 리셋 핸들러
    (__u32)Default_Handler, // NMI
    (__u32)HardFault_Handler, // Hard Fault
    (__u32)Default_Handler, // Memory Management Fault
    (__u32)Default_Handler, // Bus Fault
    (__u32)Default_Handler, // Usage Fault
    0, 0, 0, 0,                // Reserved
    (__u32)SVCall_Handler, // SVCall
    (__u32)Default_Handler, // Debug monitor
    0,                         // Reserved
    (__u32)PendSV_Handler, // PendSV
    (__u32)SysTick_Handler, // SysTick
    // 여기에 추가 인터럽트 핸들러들 작성 가능
};

// 리셋 핸들러
void Reset_Handler(void)
{
    // .data 섹션 복사 (플래시 → RAM)
    __u32 *src = &_etext;
    __u32 *dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    // .bss 섹션 0으로 초기화
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }
    //print_sysreg(); PR_ENDL;

    __init_process_pool();
    __init_process_context(main);

    systick_init(50000);

    // main() 호출
    //main();

    // main에서 리턴하지 않도록 루프
    while (1) {}
}

extern uint32_t __get_PSP(void);

void HardFault_Handler(void) {
    uint32_t* psp = (uint32_t*)__get_PSP();

    uart_print_str("HardFault!\n");
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
    print_sysreg();
    return;
}

void SVCall_Handler(void)
{
    uart_print_str("entered SVCall handler\n"); PR_ENDL;
    print_sysreg();
    //volatile int test = 1234;
    //(void)test;
    return;
}

__attribute__((naked)) void PendSV_Handler(void) {
    void* sp = __proc_list[0].process_stack_pointer;
    __asm__ __volatile__ (
        "msr psp, %[psp]         \n"
        "ldmia %[psp]!, {r4-r11} \n"
        "msr psp, %[psp]         \n"
        "ldr lr, =0xFFFFFFFD     \n"
        "bx lr                   \n"
        :
        : [psp] "r" (sp)
        : "memory"
    );
}

void SysTick_Handler(void) {
    uart_print_str("entered SysTick Handler"); PR_ENDL;
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}