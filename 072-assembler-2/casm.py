#!/usr/bin/python
import sys
import os
import string
import struct
from enum import Enum

class TokenTypes(Enum):
  LABEL_INDICATOR = 1  # : in label declaration.
  LITERAL_INT = 2
  LITERAL_STR = 4
  IDENTIFIER = 5  # Labels, etc.

class OutputElement(Enum):
  LABEL = 1

class AsmException(Exception):
  pass

class StreamException(Exception):
  pass

class Stream():
  def __init__(self, data, ln_no):
    self.data = data
    self.i = 0
    self.stack = []
    self.ln_no = ln_no

  def push(self):
    self.stack.append(self.i)

  def pop(self):
    self.i = self.stack.pop()

  def dis(self):
    self.stack.pop()

  def reset(self):
    self.i = 0

  def peek(self, size=1, quiet=False):
    if not quiet and self.i + size - 1 >= len(self.data):
      raise StreamException("out of chars")

    return self.data[self.i:self.i + size]

  def get(self):
    ch = self.peek()
    self.i += 1
    return ch

  def unget(self):
    if self.i == 0:
      raise StreamException("tried to unget on first char")

    self.i -= 1

  def empty(self):
    return self.i == len(self.data)

def s_anything(data):
  return True

def s_ws(s):
  while not s.empty() and s.peek() in " \t":
    s.get()

  return True

def s_cname(s, t):
  CNAME0 = string.ascii_letters + "_.!:"
  CNAME1 = string.ascii_letters + string.digits + "_."

  token = ""
  if s.peek() not in CNAME0:
    return False

  token = s.get()

  try:
    while s.peek() in CNAME1:
      token += s.get()
  except StreamException:
    pass

  t.append((TokenTypes.IDENTIFIER, token))  # TODO: maybe add more info on type

  return True

def s_comma(s):
  if s.peek() == ',':
    s.get()
    return True

  return False

def s_qstring(s, t):
  if s.peek() != '"':
    return False

  s.get()

  string = ""
  while s.peek() != '"':
    string += s.get()

  s.get()

  t.append((TokenTypes.LITERAL_STR, string))
  return True

def s_declit(s, t):
  lit = ""
  while not s.empty() and s.peek() in string.digits:
    lit += s.get()

  if not lit:
    return False

  t.append((TokenTypes.LITERAL_INT, int(lit)))
  return True

def s_hexlit(s, t):
  if s.peek(2, True) != '0x':
    return False

  s.get()
  s.get()

  lit = ""
  while not s.empty() and s.peek() in string.hexdigits:
    lit += s.get()

  if not lit:
    return False

  t.append((TokenTypes.LITERAL_INT, int(lit, 16)))
  return True

def s_arg(s, t):
  # "asdf"
  # 0x1234
  # 1234
  # identyfikator

  s.push()
  if s_qstring(s, t):
    s.dis()
    return True
  s.pop()

  s.push()
  if s_cname(s, t):
    s.dis()
    return True
  s.pop()

  s.push()
  if s_hexlit(s, t):
    s.dis()
    return True
  s.pop()

  s.push()
  if s_declit(s, t):
    s.dis()
    return True
  s.pop()

  return False

def s_label(s, t):
  # WS* : CNAME [WS* COMMENT]
  s.reset()
  s_ws(s)
  if s.peek(1, True) != ':':
    return False

  s.get()
  t.append((TokenTypes.LABEL_INDICATOR, ':'))

  if not s_cname(s, t):
    t.pop()
    raise AsmException("Missing label name after : in line %i" % s.ln_no)

  s_ws(s)
  try:
    res = s_comment(s)
    if res:
      return True
  except StreamException:
    return True

  return False

def s_stmt(s, t):
  # WS* CNAME WS+ (ARG WS* [, WS* ARG])? WS* COMMENT?
  s.reset()
  s_ws(s)
  if not s_cname(s, t):
    return False
  s_ws(s)

  if not s.empty():
    while s_arg(s, t):
      s_ws(s)
      if s.empty() or not s_comma(s):
        break
      s_ws(s)

  s_ws(s)

  try:
    res = s_comment(s)
    if res:
      return True
  except StreamException:
    return True

  return False

def s_comment(s):
  # WS* "#" WS* COMMENT
  # s.reset()
  s_ws(s)
  if s.peek() == '#':
    return True

  return False

def s_parse(s):
  t = []

  if s_comment(s):
    return t

  if s_label(s, t):
    return t

  if s_stmt(s, t):
    return t

  return t


