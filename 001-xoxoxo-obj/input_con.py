import input_interface
import sys

__all__ = [ "InputCon" ] 

class InputCon(input_interface.InputInterface):
  def get_move(self):
    move = sys.stdin.readline().strip()
    try:
      move = int(move)
    except ValueError:
      return False

    if move in range(9):
      return move

    return False


