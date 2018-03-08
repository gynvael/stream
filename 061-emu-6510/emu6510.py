#!/usr/bin/python
import sys

def vm_LDY_imm(self, op, arg):
  self.y = arg
  self.pc += 2

def vm_LDX_imm(self, op, arg):
  self.x = arg
  self.pc += 2

def vm_LDA_imm(self, op, arg):
  self.a = arg
  self.pc += 2

def vm_LDA_zp(self, op, arg):
  self.a = self.ram.read_byte(arg)
  self.pc += 2

def vm_LDX_zp(self, op, arg):
  self.x = self.ram.read_byte(arg)
  self.pc += 2

def vm_LDY_zp(self, op, arg):
  self.y = self.ram.read_byte(arg)
  self.pc += 2

def vm_STA_zp(self, op, arg):
  self.ram.write_byte(arg, self.a)
  self.pc += 2

def vm_STX_zp(self, op, arg):
  self.ram.write_byte(arg, self.x)
  self.pc += 2

def vm_STY_zp(self, op, arg):
  self.ram.write_byte(arg, self.y)
  self.pc += 2

def vm_LDA_zpx(self, op, arg):
  self.a = self.ram.read_byte((arg + self.x) & 0xff)
  self.pc += 2

def vm_LDY_zpx(self, op, arg):
  self.y = self.ram.read_byte((arg + self.x) & 0xff)
  self.pc += 2

def vm_LDA_abs(self, op, arg):
  arg2 = self.ram.read_byte(self.pc + 2)
  addr = arg | (arg2 << 8)
  self.a = self.ram.read_byte(addr)
  self.pc += 3

def vm_LDX_abs(self, op, arg):
  arg2 = self.ram.read_byte(self.pc + 2)
  addr = arg | (arg2 << 8)
  self.x = self.ram.read_byte(addr)
  self.pc += 3

def vm_LDY_abs(self, op, arg):
  arg2 = self.ram.read_byte(self.pc + 2)
  addr = arg | (arg2 << 8)
  self.y = self.ram.read_byte(addr)
  self.pc += 3



class RAM:
  def __init__(self):
    self.ram = bytearray(0x10000)

  def write_byte(self, addr, b):
    self.ram[addr & 0xffff] = b

  def read_byte(self, addr):
    return self.ram[addr & 0xffff]


class VM:
  def __init__(self):
    self.a = 0
    self.x = 0
    self.y = 0
    self.sp = 0x0100
    self.pc = 0x0000
    self.ram = RAM()

  def load_to_ram(self, addr, data):
    data = bytearray(data)
    sz = len(data)
    self.ram.ram[addr:addr+sz] = data

  def print_regs(self):
    print "A: %.2x" % self.a
    print "X: %.2x" % self.x
    print "Y: %.2x" % self.y
    print "SP: %.4x" % self.sp
    print "PC: %.4x" % self.pc

  def execute_one(self):
    self.pc &= 0xffff

    self.print_regs()
    op = self.ram.read_byte(self.pc)
    arg = self.ram.read_byte(self.pc + 1)

    res = {
      0xA9: vm_LDA_imm,
      0xA2: vm_LDX_imm,
      0xA0: vm_LDY_imm,

      0xA5: vm_LDA_zp,
      0xA6: vm_LDX_zp,
      0xA4: vm_LDY_zp,

      0x85: vm_STA_zp,
      0x86: vm_STX_zp,
      0x84: vm_STY_zp,

      0xB5: vm_LDA_zpx,
      0xB4: vm_LDY_zpx,

      0xAD: vm_LDA_abs,
      0xAE: vm_LDX_abs,
      0xAC: vm_LDY_abs,


    }[op](self, op, arg)

    if res is None:
      res = True

    return res

  def run(self, pc):
    self.pc = pc
    while self.execute_one():
      pass

    print "-------------------- DONE"
    self.print_regs()


def main():
  if len(sys.argv) == 1:
    sys.exit("usage: emu6510.py <filename>")

  vm = VM()

  with open(sys.argv[1], "rb") as f:
    vm.load_to_ram(0xc000, f.read())

  vm.run(0xc000)

if __name__ == "__main__":
  main()

