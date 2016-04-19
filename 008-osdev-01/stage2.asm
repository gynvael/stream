[bits 16]
[org 0x0000]

start:
  mov ax, 0x2000
  mov ds, ax
  mov es, ax

  mov ax, 0x1f00
  mov ss, ax
  xor sp, sp

  ; remove later
  mov ax, 0xb800  ; 0xb8000
  mov fs, ax
  mov bx, 0
  mov ax, 0x4141

  mov [fs:bx], ax 
  

jmp $

times 1337 db 0x41

