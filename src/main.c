#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  static char input[1024];

  // TODO: Uncomment the code below to pass the first stage
  //
  printf("$ ");

  scanf("%s", input);
  printf("%s: command not found", input);
  return 0;
}
