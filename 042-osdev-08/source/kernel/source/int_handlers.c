#include "common.h"
#include "terminal.h"
#include "terminal_backend_b8000.h"
#include "hal.h"
#include <int_handlers.h>

void GuruPanicOfDeath(const char *reason, TrapFrame *frame) {
  TerminalBackend *con = TerminalBackendB8000();
  T_ClearScreen(con);
  T_PutText(con, "Guru Panic Of Death!!!\n\n");
  T_Printf(con, "Reason: %s\n\n", reason);

  if (frame != NULL) {
    T_PutText(con, "State:\n\n");

    T_Printf(con, "    RAX: %016llx\tR8:  %016llx\n", frame->rax, frame->r8);
    T_Printf(con, "    RBX: %016llx\tR9:  %016llx\n", frame->rbx, frame->r9);
    T_Printf(con, "    RCX: %016llx\tR10: %016llx\n", frame->rcx, frame->r10);
    T_Printf(con, "    RDX: %016llx\tR11: %016llx\n", frame->rdx, frame->r11);
    T_Printf(con, "    RSI: %016llx\tR12: %016llx\n", frame->rsi, frame->r12);
    T_Printf(con, "    RDI: %016llx\tR13: %016llx\n", frame->rdi, frame->r13);
    T_Printf(con, "    RBP: %016llx\tR14: %016llx\n", frame->rbp, frame->r14);  
    T_Printf(con, "    RSP: %016llx\tR15: %016llx\n", frame->rsp, frame->r15);      

    T_Printf(con, " CS:RIP: %04x:%016llx\n\n", frame->segment_cs, frame->rip);
  }

  T_PutText(con, "System halt.");

  HAL_PauseKernel();
}


void Int_DE(TrapFrame *frame) {
  GuruPanicOfDeath("DIVISION_ERROR", frame);
}

