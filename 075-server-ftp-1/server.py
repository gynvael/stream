import socket
import threading
import socketserver

def recvuntil(s, marker, szlimit):
  data = b""
  while True:
    d = s.recv(1)
    if d == "":
      raise Exception("disconnected")

    data += d
    if marker in data:
      return data

    if len(data) > szlimit:
      raise Exception("too much data")

class ThreadedTCPRequestHandler(socketserver.BaseRequestHandler):

  def handle_USER(self, arg):
    if arg == 'anonymous':
      print("Logging in user anonymous")
      self.request.sendall(b"331 User name OK, give me e-mail.\r\n")
    else:
      print("Log in incorrect (%s)" % arg)
      self.request.sendall(b"530 Only anonymous exists.\r\n")

  def handle_PASS(self, arg):
    self.request.sendall(b"230 User logged in, proceed.\r\n")

  def handle_SYST(self, arg):
    self.request.sendall(b"215 UNIX Type: L8\r\n")

  def handle_PWD(self, arg):
    reply = "257 %s\r\n" % (self.get_pwd())
    self.request.sendall(bytes(reply, 'utf-8'))

  def handle_TYPE(self, arg):
    self.type = arg
    self.request.sendall(b'200 Command okay.\r\n')

  def handle_SIZE(self, arg):
    f = self.get_file_by_fname(arg)
    if f is None:
      self.request.sendall(b'550 File not found.\r\n')
      return

    self.request.sendall(b'213 %i\r\n' % len(f))

  def handle_QUIT(self, arg):
    self.request.sendall(b'221 Bye.\r\n')
    self.end_connection = True

  def handle_PASV(self, arg):
    ip = (127, 0, 0, 1)
    p = self.data_port
    reply = b'227 Entering Passive Mode (%i,%i,%i,%i,%i,%i).\r\n' % (
      ip[0], ip[1], ip[2], ip[3],
      p / 256, p % 256
    )
    self.request.sendall(reply)

  def handle_LIST(self, arg):
    self.request.sendall(b'150 Opening IMAGE mode data connection for LIST\r\n')
    s = self.data_s.accept()[0]
    reply = []
    f = self.get_file_by_namelist(self.cwd)
    if type(f) is not dict:
      # TODO send error
      return
    for el in f.keys():
      reply.append("-rwxrwx--- 1 root vboxsf 4114 Oct  4 21:58 %s" % el)

    reply = '\r\n'.join(reply)
    s.sendall(bytes(reply, "utf-8"))
    s.shutdown(socket.SHUT_RDWR)
    s.close()

    self.request.sendall(b'226 Transfer complete\r\n')





  def handle_CWD(self, arg):
    f = self.get_file_by_fname(arg)
    if f is None or type(f) is not dict:
      self.request.sendall(b'550 File not found.\r\n')
      return

    self.request.sendall(b'250 OK.\r\n')

  def get_file_by_fname(self, fname):
    return self.get_file_by_namelist(self.fname_to_namelist(fname))

  def get_file_by_namelist(self, namelist):
    if namelist == ['']:
      namelist = []

    d = self.files
    for el in namelist:
      print("asdf: ", d, el)
      if el not in d:
        return None

      el = d[el]

      if type(el) is dict:
        d = el
        continue

      break
    return d


  def fname_to_namelist(self, fname):
    if fname.startswith('/'):
      fname = fname[1:]
    else:
      fname = self.cwd[1:] + '/' + fname
    return fname.split('/')

  def namelist_to_fname(self, namelist):
    return '/' + '/'.join(namelist)

  def get_pwd(self):
    return self.namelist_to_fname(self.cwd)

  def handle(self):
    self.request.sendall(b"220 Livestream FTP Server.\r\n")
    self.request.settimeout(60)

    self.end_connection = False
    self.data_s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    self.data_s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    self.data_s.bind(("0.0.0.0", 0))
    self.data_s.listen(1)
    self.data_port = self.data_s.getsockname()[1]

    self.cwd = []
    methods = {}
    self.files = {
      "<h1>asdgf<u>xyz": b"<h1>asdfg<u>asdgf",
      "dir": {
        "abc": b"abc"
      }
    }

    for method_name in dir(self):
      if method_name.startswith("handle_"):
        cmd = method_name.replace("handle_", "")
        methods[cmd] = getattr(self, method_name)

    while not self.end_connection:
      ln = str(recvuntil(self.request, b"\r\n", 4096), 'utf-8').strip()
      print(ln)
      command, args = (ln.split(" ", 1) + [""])[:2]
      if command in methods:
        methods[command](args)
        continue

      break

class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
  pass

if __name__ == "__main__":
  HOST, PORT = "0.0.0.0", 1021
  server = ThreadedTCPServer((HOST, PORT), ThreadedTCPRequestHandler)
  ip, port = server.server_address
  server_thread = threading.Thread(target=server.serve_forever)
  server_thread.daemon = True
  server_thread.start()
  print("Server loop running in thread:", server_thread.name)

  server_thread.join()
  server.shutdown()
  server.server_close()

