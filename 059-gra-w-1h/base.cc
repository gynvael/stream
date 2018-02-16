#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <vector>
#include <math.h>

#undef main
SDL_Surface *framebuffer;

double gNow;   // Timer.
double gDiff;  // Difference from last frame.

double last_spawn;

bool key[4096];

char map[30 * 60 + 1] =
"............................................................"
"............................................................"
"...Sxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx................."
"..........................................x................."
"..........................................x................."
"..........................................x................."
"..................xxxxxxxxxxxxxxxxxxxxxxxxx................."
"..................x............................xxxxxx......."
"..................x............................x....x......."
"..................x............................x....x......."
"..................xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx....x......."
"....................................................x......."
"....................................................x......."
"....................................................x......."
"....................................................x......."
"....................................................x......."
"....................................................x......."
"............xxxxxxxxxx..............................x......."
".....................x..............................x......."
".....................x..............................x......."
".....................x..............................x......."
".....................x..............................x......."
".....................xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx......."
"............................................................"
"............................................................"
"............................................................"
"............................................................"
"............................................................"
"............................................................"
"............................................................";


struct Enemy {
  float x, y;
  int hp;
  size_t m;
};

Enemy enemy[10000];


struct Img {
  int w, h;
  uint8_t *data;
};


Img *evil;
Img *tower;
Img *tiles;
Img *select;

struct XY {
  int x, y;
};

std::vector<XY> towers;
std::vector<float> towers_cooldown;


XY player;
int cash = 100;

std::vector<XY> moves;
int start_x, start_y;


void make_moves() {
  int sp = strchr(map, 'S') - map;
  int x = sp % 60;
  int y = sp / 60;

  start_x = x;
  start_y = y;

  for(;;) {
    if (map[x + y * 60] == 'x') {
      map[x + y * 60] = 'X';
    }

    if (map[(x + 1) + (y + 0) * 60] == 'x') {
      x +=1;
    } else if (map[(x - 1) + (y + 0) * 60] == 'x') {
      x -=1;
    } else if (map[(x - 0) + (y + 1) * 60] == 'x') {
      y +=1;
    } else if (map[(x - 0) + (y - 1) * 60] == 'x') {
      y -=1;
    } else {
      break;
    }
    //printf("%i %i\n", x, y);
    moves.push_back(XY{x, y});
  }

}



Img *load_img(const char*fname, int w, int h) {
  Img *img = new Img;
  img->w = w;
  img->h = h;
  img->data = new uint8_t[w * h * 4];

  FILE *f = fopen(fname, "rb");
  fread(img->data, 1, w * h * 4, f);
  fclose(f);

  return img;
}

void img_blit(int x, int y, Img *img) {
  int w = framebuffer->w;
  uint8_t *px = (uint8_t*)framebuffer->pixels;

  for (int j = 0; j < img->h; j++) {
    for (int i = 0; i < img->w; i++) {

      uint8_t a = img->data[(i + j * img->w) * 4 + 3];

      if (a < 31) {
        continue;
      }

      px[((i + x) + (j + y) * w) * 4 + 0] = img->data[(i + j * img->w) * 4 + 0];
      px[((i + x) + (j + y) * w) * 4 + 1] = img->data[(i + j * img->w) * 4 + 1];
      px[((i + x) + (j + y) * w) * 4 + 2] = img->data[(i + j * img->w) * 4 + 2];
      px[((i + x) + (j + y) * w) * 4 + 3] = a;
    }
  }
}

void img_tile(int x, int y, Img *img, int si, int sj) {
  int w = framebuffer->w;
  uint8_t *px = (uint8_t*)framebuffer->pixels;

  int sx = si * 16;
  int sy = sj * 16;

  for (int j = 0; j < 16; j++) {
    for (int i = 0; i < 16; i++) {

      uint8_t a = img->data[(i + sx + (j + sy) * img->w) * 4 + 3];

      if (a < 31) {
        continue;
      }

      px[((i + x) + (j + y) * w) * 4 + 0] = img->data[(i + sx + (j + sy) * img->w) * 4 + 2];
      px[((i + x) + (j + y) * w) * 4 + 1] = img->data[(i + sx + (j + sy) * img->w) * 4 + 1];
      px[((i + x) + (j + y) * w) * 4 + 2] = img->data[(i + sx + (j + sy) * img->w) * 4 + 0];
      px[((i + x) + (j + y) * w) * 4 + 3] = a;
    }
  }
}


void RenderMap() {
  int cx = 160;
  int cy = 120;

  for (int j = 0; j < 30; j++) {
    for (int i = 0; i < 60; i++) {
      switch(map[i + j * 60]) {
        case '.': img_tile(cx + i * 16, cy + j * 16, tiles, 0, 0); break;
        case 'o':
         img_tile(cx + i * 16, cy + j * 16, tiles, 0, 0);
         img_blit(cx + i * 16, cy + j * 16, tower);
         break;
        case 'X': img_tile(cx + i * 16, cy + j * 16, tiles, 8, 8); break;
      }
    }
  }
}

void line(int sx, int sy, int ex, int ey) {
  int w = framebuffer->w;
  int h = framebuffer->h;
  (void)h;
  uint8_t *px = (uint8_t*)framebuffer->pixels;

  float dx = ((float)ex - float(sx));
  float dy = ((float)ey - float(sy));

  float d = sqrt(dx * dx + dy * dy);

  dx /= (d * 4);
  dy /= (d * 4);



  float x = sx;
  float y = sy;

  while((int)x != ex && (int)y != ey) {
    sx = x;
    sy = y;
    px[(sx + sy * w) * 4 + 0] = 0xff;
    px[(sx + sy * w) * 4 + 1] = 0xff;
    px[(sx + sy * w) * 4 + 2] = 0xff;
    px[(sx + sy * w) * 4 + 3] = 0xff;
    x += dx;
    y += dy;
  }

}

