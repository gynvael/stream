import string
import SocketServer
from pprint import pprint

packet = """GET / HTTP/1.1
Host: localhost:1234
Connection: keep-alive
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.87 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8
Accept-Encoding: gzip, deflate, br
Accept-Language: pl-PL,pl;q=0.9,en-US;q=0.8,en;q=0.7
Cookie: __utma=111872281.1709459494.1529601866.1529601866.1529601866.1; __utmc=111872281; __utmz=111872281.1529601866.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none)

"""

packet = packet.replace('\r', '').replace('\n', '\r\n')

def parse_http_packet(p):
  p = iter(p)
  verb = None
  query = None
  ver = None

  headers = []  # (header, value)
  header_name = None
  header_value = None

  S_START = 0
  S_VERB = 1
  S_WS1 = 2
  S_QUERY = 3
  S_WS2 = 4
  S_VER = 5
  S_NLR0 = 6

  S_HEADERS_S = 10
  S_HEADER = 11
  S_WS3 = 12
  S_VALUE = 13
  S_NLR1 = 14
  S_NLR2 = 15

  S_BAD = -1

  CH_CONTROL = [chr(i) for i in range(0, 32) + [127]]
  CH_HEADER = string.uppercase + string.lowercase + string.digits + '-'

  current_state = S_START

  while True:
    ch = next(p)

    if current_state == S_START:
      if ch in string.uppercase:
        current_state = S_VERB
        verb = ch
        continue

      # Fall-through.

    if current_state == S_VERB:
      if ch in string.uppercase:
        # current_state = S_VERB
        verb += ch
        continue

      if ch == " ":
        current_state = S_WS1
        continue

      # Fall-through.

    if current_state == S_WS1:
      if ch == " ":
        continue

      if ch not in CH_CONTROL:
        query = ch
        current_state = S_QUERY
        continue

      # Fall-through.

    if current_state == S_QUERY:
      if ch == " ":
        current_state = S_WS2
        continue

      if ch not in CH_CONTROL:
        query += ch
        continue

      # Fall-through.

    if current_state == S_WS2:
      if ch == " ":
        continue

      if ch not in CH_CONTROL:
        ver = ch
        current_state = S_VER
        continue

      # Fall-through.

    if current_state == S_VER:
      if ch == "\r":
        current_state = S_NLR0
        continue

      if ch not in (CH_CONTROL + [' ']):
        ver += ch
        continue

      # Fall-through.

    if current_state == S_NLR0:
      if ch == "\n":
        current_state = S_HEADERS_S
        continue

      # Fall-through.

    if current_state == S_HEADERS_S:
      if ch == "\r":
        current_state = S_NLR2
        continue

      if ch in CH_HEADER:
        header_name = ch
        current_state = S_HEADER
        continue

      # Fall-through.

    if current_state == S_HEADER:
      if ch in CH_HEADER:
        header_name += ch
        continue

      if ch == ':':
        current_state = S_WS3
        continue

      # Fall-through.

    if current_state == S_WS3:
      if ch == ' ':
        continue

      if ch not in CH_CONTROL:
        header_value = ch
        current_state = S_VALUE
        continue

      # Fall-through.

    if current_state == S_VALUE:
      if ch == '\r':
        current_state = S_NLR1
        headers.append((header_name, header_value))
        continue

      if ch not in CH_CONTROL:
        header_value += ch
        continue

      # Fall-through.

    if current_state == S_NLR1:
      if ch == '\n':
        current_state = S_HEADERS_S
        continue

      # Fall-through.

    if current_state == S_NLR2:
      if ch == '\n':
        break

      # Fall-through.

    current_state = S_BAD
    if current_state == S_BAD:
      return

  return {
    "verb": verb,
    "query": query,
    "ver": ver,
    "headers": headers
  }


def recvuntil(sock, txt):
  d = ""
  while d.find(txt) == -1:
    try:
      dnow = sock.recv(1)
      if len(dnow) == 0:
        return ("DISCONNECTED", d)
    except socket.timeout:
      return ("TIMEOUT", d)
    except socket.error as msg:
      return ("ERROR", d)
    d += dnow
  return ("OK", d)

class MyTCPHandler(SocketServer.BaseRequestHandler):
  def handle(self):
    data = recvuntil(self.request, "\r\n\r\n")[1]
    http = parse_http_packet(data)

    if http["query"] == "/":
      self.request.sendall(
"""HTTP/1.1 200 OK
Content-Type: text/html;charset=utf-8

It works! Try <a href="/asdf">/asdf</a>""")
      return

    if http["query"] == "/asdf":
      self.request.sendall(
"""HTTP/1.1 200 OK
Content-Type: text/html;charset=utf-8

Also works!""")
      return

    self.request.sendall(
"""HTTP/1.1 404 Not Found
Content-Type: text/html;charset=utf-8

Not found. Seriously.""")

if __name__ == "__main__":
  HOST, PORT = "localhost", 9999

  # Create the server, binding to localhost on port 9999
  server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)

  # Activate the server; this will keep running until you
  # interrupt the program with Ctrl-C
  server.serve_forever()





