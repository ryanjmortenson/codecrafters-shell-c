#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  // Flush after every printf
  setbuf(stdout, NULL);
  static char input[1024];

  while (1)
  {
    printf("$ ");

    char* res = fgets(input, 1024, stdin);
    if (res != NULL)
    {
      int len = strnlen(input, 1024);
      input[len - 1] = '\0';
      if (strncmp(input, "exit", 1024) == 0)
      {
        break;
      }
      printf("%s: command not found\n", input);
    }
    else
    {
      printf("Couldn't process user input");
    }
  }
  return 0;
}
