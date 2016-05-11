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
  ;mov ax, 0xb800  ; 0xb8000
  ;mov fs, ax
  ;mov bx, 0
  ;mov ax, 0x4141
  ;mov [fs:bx], ax 
  
  ; ds, cs, ss, es
  ; fs, gs

  ; idx-w-GDT, 1 bit GDT/LDT, 2 bit (0, 3)
  ; Global Descriptor Table
  ; Local
  lgdt [GDT_addr]

  mov eax, cr0
  or eax, 1
  mov cr0, eax

  jmp dword 0x8:(0x20000+start32)

start32:
  [bits 32]
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov ss, ax

  lea eax, [0xb8000]
  
  mov dword [eax], 0x41414141

jmp $

GDT_addr:
dw (GDT_end - GDT) - 1
dd 0x20000 + GDT

times (32 - ($ - $$) % 32) db 0xcc

; GLOBAL DESCRIPTOR TABLE 32-BIT
GDT:

; Null segment
dd 0, 0

; Code segment
dd 0xffff  ; segment limit
dd (10 << 8) | (1 << 12) | (1 << 15) | (0xf << 16) | (1 << 22) | (1 << 23)

; Data segment
dd 0xffff  ; segment limit
dd (2 << 8) | (1 << 12) | (1 << 15) | (0xf << 16) | (1 << 22) | (1 << 23)

; Null segment
dd 0, 0

GDT_end:




