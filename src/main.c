#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cmd_exec.h"
#include "cmd_search.h"
#include "parse_tokens.h"
#include "type.h"

#define BUFFER_SIZE (1024)
#define MAX_TOKENS (1024)

#define STDOUT_REDIRECT_SHORT ">"
#define STDOUT_REDIRECT "1>"

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
  char* redirect_file_name;
  int original_fd;

  while (1)
  {
    if (redirect_file_name != NULL)
    {
      dup2(original_fd, fileno(stdout));
    }

    printf("$ ");
    res = fgets(input, BUFFER_SIZE, stdin);

    if (res != NULL)
    {
      // Zeroize tokens
      memset(tokens, 0, sizeof(tokens));

      redirect_file_name = NULL;

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

      // Check for redirection
      for (int i = 0; i < num_tokens; i++)
      {
        if (strcmp(tokens[i], STDOUT_REDIRECT_SHORT) == 0 ||
            strcmp(tokens[i], STDOUT_REDIRECT) == 0)
        {
          redirect_file_name = tokens[i + 1];
          tokens[i] = NULL;
          break;
        }
      }

      if (redirect_file_name != NULL)
      {
        // printf("Redirecting output to %s\n", redirect_file_name);
        original_fd = dup(fileno(stdout));
        int new_fd = open(redirect_file_name, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        dup2(new_fd, fileno(stdout));
      }

      if (strncmp(command, EXIT_CMD, BUFFER_SIZE) == 0 ||
          strncmp(command, QUIT_CMD, BUFFER_SIZE) == 0)
      {
        break;
      }

      if (strncmp(command, ECHO_CMD, BUFFER_SIZE) == 0)
      {
        for (int idx = 1; idx < num_tokens && tokens[idx] != NULL; idx++)
        {
          printf("%s ", tokens[idx]);
        }
        printf("\n");
        continue;
      }

      if (strncmp(command, TYPE_CMD, BUFFER_SIZE) == 0)
      {
        handle_type(tokens[1], builtins, sizeof(builtins) / sizeof(builtins[0]));
        continue;
      }

      if (strncmp(command, CHDIR_CMD, BUFFER_SIZE) == 0)
      {
        char* directory = tokens[1];

        if (directory[0] == '~')
        {
          snprintf(full_path, BUFFER_SIZE, "%s/%s", getenv("HOME"), &directory[1]);
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
