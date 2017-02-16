#!/usr/bin/python
import sys
import os
import socket
import telnetlib 
import threading
from struct import pack, unpack

# TheAifam5: https://docs.python.org/2/library/mimetypes.html


def recvuntil(sock, txt):
  d = ""
  while d.find(txt) == -1:
    try:
      dnow = sock.recv(1)
      if len(dnow) == 0:
        print "-=(warning)=- recvuntil() failed at recv"
        print "Last received data:"
        print d
        return False
    except socket.error as msg:
      print "-=(warning)=- recvuntil() failed:", msg
      print "Last received data:"
      print d      
      return False
    d += dnow
  return d

def recvall(sock, n):
  d = ""
  while len(d) != n:
    try:
      dnow = sock.recv(n - len(d))
      if len(dnow) == 0:
        print "-=(warning)=- recvuntil() failed at recv"
        print "Last received data:"
        print d        
        return False
    except socket.error as msg:
      print "-=(warning)=- recvuntil() failed:", msg
      print "Last received data:"
      print d      
      return False
    d += dnow
  return d

# Proxy object for sockets.
class gsocket(object):
  def __init__(self, *p):
    self._sock = socket.socket(*p)

  def __getattr__(self, name):
    return getattr(self._sock, name)

  def recvall(self, n):
    return recvall(self._sock, n)

  def recvuntil(self, txt):
    return recvuntil(self._sock, txt)  

class Handler(threading.Thread):
  def __init__(self, s, addr):
    super(Handler, self).__init__()

    self.s = s
    self.addr = addr

  def return_http(self, data, status=200, status_text="OK",
                  mime="application/binary"):
    self.s.sendall("HTTP/1.1 %i %s\r\n" % (status, status_text))
    self.s.sendall("Content-Type: %s\r\n" % mime)
    self.s.sendall("Content-Length: %i\r\n" % len(data))
    self.s.sendall("\r\n")
    self.s.sendall(data)

  def run(self):
    data = recvuntil(self.s, "\r\n\r\n").splitlines()
    verb, path, ver = data[0].split(" ")

    try:

      if ".." in path or not path.startswith("/"):
        raise Exception("bye")

      if path == "/":
        path = "/index.html"

      final_path = "public_html" + path
      with open(final_path, "rb") as f:
        d = f.read()

      _, ext = os.path.splitext(path) 

      mime = {
          ".html": "text/html;charset=utf-8",
          ".png": "image/png",
      }

      mtype = "application/binary"

      if ext in mime:
        mtype = mime[ext]

      self.return_http(d, mime=mtype)

    except:
      self.return_http("NO!", status=403, status_text="Forbidden",
                       mime="text/plain;charset=utf-8")

    self.s.shutdown(socket.SHUT_RDWR)
    self.s.close()



   


def main():
  server = gsocket(socket.AF_INET, socket.SOCK_STREAM)
  server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
  server.bind(("0.0.0.0", 8080))  
  server.listen(5)

  while True:
    conn, addr = server.accept()
    print "[  INFO  ] New connection: %s:%i" % addr

    th = Handler(conn, addr)
    th.daemon = False
    th.start()


if __name__ == "__main__":
  main()
