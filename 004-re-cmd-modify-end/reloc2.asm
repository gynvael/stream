[bits 64]
[org 0x140059080]

cmp eax, 0xdeadc0de
jne not_eq

mov r13, 0x140059100
mov r12, 0x1F
jmp 0x14000B4EF

not_eq:
  mov rax, r12
  shl rax, 5
  jmp 0x14000B4DB

