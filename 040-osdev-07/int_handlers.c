#include "common.h"
#include "terminal.h"
#include "terminal_backend_b8000.h"

void Int_DE(void) {
  TerminalBackend *con = TerminalBackendB8000();
  T_PutText(con, "DIVISION BY ZERO ;f");

  for (;;);  
}

