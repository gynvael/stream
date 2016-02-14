[bits 64]
[org 0x140059100]

%define LastRetCode 0x1400309A0
%define IAT_swscanf 0x1400285D0
%define IAT_printf 0x140028790

sub rsp, 0x100

; Czy sa parametry?
mov rcx, [rcx+0x78]
test rcx, rcx
jz missing

; String --> Uint
lea rdx, [rel fmt]
mov r8, rsp
mov rax, IAT_swscanf
call [rax]

cmp rax, 1
jne error_in_params

; Silnia
mov r8, [rsp]
xor rax, rax
inc rax
mov rcx, rax

factorial:
  mul rcx
  inc rcx

  cmp rcx, r8
  jg end_of_factorial
  jmp factorial

end_of_factorial:

mov rdx, rax
lea rcx, [rel fmt_print]
mov rax, IAT_printf
call [rax]

; LastRetCode = 0
mov rax, LastRetCode
mov dword [rax], 0
jmp end

error_in_params:
  mov rax, LastRetCode
  mov dword [rax], 2
  jmp end

missing:
  mov rax, LastRetCode
  mov dword [rax], 1

end:
  add rsp, 0x100
  ret

fmt: dw '%', 'u', 0
fmt_print: db '%llu', 0xa, 0


