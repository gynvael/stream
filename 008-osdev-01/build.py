import os

cmds_to_run = [
    "nasm stage1.asm",
    "nasm stage2.asm"
]

files_to_img = [
    "stage1",
    "stage2"
]

for cmd in cmds_to_run:
  os.system(cmd)

buf = []
for fn in files_to_img:
  with open(fn, "rb") as f:
    buf.append(f.read())
# XXX: nie wyrownuje do wielkosci sektora

with open("floppy.bin", "wb") as f:
  f.write(''.join(buf)) 





