#include <dirent.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE (1024)

bool cmd_search(char* cmd, char* full_path, int full_path_len)
{
  DIR* dp;
  struct dirent* ep;
  char* path = getenv("PATH");
  char* cur_path_dir;
  char path_copy[BUFFER_SIZE];

  strcpy(path_copy, path);
  cur_path_dir = strtok(path_copy, ":");

  while (cur_path_dir != NULL)
  {
    if ((dp = opendir(cur_path_dir)) != NULL)
    {
      while ((ep = readdir(dp)) != NULL)
      {
        if (ep->d_type == DT_REG && strcmp(cmd, ep->d_name) == 0 &&
            snprintf(full_path, BUFFER_SIZE, "%s/%s", cur_path_dir, cmd) &&
            access(full_path, X_OK) == 0)
        {
          return true;
        }
      }
    }

    cur_path_dir = strtok(NULL, ":");
  }

  return false;
}
