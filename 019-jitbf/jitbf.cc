#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <windows.h>
#include <stdint.h>
#include <string>
#include <map>

struct bfcontext_t {
  uint32_t pc;
  uint32_t ptr;
  uint8_t mem[32768];
};

// ARG1 --> bfcontext_t
// ECX   pc
// EDX   ptr
// EAX mem[n]

std::string jit_prolog;
std::string jit_epilog;
std::string jit_ptr_inc;
std::string jit_ptr_dec;

std::string read_jit_piece(FILE *f) {
  char buf[1024];
  int i = 0;

  for (;;) {
    buf[i++] = fgetc(f);
    if (feof(f)) {
      return std::string(buf, i - 1);
    }

    if (i >= 4 && memcmp("\xcc\xcc\xcc\xcc", buf + i - 4, 4) == 0) {
      return std::string(buf, i - 4);
    }
  }
}

void load_jit_opcodes() {
  FILE *f = fopen("jit", "rb");
  jit_prolog = read_jit_piece(f);
  jit_epilog = read_jit_piece(f);
  jit_ptr_inc = read_jit_piece(f);
  jit_ptr_dec = read_jit_piece(f);
  fclose(f);
}

std::map<uint32_t, void*> jit_map;
size_t jit_idx;
uint8_t *jit;

void jit_append(const std::string &s) {
  memcpy(jit + jit_idx, s.data(), s.size());
  jit_idx += s.size();
}

void jit_me(bfcontext_t *ctx, char *bf) {
  if (jit_map.find(ctx->pc) != jit_map.end()) {
    printf("JIT FOUND: %u @ %p\n", ctx->pc, jit_map[ctx->pc]);
    auto fnc = (void(*)(bfcontext_t *))jit_map[ctx->pc];
    fnc(ctx);
    return;
  }

  void *new_fnc = (void*)(jit + jit_idx);
  uint32_t new_pc = ctx->pc;
  jit_append(jit_prolog);

 
  bool end_now = false;
  while (bf[ctx->pc] != '\0') {
    switch (bf[ctx->pc]) {
      case '>': jit_append(jit_ptr_inc); break;
      case '<': jit_append(jit_ptr_dec); break;
      default: end_now = true; break;
    }
    if (end_now) break;
    ctx->pc++;
  }

  jit_append(jit_epilog);

  ctx->pc = new_pc;
  jit_map[new_pc] = new_fnc;
  printf("JIT CREATED: %u @ %p\n", new_pc, new_fnc);  
  auto fnc = (void(*)(bfcontext_t *))new_fnc;
  fnc(ctx);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    puts("jitbf <filename.bf>");
    return 1;
  }
  load_jit_opcodes();  

  FILE *f = fopen(argv[1], "r");
  if (f == NULL) {
    puts("file not found");
    return 2;
  }

  fseek(f, 0, SEEK_END);
  size_t sz = ftell(f);
  fseek(f, 0, SEEK_SET);

  std::vector<char> bf;
  bf.resize(sz + 1);
  fread(&bf[0], 1, sz, f);
  fclose(f);
  
  static bfcontext_t ctx;

  jit = (uint8_t*)VirtualAlloc(
      NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
  printf("%p\n", jit);

  while (bf[ctx.pc] != '\0') {
    switch (bf[ctx.pc]) {
      case '>': jit_me(&ctx, &bf[0]); continue;
      case '<': jit_me(&ctx, &bf[0]); continue;
      case '+': ctx.mem[ctx.ptr]++; break;
      case '-': ctx.mem[ctx.ptr]--; break;
      case '.': putchar(ctx.mem[ctx.ptr]); break;
      case ',': ctx.mem[ctx.ptr] = getchar(); break;

      case '[': {
        if (ctx.mem[ctx.ptr] != 0)
          break;

        int counter = 1;
        ctx.pc++;
        while (counter != 0) {
          if (bf[ctx.pc] == ']') counter--;
          if (bf[ctx.pc] == '[') counter++;
          ctx.pc++;
        }
        continue;
      }

      case ']': {
        int counter = -1;
        while (counter != 0) {
          ctx.pc--;
          if (bf[ctx.pc] == ']') counter--;
          if (bf[ctx.pc] == '[') counter++;
        }
        continue;
      }
      
      default:
        break;
    }

    ctx.pc += 1;
  }
    

  return 0;
}

