#ifndef CD269808_58CD_4AE2_815D_3152075C317B
#define CD269808_58CD_4AE2_815D_3152075C317B

#pragma once
#include "types.h"
#include "lm3s6965.h"

#define ASCII_DIGIT_OFFSET '0'
#define ASCII_CCHAR_OFFSET 'A'
#define ASCII_SCHAR_OFFSET 'a'
#define PR_ENDL uart_println()

void uart_print_char(char c);
void uart_print_str(const char *str);
void uart_print_dec(int32_t num);
void uart_print_hex(uint32_t num);
void uart_println();

#endif /* CD269808_58CD_4AE2_815D_3152075C317B */
