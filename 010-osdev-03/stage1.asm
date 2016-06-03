[bits 16]
[org 0x7c00]
; 0x7c00
; SEGMENT:OFFSET
; adres = SEGMENT * 0x10 + OFFSET
; CS 07c0  0000
; IP 0000  7c00
;          ****

jmp word 0x0000:start

start:
  ; Load from floppy stage 2.
  ; DL == already set by BIOS
  ; AX -- 16 bits, AH AL -- 8 bits
  ; EAX -- AX
  
  mov ax, 0x2000 ; 0x2000:0x0000
  mov es, ax
  xor bx, bx ; bx == 0

  mov ah, 2  ; read sectors into memory
  mov al, 0xcc  ; 1337 stage2  3 * 512 
  nop
  nop
  mov ch, 0
  mov cl, 2  ; sectors start from 1, or so they say ;)
  mov dh, 0

  int 13h
  
  ; Jump to stage 2
  jmp word 0x2000:0x0000

  ; The CPU doesn't reach here. Ever. Hopefully.

epilogue:
%if ($ - $$) > 510
  %fatal "Bootloader code exceed 512 bytes."
%endif

times 510 - ($ - $$) db 0
db 0x55
db 0xAA




