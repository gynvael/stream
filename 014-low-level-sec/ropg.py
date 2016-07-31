import distorm3 # https://code.google.com/p/distorm/downloads/list

# XXX Setup here XXX
TARGET_FILE = "bo"          
FILE_OFFSET_START = 0x360 # In-file offset of scan start
FILE_OFFSET_END = 0xbf460 # In-file offset of scan start
VA = 0x400000 # Note: PC is calculated like this: VA + given FILE_OFFSET
X86_MODE = distorm3.Decode64Bits # just switch the 32 or 64
# XXX End of setup XXX

UNIQ = {}
def DecodeAsm(pc, d):
  global X86_MODE

  disasm = distorm3.Decode(pc, d, X86_MODE)

  k = []
  l = ""
  ist = ""

  for d in disasm:
    #print d
    addr = d[0]
    size = d[1]
    inst = d[2].lower()
    t = "0x%x    %s" % (addr,inst)
    l += t + "\n"
    ist += "%s\n" % (inst)
    k.append((addr,inst))
    if inst.find('ret') != -1:
      break

  return (l,k,ist)

d = open(TARGET_FILE, "rb").read()

for i in xrange(FILE_OFFSET_START,FILE_OFFSET_END):
  (cc,kk,ist) = DecodeAsm(VA+i, d[i:i+10])
  if cc.find('ret') == -1:
    continue

  if cc.find('db ') != -1:
    continue

  if ist in UNIQ:
    continue

  UNIQ[ist] = True  

  print "------> offset: 0x%x" % (i + VA)
  for k in kk:
    print "0x%x    %s" % (k[0],k[1])
    if k[1].find('ret') != -1:
      break

  print ""



