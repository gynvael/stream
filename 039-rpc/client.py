# Echo client program
import socket
import json
import struct

HOST = '127.0.0.1'
PORT = 50007

class rpc(object):
  def __init__(self):
    self.s = None

  def connect(self):
    self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    self.s.connect((HOST, PORT))

  def disconnect(self):
    self.s.close()

  def __getattr__(self, name):

    def worker(*args):
      self.connect()
      d = {
          "func": name,
          "argv": list(args)
          }
      d = json.dumps(d)
      print d

      self.s.sendall(struct.pack("<I", len(d)))
      self.s.sendall(d)

      data = self.s.recv(4)
      if not data:
        self.disconnect()
        return

      data_len = struct.unpack("<I", data)[0]

      data = self.s.recv(data_len)
      if not data:
        self.disconnect()
        return
      if data_len != len(data):
        print "zrob to porzadnie lol"
        os.exit(1)
      self.disconnect()

      print data  # 3
      data = json.loads(data)
      return data

    return worker
   
r = rpc()
print r.add(1, 2)
print r.sub(10, 2)
print r.upper("asdf")
print r.asdf(12,3)

