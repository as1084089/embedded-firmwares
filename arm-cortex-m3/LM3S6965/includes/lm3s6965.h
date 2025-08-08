#ifndef __LM3S6965_BSP_H__
#define __LM3S6965_BSP_H__

#include <stdint.h>
// #include "core_cm3.h" // CMSIS Core Header

//------------- Peripheral Base Addresses --------------
#define PERIPH_BASE         ((uint32_t)0x40000000)
#define APB_BASE            PERIPH_BASE
#define AHB_BASE            ((uint32_t)0x50000000)

#define GPIO_PORTA_BASE     (APB_BASE + 0x04000)
#define UART0_BASE          (APB_BASE + 0x0C000)

//------------- Nested Vector Interrupt Controller -----
#define __NVIC_PRIO_BITS    3

//------------- Peripheral Struct Definitions ----------
typedef struct {
    volatile uint32_t DATA;       // +0x000
    volatile uint32_t DIR;        // +0x400
    volatile uint32_t IS;         // +0x404
    volatile uint32_t IBE;        // +0x408
    volatile uint32_t IEV;        // +0x40C
    volatile uint32_t IM;         // +0x410
    volatile uint32_t RIS;        // +0x414
    volatile uint32_t MIS;        // +0x418
    volatile uint32_t ICR;        // +0x41C
    volatile uint32_t AFSEL;      // +0x420
    // ... 기타 생략
} GPIO_TypeDef;

typedef struct {
    volatile uint32_t DR;         // +0x000
    volatile uint32_t RSR_ECR;    // +0x004
    uint32_t RESERVED0[4];
    volatile uint32_t FR;         // +0x018
    // ... 기타 생략
} UART_TypeDef;

//------------- Peripheral Pointers --------------------
#define GPIOA               ((GPIO_TypeDef *) GPIO_PORTA_BASE)
#define UART0               ((UART_TypeDef *) UART0_BASE)

//------------- Clock Control (Mock) -------------------
#define SYSCTL_RCGC2_R      (*((volatile uint32_t *)0x400FE108))
#define SYSCTL_RCGC2_GPIOA  0x00000001

//------------- IRQn Definitions -----------------------
typedef enum IRQn {
    NonMaskableInt_IRQn   = -14,
    HardFault_IRQn        = -13,
    SVCall_IRQn           = -5,
    PendSV_IRQn           = -2,
    SysTick_IRQn          = -1,
    GPIOA_IRQn            = 0,
    UART0_IRQn            = 5,
    // ...
} IRQn_Type;

#endif // __LM3S6965_BSP_H__
