#!/usr/bin/python
import os
import subprocess
from glob import glob

def fix_stage1_size():
  stage2_size = os.stat("bootloader/stage2").st_size
  kernel_size = os.stat("kernel/kernel64.elf").st_size

  stage2_size = (stage2_size + kernel_size + 511) / 512

  if stage2_size >= 255:
    raise Exception("stage2 & kernel are too large")
  
  with open("bootloader/stage1", "rb+") as f:
    d = f.read()
    idx = d.index("\xb0\xcc\x90\x90")
    d = bytearray(d)
    d[idx+1] = stage2_size
    f.seek(0)
    f.write(d)

fix_stage1_size()

files_to_img = [
    "bootloader/stage1",
    "bootloader/stage2",
    "kernel/kernel64.elf"
]

buf = []
for fn in files_to_img:
  with open(fn, "rb") as f:
    d = f.read()
    print("length: " + str(len(d)))
    buf.append(d)

    if len(d) % 512 == 0:
      continue

    padding_size = 512 - len(d) % 512        
    buf.append("\0" * padding_size);

with open("floppy.bin", "wb") as f:
  f.write(''.join(buf))

