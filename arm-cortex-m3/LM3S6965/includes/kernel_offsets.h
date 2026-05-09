#ifndef KERNEL_OFFSETS_H
#define KERNEL_OFFSETS_H

/* SSOT for struct field offsets and enum values shared between C and assembly.
 * C-side: verified at compile time via _Static_assert in process.c.
 * Asm-side: included from context_asm.S (preprocessed assembly).
 */

#define PCB_OFFSET_STATE     4
#define PCB_OFFSET_PSP       8

#define PROC_STATE_RUNNING   0
#define PROC_STATE_READY     1
#define PROC_STATE_WAITING   2

#endif /* KERNEL_OFFSETS_H */
