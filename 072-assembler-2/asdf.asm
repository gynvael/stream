!org 0x1234

   # asdf

dd label1, label2

:label1
db "a"  # asdf
db 1, 2, 3
dw 1234, 0x123

dw label1, label2

:label2
dd 0x12345678  # asdf
dq 0x1625376123786162
dw 01234

dw label1, label2

db 1

nop
nop
int3
nop
