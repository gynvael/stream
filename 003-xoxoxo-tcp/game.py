
__all__ = ["Game"]


class Board():
  X = 'x'
  O = 'o'
  EMPTY = ' '

  def __init__(self):
    self._board = [Board.EMPTY] * 9
    
  def get(self):
    return self._board[:]  # copy.copy(self.board)

  def put(self, where, what):
    assert what in [Board.X, Board.O]
    assert where in range(9)

    if self._board[where] != Board.EMPTY:
      return False

    self._board[where] = what
    return True

  def move_available(self):
    return Board.EMPTY in self._board


class Game():
  X = Board.X
  O = Board.O
  EMPTY = Board.EMPTY

  PLAYER1 = X
  PLAYER2 = O
  PLAYERS = [PLAYER1, PLAYER2]

  def __init__(self):
    self._board = Board()
    self._turn = Game.PLAYER1
    self._end = False
    self._winner = None  # None, PLAYER1, PLAYER2

  def get_end(self):
    return self._end

  def get_winner(self):
    return self._winner

  def get_turn(self):
    return self._turn

  def get_turn_no(self):
    return Game.PLAYERS.index(self._turn)

  def get_board(self):
    return self._board.get()

  def make_move(self, where):
    assert where in range(9)

    if self._end:
      return False

    if not self._board.put(where, self._turn):
      return False

    if self._check_winner():
      self._winner = self._turn
      self._end = True
      return True

    if not self._board.move_available():
      self._end = True
      return True

    if self._turn == Game.PLAYER1:
      self._turn = Game.PLAYER2
    else:
      self._turn = Game.PLAYER1

    #self._turn = {
    #    PLAYER1: PLAYER2, PLAYER2: PLAYER1
    #    }[self._turn]

    #self._turn = PLAYER2 if self._turn == PLAYER1 else PLAYER2

    return True

  def _check_winner(self):
    b = self._board.get()
    winning_pos = [
        [ 0, 1, 2 ],
        [ 3, 4, 5 ],
        [ 6, 7, 8 ],
        [ 0, 3, 6 ],
        [ 1, 4, 7 ],
        [ 2, 5, 8 ],
        [ 0, 4, 8 ],
        [ 2, 4, 6 ]
    ]

    for w in winning_pos:
      if (b[w[0]] == b[w[1]] and 
          b[w[1]] == b[w[2]] and
          not b[w[0]] == Game.EMPTY):
        return True

    return False

if __name__ == "__main__":
  g = Game()
  print(g.get_board())
  print(g.get_turn())
  print(g.get_winner())
  print(g.get_end())

  print(g.make_move(0))

  print(g.get_board())
  print(g.get_turn())
  print(g.get_winner())
  print(g.get_end())

  print(g.make_move(0))
  print(g.make_move(1))  

  print(g.get_board())
  print(g.get_turn())
  print(g.get_winner())
  print(g.get_end())




  