int enemies = 0;
void SpawnEnemy() {

  if (enemies > 10000) {
    return;
  }

  enemy[enemies].x = start_x;
  enemy[enemies].y = start_y;

  enemy[enemies].hp = 100;
  enemy[enemies++].m = 0;
}


void HandleRendering() {

  if (gNow - last_spawn > 0.8) {
    SpawnEnemy();
    last_spawn = gNow;
  }

  int w = framebuffer->w;
  int h = framebuffer->h;
  (void)h;
  uint8_t *px = (uint8_t*)framebuffer->pixels;

  // SpawnEnemy();

  if (key[SDL_SCANCODE_DOWN]) {
    key[SDL_SCANCODE_DOWN] = false;
    player.y = (player.y + 1) % 30;
  }

  if (key[SDL_SCANCODE_UP]) {
    key[SDL_SCANCODE_UP] = false;
    player.y = (player.y - 1) % 30;
    if (player.y < 0) player.y = 29;
  }

  if (key[SDL_SCANCODE_RIGHT]) {
    key[SDL_SCANCODE_RIGHT] = false;
    player.x = (player.x + 1) % 60;
  }

  if (key[SDL_SCANCODE_LEFT]) {
    key[SDL_SCANCODE_LEFT] = false;
    player.x = (player.x - 1) % 60;
    if (player.x < 0) player.x = 59;
  }

  if (key[SDL_SCANCODE_SPACE]) {
    key[SDL_SCANCODE_SPACE] = false;

    if (map[player.x + player.y * 60] == '.' &&
        cash >= 10) {
      cash -= 10;
      towers.push_back(XY{player.x, player.y});
      towers_cooldown.push_back(float{gNow});
      map[player.x + player.y * 60] = 'o';
    }
  }
  printf("Cash: %i         \r", cash);

  memset(px, 0, w * h * 4);
  RenderMap();

  for (int i = 0; i < 100; i++) {
    if (enemy[i].hp > 0) {
      img_blit(enemy[i].x * 16 + 160,
               enemy[i].y * 16 + 120, evil);

      if (enemy[i].m < moves.size()) {
        XY m = moves[enemy[i].m];

        float dx = enemy[i].x - m.x;
        float dy = enemy[i].y - m.y;

        float d = fabs(dx) + fabs(dy);

        float movement = gDiff * 3.0;

        if (movement >= d) {
          enemy[i].m++;
        } else {
          if (dx < 0) enemy[i].x += movement;
          if (dx > 0) enemy[i].x -= movement;
          if (dy < 0) enemy[i].y += movement;
          if (dy > 0) enemy[i].y -= movement;
        }
      }
    }
  }


  for (size_t i = 0; i < towers_cooldown.size(); i++) {
    if (gNow - towers_cooldown[i] < 1.0)
      continue;

    towers_cooldown[i] = gNow;

    float x = towers[i].x;
    float y = towers[i].y;

    for (int j = 0; j < enemies; j++) {
      if (enemy[j].hp <= 0) {
        continue;
      }

      float dx = (x - enemy[j].x);
      float dy = (y - enemy[j].y);
      float dist_sq = dx * dx + dy * dy;

      if (dist_sq > 4 * 4) {
        continue;
      }

      enemy[j].hp -= 10;
      if (enemy[j].hp <= 0) {
        cash += 8;
      }

      line(towers[i].x * 16 + 160 + 8,
           towers[i].y * 16 + 120 + 8,
           enemy[j].x * 16 + 160 + 10,
           enemy[j].y * 16 + 120 + 9);

      break;
    }

  }



  img_blit(160 + player.x * 16, 120 + player.y * 16,
          select);

  /*
  if (key[SDL_SCANCODE_DOWN]) {
    px[(320 + 240 * w) * 4 + 1] = 0xff;
  } else {
    px[(320 + 240 * w) * 4 + 0] = 0xff;
  }
  */
}


int main(int, char **) {
  evil = load_img("evil.data", 20, 18);
  select = load_img("select.data", 16, 16);
  tiles = load_img("snow-expansion.data", 352, 224);
  tower = load_img("tower.data", 16, 16);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
    puts("SDL failed");
    return 3;
  }

  // Initialize timer.
  gNow = (double)SDL_GetTicks() / 1000.0;
  gDiff = 0.001;

  SDL_Window *win = SDL_CreateWindow(
    "Base Code", SDL_WINDOWPOS_CENTERED, 1200 + 200,
    1280, 720, SDL_WINDOW_SHOWN);
  if (win == nullptr) {
    puts("SDL failed");
    SDL_Quit();
    return 4;
  }

  SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  if (ren == nullptr){
    puts("SDL failed");
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }
  make_moves();
  framebuffer = SDL_GetWindowSurface(win);

  SDL_Event e;
  bool end = false;
  while (!end) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        end = true;
        break;
      }

      if (e.type == SDL_KEYDOWN) {
        //printf("%i\n", e.key.keysym.scancode); fflush(stdout);
        key[e.key.keysym.scancode] = true;
      }

      if (e.type == SDL_KEYUP) {
        //printf("%i\n", e.key.keysym.scancode); fflush(stdout);
        key[e.key.keysym.scancode] = false;
      }
    }

    // Update timer.
    double now = (double)SDL_GetTicks() / 1000.0;
    gDiff = now - gNow;
    gNow = now;

    HandleRendering();
    SDL_UpdateWindowSurface(win);
  }

  framebuffer = nullptr;
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}
