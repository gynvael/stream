import subprocess

print subprocess.check_output(
    ["nasm", "smc.nasm"]
)

print subprocess.check_output(
    ["nasm", "-f", "elf64", "smc.nasm"]
)

o = subprocess.check_output(
    ["objdump", "-t", "smc.o"]
)

o = o.splitlines()
def getaddr(o, needle):
  scan = False
  for ln in o:
    if ln.startswith("SYMBOL TABLE:"):
      scan = True
      continue

    if not scan:
      continue

    ln = ln.strip()
    if not ln:
      break

    ln = ln.split(" ")
    addr = ln[0]
    symb = ln[-1]

    if needle == symb:
      return int(addr, 16)

  raise Exception("not found: %s" % needle)

with open("smc", "rb") as f:
  d = bytearray(f.read())

enc_start = getaddr(o, "encrypted_start")
enc_end = getaddr(o, "encrypted_end")

print "%x %x" % (enc_start, enc_end)

KEY = 0x87
for i in xrange(enc_start, enc_end):
  d[i] ^= KEY

with open("smc.h", "w") as f:
  print >>f, ', '.join([
      str(b) for b in d
  ])

print subprocess.check_output(
    ["g++", "-Wall", "-Wextra", "asdf.cc"]
)


