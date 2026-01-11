#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#define BUF_SIZE (1024)

static bool check_builtins(char* tokens, char** builtins, int builtins_len)
{
  bool found = false;

  for (int idx = 0; idx < builtins_len; idx++)
  {
    if (strcmp(builtins[idx], tokens) == 0)
    {
      printf("%s is a shell builtin\n", tokens);
      return true;
    }
  }

  return false;
}

static bool check_commands(char* tokens)
{
  char* cur_token;
  DIR* dp;
  struct dirent *ep;
  char* path = getenv("PATH");
  char full_path[BUF_SIZE];
  char path_copy[BUF_SIZE];

  strcpy(path_copy, path);
  cur_token = strtok(path_copy, ":");
  while (cur_token != NULL)
  {
    dp = opendir(cur_token);
    if (dp != NULL)
    {
      while ((ep = readdir(dp)) != NULL)
      {
        if (ep->d_type == DT_REG &&
            strcmp(tokens, ep->d_name) == 0 &&
            snprintf(full_path, BUF_SIZE, "%s/%s", cur_token, ep->d_name) &&
            access(full_path, X_OK) == 0)
        {
          printf("%s is %s\n", ep->d_name, full_path);
          return true;
        }
      }
    }

    cur_token = strtok(NULL, ":");
  }

  return false;
}

void handle_type(char** tokens, int token_len, char** builtins, int builtins_len)
{
  char* cmd;

  if (tokens == NULL || *tokens == NULL)
  {
    printf("Invalid input\n");
  }

  if (builtins == NULL || *builtins == NULL)
  {
    printf("Invalid input\n");
  }

  cmd = tokens[1];
  if (check_builtins(cmd, builtins, builtins_len) == false &&
      check_commands(cmd) == false)
  {
    printf("%s: not found\n", cmd);
  }
}
