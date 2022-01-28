#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main() {
  int i, tp;

  srand(time(NULL) + getpid());

  for (i = 0; i < 6; i++) {
    tp = rand() % 256;
    printf("%s%X%s", tp < 16 ? "0" : "", tp, i < 5 ? ":" : "\n");
  }
  return EXIT_SUCCESS;
}
