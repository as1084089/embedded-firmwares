ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 256K
  SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS
{
  .text : 
  {
    KEEP(*(.isr_vector))    /* 인터럽트 벡터 테이블 */
    *(.text*)               /* 코드 */
    *(.rodata*)             /* 읽기 전용 데이터 */
    _etext = .;
  } > FLASH

  .data : AT (ADDR(.text) + SIZEOF(.text))
  {
    _sdata = .;
    *(.data*)               /* 초기화된 전역 변수 */
    _edata = .;
  } > SRAM

  .bss :
  {
    _sbss = .;
    *(.bss*)                /* 초기화되지 않은 전역 변수 */
    *(COMMON)
    _ebss = .;
  } > SRAM

  .stack :
  {
    . = ALIGN(0x8);
    _stack_end = .;         /* stack의 끝점, 낮은 주소 */
    . += 0x1000;
    _stack_start = .;       /* stack의 시작점, 높은 주소 */
  } > SRAM

  .psp_pool :
  {
    . = ALIGN(0x1000);
    _psp_pool_end = .;
    . += 0x8000;            /* 32KB size for the process stack pool */
    _psp_pool_start = .;
  } > SRAM
}
