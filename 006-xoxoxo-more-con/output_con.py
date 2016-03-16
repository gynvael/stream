import output_interface

__all__ = [ "OutputCon" ] 

class OutputCon(output_interface.OutputInterface):
  def show_welcome(self):
    print("Welcome to XOXOXOX!!!\n")

  def show_board(self, board):
    print("%c | %c | %c\n"
          "--+---+--\n"
          "%c | %c | %c\n"
          "--+---+--\n"
          "%c | %c | %c\n" % tuple(board))

  def show_player_turn(self, player):
    print("Now moves (0-8):", player)

  def show_move_error(self, player):
    print("It's a no no what you've done! Plz repeat the move.")

  def show_draw(self):
    print("It's a draw! Nobody wins!")

  def show_winner(self, player):
    print("Player %s wins! CONGRATZ!" % player)