class Assembler():
  def __init__(self):
    self.output = []
    self.output_sz = 0
    self.labels = {}
    self.current_ln_no = None
    self.ins_set = {
      "NOP",
      "INT3"
    }
    self.ins_list = [
     #INT3        CC        8    -         -         -         -         -
     ( "INT3", 0xcc, 8, None, None, None, None, None, ),

     #NOP         90        8    -         -         -         -         -
     ( "NOP", 0x90, 8,  None, None, None, None, None, )

    ]

  def handle_data(self, tokens):
    fmt = {
        "db": ("B", 1),
        "dw": ("H", 2),
        "dd": ("I", 4),
        "dq": ("Q", 8)
    }[tokens[0][1]]

    output = []
    output_sz = 0

    for t in tokens[1:]:
      if t[0] == TokenTypes.LITERAL_STR:
        output.append(t[1])
        output_sz += len(t[1])
        continue

      if t[0] == TokenTypes.LITERAL_INT:
        output.append(struct.pack(fmt[0], t[1]))
        output_sz += fmt[1]
        continue

      if t[0] == TokenTypes.IDENTIFIER:
        output.append((OutputElement.LABEL, t[1], fmt[1]))
        output_sz += fmt[1]
        continue

      raise Exception("handle data failed")

    #print output
    return (output, output_sz)

  def handle_org(self, tokens):
    self.org = tokens[1][1]
    return True

  def handle_ins(self, tokens):
    ins = tokens[0][1].upper()

    for e in self.ins_list:
      if e[0] == ins:
        break
    else:
      raise AsmException("Instruction found but not found wtf '%s'" % ins)

    self.output.append(chr(e[1]))
    self.output_sz += e[2] / 8
    return True

  def handle_label(self, tokens):
    if len(tokens) != 2:
      raise AsmException(
          "Unexpected extra characters in label name on line %i" % (
              self.current_ln_no
          ))

    if tokens[1][0] != TokenTypes.IDENTIFIER:
      raise AsmException(
          "Syntax error at line %i" % self.current_ln_no)

    label_name = tokens[1][1]

    if label_name in self.labels:
      raise AsmException("Label redeclared in line %i" % self.current_ln_no)

    self.labels[label_name] = self.output_sz
    return True

  def phase1(self, data):
    for ln_no, ln in data:
      res = self.phase1_worker(ln_no, ln)
      if not res:
        raise AsmException("Something went wrong in phase 1 at line %i" % ln_no)

  def phase1_worker(self, ln_no, ln):
    self.current_ln_no = ln_no
    s = Stream(ln, ln_no)
    try:
      tokens = s_parse(s)
    except StreamException as e:
      print ln_no, ":", e
      raise

    if len(tokens) == 0:
      return True

    print tokens
    if (tokens[0][0] == TokenTypes.IDENTIFIER and
        tokens[0][1] in {"db", "dw", "dd", "dq"}):
      output, output_sz = self.handle_data(tokens)
      self.output += output
      self.output_sz += output_sz
      return True

    if tokens[0][0] == TokenTypes.IDENTIFIER and tokens[0][1] in {"!org"}:
      return self.handle_org(tokens)

    if (tokens[0][0] == TokenTypes.IDENTIFIER and
        tokens[0][1].upper() in self.ins_set):
      return self.handle_ins(tokens)

    if tokens[0][0] == TokenTypes.LABEL_INDICATOR:
      return self.handle_label(tokens)

    raise AsmException("Unknown directive/instruction/etc in line %i" % ln_no)

  def phase2(self):
    for i in xrange(len(self.output)):
      if type(self.output[i]) is str:
        continue

      el_type = self.output[i][0]
      el_args = self.output[i][1:]

      if el_type == OutputElement.LABEL:
        fmt = {
            1: "B",
            2: "H",
            4: "I",
            8: "Q"
        }[el_args[1]]

        label = el_args[0]
        if label not in self.labels:
          raise AsmException("Unknown label '%s'" % label)

        self.output[i] = struct.pack(fmt, self.labels[label] + self.org)
        continue

      raise AsmException("Unsupported output element %s" % repr(self.output[i]))


  def get_output(self):
    return self.output


def main():
  if len(sys.argv) != 2:
    sys.exit("usage: casm.py <fname.asm>")

  infname = sys.argv[1]
  outfname = os.path.splitext(infname)[0] + ".bin"

  with open(infname, "r") as f:
    data = f.read().splitlines()

  data = [(i + 1, ln) for i, ln in enumerate(data) if ln.strip()]

  c = Assembler()

  c.phase1(data)
  c.phase2()

  with open(outfname, "wb") as f:
    f.write(''.join(c.get_output()))

if __name__ == "__main__":
  main()


