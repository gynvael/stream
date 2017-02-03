[bits 32]


; stack: esp --> [RET] [ARG1]
prolog:
  mov eax, [esp+4]
  push ecx
  push edx
  mov ecx, [eax+0]  ; pc
  mov edx, [eax+4]  ; ptr
  add eax, 8


dd 0xcccccccc
epilog:
  mov eax, [esp+4+8]
  mov [eax+0], ecx
  mov [eax+4], edx
  pop edx
  pop ecx
  ret

dd 0xcccccccc
ptr_inc:
  inc edx
  inc ecx

dd 0xcccccccc
ptr_dec:
  dec edx
  inc ecx





