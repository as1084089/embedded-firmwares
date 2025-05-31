#ifndef CD269808_58CD_4AE2_815D_3152075C317B
#define CD269808_58CD_4AE2_815D_3152075C317B

#pragma once
#include "types.h"

static volatile unsigned int *UART0_DR = (unsigned int *)0x4000C000;

void uart_putc(char c);
void uart_puts(const char *str);
void utoa_fast(u32 value, char *buf);

#endif /* CD269808_58CD_4AE2_815D_3152075C317B */
