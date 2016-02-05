[bits 64]
[org 0x140059000]

%define IAT_wcsicmp 0x1400286B8

; Is it our command?
lea rdx, [rel cmd_name]
lea rcx, [rsp+0x20]

mov rax, IAT_wcsicmp
call [rax]

test eax, eax
jnz not_ours

; Our command!
mov ax, 0x02
mov ebx, 0xdeadc0de
jmp 0x14000B9C8

not_ours:
  mov rax, 0x140030034
  mov [rax], r13d
  mov ebx, r13d
  jmp 0x14000B948

cmd_name: dw 'H', 'E', 'L', 'L', 'O', 0

