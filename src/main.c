#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type.h"

#define BUFFER_SIZE (1024)
#define NUM_TOKENS  (1024)

#define ECHO_CMD "echo"
#define EXIT_CMD "exit"
#define TYPE_CMD "type"

char* builtins[] = {
  ECHO_CMD,
  EXIT_CMD,
  TYPE_CMD
};

int main(int argc, char *argv[])
{
  // Flush after every printf
  setbuf(stdout, NULL);
  static char input[BUFFER_SIZE];
  static char* tokens[NUM_TOKENS];
  int token_idx;
  int len;
  char* cur_token;
  char* command;

  while (1)
  {
    for (int idx = 0; idx < NUM_TOKENS; idx++)
    {
      tokens[idx] = NULL;
    }

    printf("$ ");

    char* res = fgets(input, BUFFER_SIZE, stdin);
    if (res != NULL)
    {
      // Remove carriage return from end of string
      len = strnlen(input, BUFFER_SIZE);
      input[len - 1] = '\0';

      // Tokenize input
      token_idx = 0;
      cur_token = strtok(input, " ");
      tokens[token_idx++] = cur_token;

      while ((cur_token = strtok(NULL, " ")) != NULL) {
        tokens[token_idx++] = cur_token;
      }

      command = tokens[0];
      if (strncmp(command, EXIT_CMD, BUFFER_SIZE) == 0)
      {
        break;
      }

      if (strncmp(command, ECHO_CMD, BUFFER_SIZE) != 0)
      {
        for (int idx = 1; idx < token_idx; idx++)
        {
          printf("%s ", tokens[idx]);
        }
        printf("\n");
        continue;
      }

      if (strstr(command, TYPE_CMD) != 0)
      {
        handle_type(tokens[1],
                    builtins,
                    sizeof(builtins)/sizeof(builtins[0]));
        continue;
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
