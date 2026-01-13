#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "cmd_search.h"

#define BUFFER_SIZE (1024)

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
  char full_path[BUFFER_SIZE];

  if (cmd_search(cmd, full_path, BUFFER_SIZE) == true)
  {
    printf("%s is %s\n", cmd, full_path);
    return true;
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

  if (check_builtins(cmd, builtins, builtins_len) == false && check_commands(cmd) == false)
  {
    printf("%s: not found\n", cmd);
  }
}
