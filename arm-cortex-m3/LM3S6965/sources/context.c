#include "context.h"

__attribute__((naked)) void trigger_svcall(void) {
    __asm__ __volatile__ ("SVC #0\n");
}

void __save_proc_context(void) {
    // 
}

void __restore_proc_context(void) {

}