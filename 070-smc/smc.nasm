[bits 64]

; rdi, rsi, ...

%define KEY 0x87

; int3

lea rax, [rel encrypted_start]
lea rdx, [rel encrypted_end]

loop_dec:
  xor byte [rax], KEY
  inc rax
  cmp rax, rdx
  jne loop_dec

encrypted_start:
  mov rax, rsi
  xor rax, rdi
  mov rsi, rax

encrypted_end:

  lea rax, [rel encrypted_start]
  lea rdx, [rel encrypted_end]

loop_enc:
  xor byte [rax], KEY
  inc rax
  cmp rax, rdx
  jne loop_enc

mov rax, rsi
ret


