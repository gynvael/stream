.file "int_wrappers.s"
.intel_syntax noprefix


# by Karol Grzybowski
# https://pastebin.com/r79HA1qT

.global Int_Wrapper_DE
.type Int_Wrapper_DE, @function
.text
Int_Wrapper_DE:
  push rsp
  push rax
  push rbx
  push rcx
  push rdx
  push rsi
  push rdi
  push rbp
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15

  mov rdi, rsp

  call Int_DE@PLT

  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rbp
  pop rdi
  pop rsi
  pop rdx
  pop rcx
  pop rbx
  pop rax
  add rsp, 8

  iretq

