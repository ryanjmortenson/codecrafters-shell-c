#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  static char input[1024];

  while (1)
  {
    printf("$ ");

    scanf("%s", input);
    printf("%s: command not found\n", input);
  }
  return 0;
}
