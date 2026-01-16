#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
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

#define COMPLETION_LIST_LEN (1024)
#define BUFFER_SIZE (1024)
#define MAX_TOKENS (1024)

#define STDOUT_REDIRECT_SHORT ">"
#define STDOUT_REDIRECT "1>"
#define STDERR_REDIRECT "2>"

#define STDOUT_APPEND_REDIRECT_SHORT ">>"
#define STDOUT_APPEND_REDIRECT "1>>"
#define STDERR_APPEND_REDIRECT "2>>"

#define ECHO_CMD "echo"
#define EXIT_CMD "exit"
#define QUIT_CMD "quit"
#define TYPE_CMD "type"
#define PWD_CMD "pwd"
#define CHDIR_CMD "cd"

char* builtins[] = {
  ECHO_CMD,  // Formatting comment
  EXIT_CMD,  // Formatting comment
  QUIT_CMD,  // Formatting comment
  TYPE_CMD,  // Formatting comment
  PWD_CMD,   // Formatting comment
  CHDIR_CMD, // Formatting comment
};

static char* completion_list[COMPLETION_LIST_LEN];

char* generator(const char* input, int state)
{
  static int list_index, len;
  char* name;

  if (!state)
  {
    list_index = 0;
    len = strlen(input);
  }

  while (list_index < COMPLETION_LIST_LEN && (name = completion_list[list_index++]))
  {
    if (strncmp(name, input, len) == 0)
    {
      return strdup(name);
    }
  }

  return NULL;
}

char** completion(const char* input, int start, int end)
{
  rl_attempted_completion_over = 1;
  return rl_completion_matches(input, generator);
}

int main(int argc, char* argv[])
{
  // Flush after every printf
  setbuf(stdout, NULL);
  static char* tokens[MAX_TOKENS];
  static char full_path[BUFFER_SIZE];
  char* input = NULL;
  int len;
  int num_tokens;
  char* command;
  int original_fd;
  FILE* redirection = NULL;
  int mode = 0;
  char tmp;
  int input_idx;
  int i;

  for (i = 0; i < COMPLETION_LIST_LEN; i++)
  {
    completion_list[i] = malloc(BUFFER_SIZE);
    memset(completion_list[i], 0, BUFFER_SIZE);
  }

  for (i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++)
  {
    strcpy(completion_list[i], builtins[i]);
  }

  get_cmd_list(&completion_list[i], COMPLETION_LIST_LEN - i);

  rl_attempted_completion_function = completion;

  while (1)
  {
    if (redirection != NULL)
    {
      dup2(original_fd, fileno(redirection));
    }

    if (input != NULL)
    {
      free(input);
      input = NULL;
    }
    input = readline("$ ");

    if (input != NULL)
    {
      // Zeroize tokens
      memset(tokens, 0, sizeof(tokens));

      // Remove carriage return from end of string
      len = strnlen(input, BUFFER_SIZE);
      if (len == 1)
      {
        continue;
      }

      // Parse into tokens including spaces and quotes
      if (parse_tokens(input, len, tokens, MAX_TOKENS, &num_tokens) == false)
      {
        printf("Could not parse command\n");
        continue;
      }

      // Process commands
      command = tokens[0];

      // Check for redirection
      redirection = NULL;
      for (int i = 0; i < num_tokens; i++)
      {
        if (strcmp(tokens[i], STDOUT_REDIRECT_SHORT) == 0 ||
            strcmp(tokens[i], STDOUT_REDIRECT) == 0)
        {
          redirection = stdout;
          mode = O_CREAT;
        }

        if (strcmp(tokens[i], STDERR_REDIRECT) == 0)
        {
          redirection = stderr;
          mode = O_CREAT;
        }

        if (strcmp(tokens[i], STDOUT_APPEND_REDIRECT_SHORT) == 0 ||
            strcmp(tokens[i], STDOUT_APPEND_REDIRECT) == 0)
        {
          redirection = stdout;
          mode = O_APPEND;
        }

        if (strcmp(tokens[i], STDERR_APPEND_REDIRECT) == 0)
        {
          redirection = stderr;
          mode = O_APPEND;
        }

        if (redirection != NULL)
        {
          original_fd = dup(fileno(redirection));
          int new_fd = open(tokens[i + 1], O_CREAT | O_WRONLY | mode, 0644);
          dup2(new_fd, fileno(redirection));
          tokens[i] = NULL;
          break;
        }
      }

      if (strncmp(command, EXIT_CMD, BUFFER_SIZE) == 0 ||
          strncmp(command, QUIT_CMD, BUFFER_SIZE) == 0)
      {
        for (int k = i; k < COMPLETION_LIST_LEN; k++)
        {
          free(completion_list[k]);
        }
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
