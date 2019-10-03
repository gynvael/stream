#!/usr/bin/python3
import distsystem

__all__ = [ "DistInt" ]

def _get_int(b):
    if type(b) is DistInt:
      return b.v
    return int(b)

class DistInt:
  def __init__(self, v=0):
    self.v = v

  def dumps(self):
    return str(v)

  def loads(self, v):
    self.v = int(v)

  def set(self, v):
    self.v = v
    distsystem.push_change(self)
    print("%x: v changed --> %i" % (id(self), self.v))

  def get(self):
    return self.v

  def __add__(self, b):
    return self.v + _get_int(b)

  def __iadd__(self, b):
    self.set(self.v + _get_int(b))
    return self

def test():
  a = DistInt(4)
  print(a + 5)
  a += 5
  print(a.get())
  a += DistInt(10)
  print(a.get())


  pass

if __name__ == "__main__":
  test()
