#include <stdio.h>
#include <string.h>

int main() {
  char buf[256] = {0};

  puts("Give me the password:");
  scanf("%255[^\n]", buf);
  // printf("[%s] %i\n", buf, strlen(buf));

  if (strlen(buf) != 16) {
    puts("Nope.");
    return 1;
  }

  unsigned int hash[4] = { 
    0x12345678,
    0x89765443,
    0x41414141,
    0xabcdef12
  };

  for (size_t i = 0; i < 16; i++) {
    hash[0] = ((hash[0] << 3) | (hash[0] >> 3)) ^ (buf[i] * 12345);
    hash[1] = ((hash[1] << 5) | (hash[1] >> 5)) ^ (buf[i] * 716238);
    hash[2] = ((hash[2] << 7) | (hash[2] >> 7)) ^ (buf[i] * 192399);
    hash[3] = ((hash[3] << 9) | (hash[3] >> 9)) ^ (buf[i] * 912839);
  }

  // printf("%.8x %.8x %.8x %.8x\n", hash[0], hash[1], hash[2], hash[3]);
  
  if (hash[0] == 0xfff4a1ce &&
      hash[1] == 0xb5a4a9a7 &&
      hash[2] == 0xf05a945c &&
      hash[3] == 0x9504a82d) {
    puts("GOOD WORK!");
  } else {
    puts("Nope.");
  }
}

