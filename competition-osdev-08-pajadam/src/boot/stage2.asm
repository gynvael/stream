[bits 16]
[org 0x0000]

start:

    ; SWITCH GRAPHIC MODE - Int 10 / AH=00h
    ; 320X200
    mov ax, 0x0013
    int 0x10

  ; enabling A20 - based on http://wiki.osdev.org/A20_Line
  ; note: A20 activated by Bochs by default
  mov ax, 0x2401
  int 0x15

  ; http://wiki.osdev.org/A20_Line
  ; Use BIOS to turn on A20
  ; mov ax, 0x2401
  ; int 0x15
  ; Use fast A20 gate
  in al, 0x92
  or al, 2
  out 0x92, al

  ; Clear (disable) interrupts.

  cli

  mov ax, 0x2000
  mov ds, ax
  mov es, ax

  mov ax, 0x1f00
  mov ss, ax
  xor sp, sp

  ; A20  OSDev #4
  ; 0-19  0-23
  ; ffff:ffff --> ffff0 + ffff
  ; 1024 * 1024 do (1024 * 1024 * 2 - 1)

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
  [bits 32]32
  mov ax, 0x10   ; GDT_idx kolejne_3_bity
  mov ds, ax
  mov es, ax
  mov ss, ax

  ;lea eax, [0xb8000]  ; mov eax, 0xb8000

  ;mov dword [eax], 0x41414141

; Vol 3, 4.5 IA-32e Paging
; http://os.phil-opp.com/entering-longmode.html (reenz0h)
; http://wiki.osdev.org/Setting_Up_Long_Mode
; Vol 3. 3.4.5 Segment Descriptors

  mov eax, (PML4 - $$) + 0x20000
  mov cr3, eax

  mov eax, cr4
  or eax, 1 << 5
  mov cr4, eax

  mov ecx, 0xC0000080 ; EFER
  rdmsr
  or eax, 1 << 8
  wrmsr

  mov eax, cr0
  or eax, 1 << 31
  mov cr0, eax

  lgdt [GDT64_addr + 0x20000]
  jmp dword 0x8:(0x20000+start64)

start64:
  [bits 64]
  mov ax, 0x10   ; GDT_idx kolejne_3_bity
  mov ds, ax
  mov es, ax
  mov ss, ax

  ; Loader ELF
loader:
  mov rsi, [0x20000 + kernel + 0x20]
  add rsi, 0x20000 + kernel

  movzx ecx, word [0x20000 + kernel + 0x38]

  cld

  ; Assumes that the linker always stores ELF header at
  ; first p_vaddr.
  xor r14, r14 ; First PT_LOAD p_vaddr.

  .ph_loop:
  mov eax, [rsi + 0]
  cmp eax, 1  ; If it's not PT_LOAD, ignore.
  jne .next

  mov r8, [rsi + 8] ; p_offset
  mov r9, [rsi + 0x10] ; p_vaddr
  mov r10, [rsi + 0x20] ; p_filesz
  mov r11, [rsi + 0x28] ; p_memsz

  test r14, r14
  jnz .skip
  mov r14, r9
  .skip:

  ; Backup
  mov rbp, rsi
  mov r15, rcx

  ; Zero memory
  mov rdi, r9
  mov rcx, r11
  xor al, al
  rep stosb

  ; Copy segment
  lea rsi, [0x20000 + kernel + r8d]
  mov rdi, r9
  mov rcx, r10
  rep movsb

  ; Restore
  mov rcx, r15
  mov rsi, rbp
  .next:
  add rsi, 0x38
  loop .ph_loop

  ; Fix stack
  mov rsp, 0x30f000

  ; Jump to EP
  mov rdi, r14
  mov rax, [0x20000 + kernel + 0x18]
  call rax


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


; 64-bit GDT here!

GDT64_addr:
dw (GDT64_end - GDT64) - 1
dd 0x20000 + GDT64

times (32 - ($ - $$) % 32) db 0xcc

; GLOBAL DESCRIPTOR TABLE 64-BIT
GDT64:

; Null segment
dd 0, 0

; Code segment
dd 0xffff  ; segment limit
dd (10 << 8) | (1 << 12) | (1 << 15) | (0xf << 16) | (1 << 21) | (1 << 23)

; Data segment
dd 0xffff  ; segment limit
dd (2 << 8) | (1 << 12) | (1 << 15) | (0xf << 16) | (1 << 21) | (1 << 23)

; Null segment
dd 0, 0

GDT64_end:

times (4096 - ($ - $$) % 4096) db 0


; code by reenz0h
; https://github.com/reenz0h/osdev-by-gyn/tree/master/osdev3-addons

;PML4:
;dq 1 | (1 << 1) | (PDPTE - $$ + 0x20000)
;times 511 dq 0

; Assume: aligned to 4 KB

;PDPTE:
;dq 1 | (1 << 1) | (1 << 7)
;times 511 dq 0

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




times (512 - ($ - $$) % 512) db 0
kernel:
