; -------------------------------------------------------------
; entering long mode w/ 2MB paging 
; -------------------------------------------------------------

%include "s2def.i"

[bits 16]
[org 0x0000]

start:

  ; enabling A20 - based on http://wiki.osdev.org/A20_Line
  ; note: A20 activated by Bochs by default
  mov ax, 0x2401
  int 0x15

  ; disable interrupts
  cli

  ; set up segment regs to 0x20000
  mov ax, 0x2000
  mov ds, ax
  mov es, ax

  mov ax, 0x1f00
  mov ss, ax
  xor sp, sp

  ; idx-w-GDT, 1 bit GDT/LDT, 2 bit (0, 3)
  ; Global Descriptor Table
  ; Local
  lgdt [GDT_addr]

  ; enable protected mode
  mov eax, cr0
  or eax, 1
  mov cr0, eax

  ; and jump to 32-bit code
  jmp dword CODE_SEL:(0x20000+start32)

start32:
  [bits 32]
  ; again, set up segment regs
  mov ax, DATA_SEL
  mov ds, ax
  mov es, ax
  mov ss, ax

  ; set CR3 with address of PML4
  mov eax, (PML4 - $$ + 0x20000)
  mov cr3, eax

  ; enable PAE
  mov eax, cr4
  or eax, 1 << 5
  mov cr4, eax

  ; enable IA-32e mode via EFER MSR
  mov ecx, 0xC0000080
  rdmsr
  or eax, 1 << 8
  wrmsr

  ; enable paging
  mov eax, cr0
  or eax, 1 << 31
  mov cr0, eax

  ; set new GDT for 64-bit and jump to 64-bit code
  lgdt [GDT64_addr+0x20000]
  jmp dword CODE_SEL:(0x20000+start64)

start64:
  [bits 64]
  ; and again, set up segment regs
  mov ax, DATA_SEL
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  ; print some stuff!
  mov rax, 0xb8000
  mov rdx, 0x4141414141414141
  mov [rax], rdx

  jmp $  ; stop exec


; -------------------------------------------------------------
;
; [ data structures - GDT, PDTs, etc. ]
; 
; -------------------------------------------------------------

; Protected mode GDT stuffs...
; GDT address structure - for lgdt
GDT_addr:
dw (GDT_end - GDT) - 1
dd 0x20000 + GDT

; alignment to 32 bytes
align2(32,0xcc)

; -------------------------------------------------------------
; GLOBAL DESCRIPTOR TABLE 32-BIT
; -------------------------------------------------------------
GDT: 

; Null segment - GDT[0] == 0x0
dd 0, 0

; Code segment - GDT[1] == 0x8
dd 0xffff  ; segment limit
dd SEG_CODE_XR | SEG_DESTYPE(1) | SEG_PRIV(0) | SEG_PRESENT(1) | (0xf << 16) | SEG_OPSIZE(1) | SEG_GRAN(1)

; Data segment - GDT[2] == 0x10
dd 0xffff  ; segment limit
dd SEG_DATA_RW | SEG_DESTYPE(1) | SEG_PRIV(0) | SEG_PRESENT(1) | (0xf << 16) | SEG_OPSIZE(1) | SEG_GRAN(1)

; Null segment - not needed?
dd 0, 0

GDT_end: 
; -------------------------------------------------------------



; -------------------------------------------------------------
; GDT64 address structure - for lgdt
GDT64_addr:
dw (GDT64_end - GDT64) - 1
dd 0x20000 + GDT64

; alignment to 32 bytes
align2(32,0xcc)

; -------------------------------------------------------------
; GLOBAL DESCRIPTOR TABLE 64-BIT
; -------------------------------------------------------------
GDT64: 

; Null segment - GDT64[0] == 0x0
;dd 0, 0
dq 0

; KERNEL CODE segment - GDT64[1] == 0x8
dd 0xffff  ; segment limit
dd SEG_CODE_XR | SEG_DESTYPE(1) | SEG_PRIV(0) | SEG_PRESENT(1) | (0xf << 16) | SEG_LONG(1) | SEG_GRAN(1)

; KERNEL DATA segment - GDT64[2] == 0x10
dd 0xffff  ; segment limit
dd SEG_DATA_RW | SEG_DESTYPE(1) | SEG_PRIV(0) | SEG_PRESENT(1) | (0xf << 16) | SEG_LONG(1) | SEG_GRAN(1)

GDT64_end: 
; -------------------------------------------------------------


; -------------------------------------------------------------
; MEMORY STRUCTURES FOR PAGING - PML4, PDT, etc.
; -------------------------------------------------------------
; alignment to 4096 bytes
align2(4096,0x0)

; -------------------------------------------------------------
; Level 4 TABLE - Page-Map Level-4 Table
PML4: 
dq 1 | (1 << 1) | (PDPT - $$ + 0x20000)
times 511 dq 0x0

; 1 GB pages - initially tried by Gyn during #OSDev3 stream
; PDPT:
;dq 1 | (1 << 1) | (1 << 7)

; Level 3 TABLE - Page-Directory Pointer Table
PDPT:
dq 1 | (1 << 1) | (PDT - $$ + 0x20000) 
times 511 dq 0x0

; Level 2 TABLE - Page Directory Table
PDT:
; preprocessor loop dance... - mapping first 1GB == 512 * 0x200000 bytes
; 0x83 == 0x1 + 0x2 + 0x80 ---> Present/Writeablei/2MB page
%assign i 0
%rep 512
  dq 0x200000*i+0x83
%assign i i+1
%endrep
; -------------------------------------------------------------

