#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void handle_type(char** tokens, int token_len, char** builtins, int builtins_len)
{
  bool found = false;
  char* path = getenv("PATH");

  for (int idx = 0; idx < builtins_len; idx++)
  {
    if (strcmp(builtins[idx], tokens[1]) == 0)
    {
      printf("%s is a shell builtin\n", tokens[1]);
      found = true;
      break;
    }
  }

  if (found == false)
  {
    printf("%s: not found\n", tokens[1]);
  }
}
