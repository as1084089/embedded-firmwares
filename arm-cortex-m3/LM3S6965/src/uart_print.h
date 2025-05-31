#ifndef CD269808_58CD_4AE2_815D_3152075C317B
#define CD269808_58CD_4AE2_815D_3152075C317B

#pragma once
#include "types.h"

#define ASCII_DIGIT_OFFSET '0'
#define ASCII_CCHAR_OFFSET 'A'
#define ASCII_SCHAR_OFFSET 'a'
#define PR_ENDL uart_println()

static volatile unsigned int *UART0_DR = (unsigned int *)0x4000C000;

void uart_print_char(char c);
void uart_print_str(const char *str);
void uart_print_dec(i32 num);
void uart_print_hex(u32 num);
void uart_println();

#endif /* CD269808_58CD_4AE2_815D_3152075C317B */
