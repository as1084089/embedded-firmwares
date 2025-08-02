#ifndef D33974CD_AF5C_4EFB_A13A_825C08365082
#define D33974CD_AF5C_4EFB_A13A_825C08365082

#include "types.h"

#pragma once

#define __SVCALL svcall()

typedef struct proc_context {

} __context_t;

void trigger_svcall(void);
void __save_proc_context(void);
void __restore_proc_context(void);

#endif /* D33974CD_AF5C_4EFB_A13A_825C08365082 */
