#include <dirent.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE (1024)
#define PATH_SPLIT_TOKEN ":"
#define PATH_JOIN_FORMAT "%s/%s"
#define PATH_ENV_NAME "PATH"

bool cmd_search(char* cmd, char* full_path, int full_path_len)
{
  DIR* dp;
  struct dirent* ep;
  char* path = getenv(PATH_ENV_NAME);
  char* cur_path_dir;
  char path_copy[BUFFER_SIZE];

  strcpy(path_copy, path);
  cur_path_dir = strtok(path_copy, PATH_SPLIT_TOKEN);

  while (cur_path_dir != NULL)
  {
    if ((dp = opendir(cur_path_dir)) != NULL)
    {
      while ((ep = readdir(dp)) != NULL)
      {
        if (ep->d_type == DT_REG && strcmp(cmd, ep->d_name) == 0 &&
            snprintf(full_path, full_path_len, PATH_JOIN_FORMAT, cur_path_dir, cmd) &&
            access(full_path, X_OK) == 0)
        {
          return true;
        }
      }
    }

    cur_path_dir = strtok(NULL, PATH_SPLIT_TOKEN);
  }

  return false;
}

bool get_cmd_list(char** list, int list_len)
{
  DIR* dp;
  struct dirent* ep;
  char* path = getenv(PATH_ENV_NAME);
  int path_len = strlen(path) + 1;
  bool ret = true;
  int list_idx = 0;
  char* cur_path_dir;
  char* path_copy;
  char* full_path;
  int res;
  int len;
  char* cur;

  path_copy = malloc(path_len);
  if (path_copy == NULL)
  {
    printf("Couldn't malloc a copy of the path environment variable\n");
    ret = false;
    goto cleanup;
  }

  full_path = malloc(BUFFER_SIZE);
  if (full_path == NULL)
  {
    printf("Couldn't malloc a buffer for storing a full path\n");
    ret = false;
    goto cleanup;
  }

  // Copy path for using strtok which inserts '\0' at each token
  memset(path_copy, 0, path_len);
  strncpy(path_copy, path, path_len);

  cur_path_dir = strtok(path_copy, PATH_SPLIT_TOKEN);
  while (cur_path_dir != NULL && strlen(cur_path_dir) < BUFFER_SIZE && list_idx < list_len)
  {
    if ((dp = opendir(cur_path_dir)) != NULL)
    {
      while ((ep = readdir(dp)) != NULL)
      {
        if (ep->d_type == DT_REG)
        {
          res = snprintf(full_path, BUFFER_SIZE, PATH_JOIN_FORMAT, cur_path_dir, ep->d_name);

          if (res > 0 && res < BUFFER_SIZE && access(full_path, X_OK) == 0)
          {
            len = strlen(ep->d_name) + 1;

            if ((cur = malloc(len)) != NULL)
            {
              memset(cur, 0, len);
              strncpy(cur, ep->d_name, len);
              list[list_idx++] = cur;
            }
          }
        }
      }

      if (closedir(dp) != 0)
      {
        printf("Couldn't close directory");
      }
    }

    cur_path_dir = strtok(NULL, PATH_SPLIT_TOKEN);
  }

  list[list_idx] = malloc(1);
  memset(list[list_idx], 0, 1);
  list_idx++;

cleanup:
  if (full_path != NULL)
  {
    free(full_path);
  }

  if (path_copy != NULL)
  {
    free(path_copy);
  }

  return ret;
}
