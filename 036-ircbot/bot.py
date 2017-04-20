#!/usr/bin/python3
import sys
import socket
import threading
import os
import time
import random
import string
import telnetlib 
import queue

def recvuntil(sock, txt):
  d = b""
  while d.find(txt) == -1:
    try:
      dnow = sock.recv(1)
      if len(dnow) == 0:
        return False
    except socket.error as msg:
      return False
    d += dnow
  return d

def recvall(sock, n):
  d = ""
  while len(d) != n:
    try:
      dnow = sock.recv(n - len(d))
      if len(dnow) == 0:
        return False
    except socket.error as msg:
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


def gen_random_nick(prefix):
  return prefix + ''.join(random.choice(string.ascii_letters) for _ in range(4))

def toutf8(x):
  return bytes(x, "utf-8")

class ReciverThread(threading.Thread):
  def __init__(self, s, q):
    threading.Thread.__init__(self)
    self.s = s
    self.q = q

  def run(self):
    while True:
      txt = str(self.s.recvuntil(b"\n").strip(), "utf-8")
      self.q.put(txt)

def handle_PING(s, src, cmd, params):
  s.sendall(toutf8("PONG " + params + "\r\n"))
  print("********************** SENT PONG")

def xhandle_PRIVMSG(s, src, cmd, params):
  x = params.split(" ", 2)
  if len(x) != 2:
    return

  chan, msg = x
  
  if msg.startswith(":"):
    msg = msg[1:]

  if msg == "!rand":
    x = random.randint(0, 10000000000000000000000000)
    s.sendall(toutf8("PRIVMSG %s :%i\r\n" % (chan, x)))
    print("RANDOM SENT")

def xhandle_376(s, src, cmd, params):
  s.sendall(toutf8("JOIN #gynvaelstream\r\n"))

handlers = {}


def init_handlers():
  for k, v in globals().items():
    if k.startswith("handle_"):
      handlers[k[7:]] = v

  print(handlers)

def go():  
  global HOST
  global PORT
  init_handlers()
  s = gsocket(socket.AF_INET, socket.SOCK_STREAM)
  s.connect((HOST, PORT))

  nick = gen_random_nick("Pacynka_")

  s.sendall(toutf8("NICK %s\r\n" % nick))
  s.sendall(toutf8("USER %s %s %s :%s\r\n" % (nick, nick, nick, nick)))
  
  msg_queue = queue.Queue()

  rt = ReciverThread(s, msg_queue)
  rt.daemon = True
  rt.start()
 
  while True:
    try:
      msg = msg_queue.get(timeout=0.1)
      msg_split = msg.split(" ", 2)
      if len(msg_split) == 3:
        src, cmd, params = msg_split
      else:
        src, cmd, params = "", *msg_split
      print("DEBUG: (%s, %s, %s)" % (src, cmd, params))
      if cmd in handlers:
        print("USING HANDLER: %s" % (str(handlers[cmd])))
        handlers[cmd](s, src, cmd, params)
      else:
        print("HANDLER NOT FOUND")
        

    except queue.Empty:
      pass

  #t = telnetlib.Telnet()
  #t.sock = s
  #t.interact()

  s.close()

HOST = 'irc.freenode.net'
PORT = 6667
go()

