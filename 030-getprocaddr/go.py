import subprocess
import types
from struct import *
import shutil

def merge():
  out = []
  for fn in [
      "vir",
      "vir_worker.bin"
      ]:
    out.append(open(fn, "rb").read())

  data = ''.join(out)
  idx = data.find(pack("I", 0xc0d31235))
  data = bytearray(data)
  data[idx:idx+4] = bytearray(pack("I", len(data)))  
  
  open("vir.out", "wb").write(str(data))

def copyexe():
  shutil.copyfile("hello.exe", "tests\\hello.exe")


for cmd in [
  "gcc vir_worker.c -nostdlib -c -fPIE -fpic",
  "objcopy -O binary --only-section=.text vir_worker.o vir_worker.bin",
  #"ndisasm -b32 vir_worker.bin",
  "nasm vir.nasm",
  merge,
  copyexe,  
  "asmloader vir.out",
  ]:
  if type(cmd) is str:
    subprocess.check_call(cmd.split(" "))
  elif type(cmd) is types.FunctionType:
    cmd()






