#!/usr/bin/python3
import socket
import threading
import socketserver
import time

HOST, PORT = 0.0.0.0, 31337
the_end = False

class ThreadedTCPRequestHandler(socketserver.BaseRequestHandler):
  def handle(self):
    data = str(self.request.recv(1024), 'ascii')
    cur_thread = threading.current_thread()
    response = bytes("{}: {}".format(cur_thread.name, data), 'ascii')
    self.request.sendall(response)

class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
  pass

server = ThreadedTCPServer((HOST, PORT), ThreadedTCPRequestHandler)

with server:
  ip, port = server.server_address
  server_thread = threading.Thread(target=server.serve_forever)
  server_thread.daemon = True
  server_thread.start()

  print("Server loop running in thread:", server_thread.name)

  while not the_end:
    time.sleep(0.250)

  server.shutdown()


