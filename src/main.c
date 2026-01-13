#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cmd_exec.h"
#include "cmd_search.h"
#include "type.h"
#include "parse_tokens.h"

#define BUFFER_SIZE (1024)
#define MAX_TOKENS (1024)

#define ECHO_CMD "echo"
#define EXIT_CMD "exit"
#define QUIT_CMD "quit"
#define TYPE_CMD "type"
#define PWD_CMD "pwd"
#define CHDIR_CMD "cd"

char* builtins[] = {
  ECHO_CMD, // Formatting comment
  EXIT_CMD, // Formatting comment
  QUIT_CMD, // Formatting comment
  TYPE_CMD, // Formatting comment
  PWD_CMD,  // Formatting comment
  CHDIR_CMD // Formatting comment
};

int main(int argc, char* argv[])
{
  // Flush after every printf
  setbuf(stdout, NULL);
  static char input[BUFFER_SIZE];
  static char* tokens[MAX_TOKENS];
  static char full_path[BUFFER_SIZE];
  int len;
  int num_tokens;
  char* command;
  char* res;

  while (1)
  {
    printf("$ ");
    res = fgets(input, BUFFER_SIZE, stdin);

    if (res != NULL)
    {
      // Zeroize tokens
      memset(tokens, 0, sizeof(tokens));

      // Remove carriage return from end of string
      len = strnlen(input, BUFFER_SIZE);
      if (len == 1)
      {
        continue;
      }

      input[len - 1] = '\0';

      // Parse into tokens including spaces and quotes
      if (parse_tokens(input, len, tokens, MAX_TOKENS, &num_tokens) == false)
      {
        printf("Could not parse command\n");
        continue;
      }

      // Process commands
      command = tokens[0];
      if (strncmp(command, EXIT_CMD, BUFFER_SIZE) == 0 ||
          strncmp(command, QUIT_CMD, BUFFER_SIZE) == 0)
      {
        break;
      }

      if (strncmp(command, ECHO_CMD, BUFFER_SIZE) == 0)
      {
        for (int idx = 1; idx < num_tokens; idx++)
        {
          printf("%s ", tokens[idx]);
        }
        printf("\n");
        continue;
      }

      if (strncmp(command, TYPE_CMD, BUFFER_SIZE) == 0)
      {
        handle_type(tokens[1],
                    builtins,
                    sizeof(builtins) / sizeof(builtins[0]));
        continue;
      }

      if (strncmp(command, CHDIR_CMD, BUFFER_SIZE) == 0)
      {
        char* directory = tokens[1];

        if (directory[0] == '~')
        {
          snprintf(
            full_path, BUFFER_SIZE, "%s/%s", getenv("HOME"), &directory[1]);
          directory = full_path;
        }

        if (chdir(directory) != 0)
        {
          printf("cd: %s: No such file or directory\n", tokens[1]);
        }
        continue;
      }

      if (strncmp(command, PWD_CMD, BUFFER_SIZE) == 0)
      {
        if (getcwd(full_path, BUFFER_SIZE) != NULL)
        {
          printf("%s\n", full_path);
        }
        else
        {
          printf("Couldn't get current working director\n");
        }
        continue;
      }

      if (cmd_search(command, full_path, BUFFER_SIZE) == true)
      {
        if (execute(command, tokens) == true)
        {
          continue;
        }
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
