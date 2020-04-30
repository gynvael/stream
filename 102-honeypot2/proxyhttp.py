#!/usr/bin/env python3
import threading
import socket
import time
import sys

TIMEOUT = 10  # Seconds.
DATA_LIMIT = 64 * 1024

HOST = ''
PORT = 8080

DST_HOST = '192.168.2.226'
DST_PORT = 8080

def recv_thread(c, p, end):
  d = b""

  while not end.is_set():
    try:
      data = c.recv(4096)
    except socket.timeout:
      continue
    if not data:
      end.set()
      continue
    p.sendall(data)

    if len(d) > DATA_LIMIT:
      d = d[:DATA_LIMIT]
    else:
      d += data

    lines = d.splitlines()
    if len(lines) > 1:
      print(str(lines[0], 'iso-8859-2'))

    # TODO: detect \r\n\r\n to get the full HTTP request header
    # In case of POST/PUT look at content-length and get more data
    # Create a new function to do this.


def send_thread(c, p, end, rth):
  start = time.time()
  while not end.is_set():
    if time.time() - start > TIMEOUT:
      print("TIMEOUT")
      sys.stdout.flush()
      break

    try:
      data = p.recv(4096)
    except socket.timeout:
      continue

    if not data:
      break

    c.sendall(data)

  end.set()

  rth.join()
  p.shutdown(socket.SHUT_RDWR)
  c.shutdown(socket.SHUT_RDWR)
  p.close()
  c.close()

def main():
  while True:

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
      s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
      s.bind((HOST, PORT))
      s.listen(16)
      conn, addr = s.accept()

      try:
        p = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        p.connect((DST_HOST, DST_PORT))
      except ConnectionError:
        print("Failed to connect to wordpress")
        conn.shutdown(socket.SHUT_RDWR)
        continue

      p.settimeout(1)
      conn.settimeout(1)

      end = threading.Event()

      rth = threading.Thread(target=recv_thread, args=(conn, p, end), daemon=True)
      sth = threading.Thread(target=send_thread, args=(conn, p, end, rth), daemon=True)

      rth.start()
      sth.start()


if __name__ == "__main__":
  main()
