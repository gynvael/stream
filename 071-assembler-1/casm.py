#!/usr/bin/python
import sys
import os
import string
import struct

class StreamException(Exception):
  pass

class Stream():
  def __init__(self, data):
    self.data = data
    self.i = 0
    self.stack = []

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

def s_ctype(s, t):
  CTYPE0 = string.ascii_letters + "_.!:"
  CTYPE1 = string.ascii_letters + string.digits + "_."

  token = ""
  if s.peek() not in CTYPE0:
    return False

  token = s.get()

  try:
    while s.peek() in CTYPE1:
      token += s.get()
  except StreamException:
    pass

  t.append(token)  # TODO: maybe add more info on type

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

  t.append(string)
  return True

def s_declit(s, t):
  lit = ""
  while not s.empty() and s.peek() in string.digits:
    lit += s.get()

  if not lit:
    return False

  t.append(int(lit))
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

  t.append(int(lit, 16))
  return True

def s_arg(s, t):
  # "asdf"
  # 0x1234
  # 1234

  s.push()
  if s_qstring(s, t):
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

def s_stmt(s, t):
  # WS* CTYPE WS+ (ARG WS* [, WS* ARG])? WS* COMMENT?
  s.reset()
  s_ws(s)
  if not s_ctype(s, t):
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
  s.reset()
  s_ws(s)
  if s.peek() == '#':
    return True

  return False

def s_parse(s):
  t = []

  if s_comment(s):
    return t

  if s_stmt(s, t):
    return t

  return t


class Assembler():
  def __init__(self):
    self.output = []

  def handle_data(self, tokens):
    fmt = {
        "db": "B",
        "dw": "H",
        "dd": "I",
        "dq": "Q"
    }[tokens[0]]

    output = []

    for t in tokens[1:]:
      if type(t) is str:
        output.append(t)
        continue

      if type(t) in { int, long }:
        output.append(struct.pack(fmt, t))
        continue

      raise Exception("handle data failed")

    #print output
    return output

  def handle_org(self, tokens):
    self.org = tokens[1]

  def phase1(self, ln_no, ln):
    s = Stream(ln)
    try:
      tokens = s_parse(s)
    except StreamException as e:
      print ln_no, ":", e
      raise

    if len(tokens) == 0:
      return []

    output = []

    print tokens
    if tokens[0] in {"db", "dw", "dd", "dq"}:
      self.output += self.handle_data(tokens)

    if tokens[0] in {"!org"}:
      return self.handle_org(tokens)

    return None


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

  for ln_no, ln in data:
    res = c.phase1(ln_no, ln)


  with open(outfname, "wb") as f:
    f.write(''.join(c.get_output()))

if __name__ == "__main__":
  main()


