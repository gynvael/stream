import output_interface

__all__ = ["OutputProxy"]

class OutputProxy(output_interface.OutputInterface):
  def __init__(self):
    self._outputs = []

  def register_output(self, output):
    self._outputs.append(output)

  def show_welcome(self):
    for output in self._outputs:
      output.show_welcome()

  def show_board(self, board):
    for output in self._outputs:
      output.show_board(board)

  def show_player_turn(self, player):
    for output in self._outputs:
      output.show_player_turn(player)

  def show_move_error(self, player):
    for output in self._outputs:
      output.show_move_error(player)

  def show_draw(self):
    for output in self._outputs:
      output.show_draw()

  def show_winner(self, player):
    for output in self._outputs:
      output.show_winner(player)

