#include "common.h"
#include <stdint.h>

#include <int_handlers.h>

void* __stack_chk_guard = NULL;

void
__stack_chk_fail()
{
    GuruPanicOfDeath("STACK_FRAME_SMASHED", NULL);
}

void
__chk_fail(void)
{
    GuruPanicOfDeath("STACK_FRAME_SMASHED", NULL);
}
