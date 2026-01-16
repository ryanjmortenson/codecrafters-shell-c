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
            snprintf(full_path, full_path_len, "%s/%s", cur_path_dir, cmd) &&
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

bool get_cmd_list(char** list, int list_len)
{
  DIR* dp;
  struct dirent* ep;
  char* path = getenv("PATH");
  char* cur_path_dir;
  char path_copy[BUFFER_SIZE];
  char full_path[BUFFER_SIZE];
  int list_idx = 0;

  strcpy(path_copy, path);
  cur_path_dir = strtok(path_copy, ":");

  while (cur_path_dir != NULL)
  {
    if ((dp = opendir(cur_path_dir)) != NULL)
    {
      while ((ep = readdir(dp)) != NULL)
      {
        if (ep->d_type == DT_REG &&
            snprintf(full_path, BUFFER_SIZE, "%s/%s", cur_path_dir, ep->d_name) &&
            access(full_path, X_OK) == 0 && list_idx < list_len)
        {
          strcpy(&list[list_idx++][0], ep->d_name);
        }
      }
      closedir(dp);
    }

    cur_path_dir = strtok(NULL, ":");
  }

  return false;
}
