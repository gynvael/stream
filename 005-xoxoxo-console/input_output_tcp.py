import socket
import output_interface
import input_interface
from game import Game

class InputOutputTcp(output_interface.OutputInterface,
                     input_interface.InputInterface):
  def __init__(self, bind_ip, bind_port):
    self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    self.s.bind((bind_ip, bind_port))
    self.s.listen(5)

    print("Waiting for connection on port:", bind_port)
    self.conn, addr = self.s.accept()
    print("Player 2 connect:", addr)

    self.s.close()

  def get_move(self):
    #self.conn.sendall(bytes('G', 'utf-8'))
    self.conn.sendall(b'G')

    packet_id = self.conn.recv(1)
    if packet_id != b'g':
      raise Exception("invalid response")

    move = ord(self.conn.recv(1))  # recv b''
    # 9 in range(9)
    return move

  def show_welcome(self):
    self.conn.sendall(b'W')

  def show_board(self, board):
    self.conn.sendall(b'B' + bytes(''.join(board), 'utf-8'))

  def show_player_turn(self, player):
    self.conn.sendall(b'P' + bytes(player, 'utf-8'))

  def show_move_error(self, player):
    self.conn.sendall(b'M' + bytes(player, 'utf-8'))

  def show_draw(self):
    self.conn.sendall(b'D')

  def show_winner(self, player):
    self.conn.sendall(b'I' + bytes(player, 'utf-8'))













