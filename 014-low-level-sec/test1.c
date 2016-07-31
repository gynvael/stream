void a() {
  char xxx[1024 * 1024] = {1};
  a();
}

int main(void) {
  a();

  return 0;
}


