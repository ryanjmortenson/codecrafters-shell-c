#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cmd_search.h"
#include "type.h"

#define BUFFER_SIZE (1024)
#define NUM_TOKENS  (1024)

#define ECHO_CMD "echo"
#define EXIT_CMD "exit"
#define TYPE_CMD "type"
#define PWD_CMD  "pwd"
#define CHDIR_CMD  "cd"

char* builtins[] = {
  ECHO_CMD,
  EXIT_CMD,
  TYPE_CMD,
  PWD_CMD,
  CHDIR_CMD
};

int main(int argc, char *argv[])
{
  // Flush after every printf
  setbuf(stdout, NULL);
  static char input[BUFFER_SIZE];
  static char* tokens[NUM_TOKENS];
  static char full_path[BUFFER_SIZE];
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
      command = tokens[0];

      while ((cur_token = strtok(NULL, " ")) != NULL)
      {
        tokens[token_idx++] = cur_token;
      }

      tokens[token_idx] = NULL;

      if (strncmp(command, EXIT_CMD, BUFFER_SIZE) == 0)
      {
        break;
      }

      if (strncmp(command, ECHO_CMD, BUFFER_SIZE) == 0)
      {
        for (int idx = 1; idx < token_idx; idx++)
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
                    sizeof(builtins)/sizeof(builtins[0]));
        continue;
      }

      if (strncmp(command, CHDIR_CMD, BUFFER_SIZE) == 0)
      {
        if (chdir(tokens[1]) != 0)
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
        int pid = fork();
        if (pid == 0)
        {
          if(execvp(command, tokens))
          {
            exit(127);
          }
        }
        else if (pid > 0)
        {
          waitpid(pid, NULL, 0);
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
