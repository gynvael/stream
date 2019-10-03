def client(ip, port, message):
  with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    sock.connect((ip, port))
    sock.sendall(bytes(message, 'ascii'))
    response = str(sock.recv(1024), 'ascii')
    print("Received: {}".format(response))
    sock.shutdown(socket.SHUT_RDWR)



