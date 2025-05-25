#include <stdint.h>

// 함수 선언
void Reset_Handler(void);
void Default_Handler(void);
int main(void);

// 외부 심볼 (링커 스크립트에서 제공)
extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _stack_top;

// 인터럽트 벡터 테이블
__attribute__ ((section(".isr_vector")))
const uint32_t VectorTable[] = {
    (uint32_t)&_stack_top,   // 초기 스택 포인터
    (uint32_t)Reset_Handler, // 리셋 핸들러
    (uint32_t)Default_Handler, // NMI
    (uint32_t)Default_Handler, // Hard Fault
    (uint32_t)Default_Handler, // Memory Management Fault
    (uint32_t)Default_Handler, // Bus Fault
    (uint32_t)Default_Handler, // Usage Fault
    0, 0, 0, 0,                // Reserved
    (uint32_t)Default_Handler, // SVCall
    (uint32_t)Default_Handler, // Debug monitor
    0,                         // Reserved
    (uint32_t)Default_Handler, // PendSV
    (uint32_t)Default_Handler, // SysTick
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

    // main() 호출
    main();

    // main에서 리턴하지 않도록 루프
    while (1) {}
}

// 디폴트 핸들러 (사용자 정의되지 않은 인터럽트 진입 시)
void Default_Handler(void)
{
    while (1) {}
}
