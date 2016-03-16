#include <cstdio>
#include <stdint.h>
#include <windows.h>
#include "NetSock.h"
#include "console.h"

void usage() {
  printf("usage: xoxoxo <host> <port>\n");
}

class XoXoXo {
 public:
  XoXoXo() :
    s_circle("s_circle", 2),
    s_cross("s_cross", 5)
  {
  }

  bool HandleSingleRequest(NetSock &s) {
    uint8_t packet_id;

    int ret = s.Read(&packet_id, 1);
    if (ret == 0) {
      puts("disconnected");
      return false;
    }

    switch(packet_id) {
      case 'G': // get_move request
        {
          uint8_t move = (uint8_t)HandleGetMoveRequest();
          s.Write("g", 1);
          s.Write(&move, 1);
        }
        break;

      case 'W': // show_welcome
        HandleShowWelcome();
        break;

      case 'D': // show_draw
        HandleShowDraw();
        break;

      case 'B': // show_board
        {
          char board[9];
          if (s.ReadAll(board, 9) != 9) {
            puts("disconnected");
            return false;
          }
          HandleShowBoard(board);
        }
        break;

      case 'P': // show_player_turn
        {
          char player;
          if (s.Read(&player, 1) != 1) {
            puts("disconnected");
            return false;
          }
          HandleShowPlayerTurn(player);
        }
        break;

        case 'M': // show_move_error
        {
          char player;
          if (s.Read(&player, 1) != 1) {
            puts("disconnected");
            return false;
          }
          HandleShowMoveError(player);
        }
        break;

        case 'I': // show_winner
        {
          char player;
          if (s.Read(&player, 1) != 1) {
            puts("disconnected");
            return false;
          }
          HandleShowWinner(player);
        }
        break;

      default:
        puts("invalid packet ID");
        return false;
    }

    return true;
  }
  

 private:
  DecompressedXP s_circle;
  DecompressedXP s_cross;

  void HandleShowWelcome() {
    DecompressedXP img("xoxoxo-title", 0);
    img.SetPalette();
    img.Render(0, 0);
    getchar(); // XXX: zmienic to
  }

  void HandleShowBoard(char *board) {
    s_circle.SetPalette();
    s_cross.SetPalette();

    HANDLE con = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD topleft = { 0, 0 };

    SetConsoleCursorPosition(con, topleft); 

    DWORD num;
    FillConsoleOutputCharacter(con, ' ', 80 * 38, topleft, &num);
    FillConsoleOutputAttribute(con, 1, 80 * 38, topleft, &num);

    for (int i = 0; i < 9; i++) {
      int x = (i % 3) * 8 + 40;
      int y = (i / 3) * 8;

      switch (board[i]) {
        case ' ':
          break;

        case 'x':
          s_cross.Render(x, y);
          break;

        case 'o':
          s_circle.Render(x, y);
          break;
      }
    }

    /*printf("%c%c%c\n%c%c%c\n%c%c%c\n",
        board[0], board[1], board[2], 
        board[3], board[4], board[5], 
        board[6], board[7], board[8]);*/
  }

  void HandleShowPlayerTurn(char player) {
    printf("player %c turn\n", player);
  }

  void HandleShowMoveError(char player) {
    printf("error error wrong move player %c \n", player);
  }  

  void HandleShowWinner(char player) {
    printf("player %c won\n", player);
  }  

  void HandleShowDraw() {
    puts("draw!");
  }
  
  int HandleGetMoveRequest() {
    printf("your move (0-8)!\n");

    int i;
    scanf("%i", &i);

    return i;
  }
};

// a.asdf    (&a)->asdf
// a->asdf <----- (*a).asdf
// x = 10;
// while (    x      
// --   > 
// 0  ) { }
//
// x = x + 1
// x += 1

static PaletteArchiver *g_pa;

BOOL WINAPI CtrlCHandler(DWORD /*dwCtrlType*/) {
  g_pa->Restore();     
  return FALSE;
}

int main(int argc, char **argv) {
  PaletteArchiver pa;
  g_pa = &pa;
  SetConsoleCtrlHandler(CtrlCHandler, TRUE);

  if (argc != 3) {
    usage();
    return 1;
  }

  unsigned short port;
  if (sscanf(argv[2], "%hu", &port) != 1) {
    usage();
    return 2;
  }

  const char *host = argv[1];

  XoXoXo xo;

  NetSock::InitNetworking();
  NetSock s;
  
  if (!s.Connect(host, port)) {
    fprintf(stderr, "Could not connect!\n");
    return 3;
  }

  for(;;) {

    if (!xo.HandleSingleRequest(s)) {
      break;
    }

  }
  
  s.Disconnect();
   
  return 0;
}


