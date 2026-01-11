#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#define BUF_SIZE (1024)

static bool check_builtins(char* cmd, char** builtins, int builtins_len)
{
  bool found = false;

  for (int idx = 0; idx < builtins_len; idx++)
  {
    if (strcmp(builtins[idx], cmd) == 0)
    {
      printf("%s is a shell builtin\n", cmd);
      return true;
    }
  }

  return false;
}

static bool check_commands(char* cmd)
{
  char* cur_path_dir;
  DIR* dp;
  struct dirent *ep;
  char* path = getenv("PATH");
  char full_path[BUF_SIZE];
  char path_copy[BUF_SIZE];

  strcpy(path_copy, path);
  cur_path_dir = strtok(path_copy, ":");
  while (cur_path_dir != NULL)
  {
    dp = opendir(cur_path_dir);
    if (dp != NULL)
    {
      while ((ep = readdir(dp)) != NULL)
      {
        if (ep->d_type == DT_REG &&
            strcmp(cmd, ep->d_name) == 0 &&
            snprintf(full_path, BUF_SIZE, "%s/%s", cur_path_dir, ep->d_name) &&
            access(full_path, X_OK) == 0)
        {
          printf("%s is %s\n", ep->d_name, full_path);
          return true;
        }
      }
    }

    cur_path_dir = strtok(NULL, ":");
  }

  return false;
}

void handle_type(char* cmd, char** builtins, int builtins_len)
{
  if (cmd == NULL)
  {
    printf("Invalid input cmd\n");
  }

  if (builtins == NULL || *builtins == NULL)
  {
    printf("Invalid input builtins\n");
  }

  if (check_builtins(cmd, builtins, builtins_len) == false &&
      check_commands(cmd) == false)
  {
    printf("%s: not found\n", cmd);
  }
}
