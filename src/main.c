#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE (1024)

int main(int argc, char *argv[])
{
  // Flush after every printf
  setbuf(stdout, NULL);
  static char input[BUFFER_SIZE];

  while (1)
  {
    printf("$ ");

    char* res = fgets(input, BUFFER_SIZE, stdin);
    if (res != NULL)
    {
      int len = strnlen(input, BUFFER_SIZE);
      input[len - 1] = '\0';
      if (strncmp(input, "exit", BUFFER_SIZE) == 0)
      {
        break;
      }
      if (strstr(input, "echo") != 0)
      {
        printf("%s\n", &input[5]);
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
