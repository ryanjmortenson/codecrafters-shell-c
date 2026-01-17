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
  char* env_path = getenv(PATH_ENV_NAME);
  int path_len = strlen(env_path) + 1;
  bool ret = true;
  int list_idx = 0;
  DIR* dp;
  struct dirent* ep;
  char* cur_executable;
  char* cur_env_path;
  char* cur_full_path;
  char* env_path_copy;
  int res;
  int len;

  env_path_copy = malloc(path_len);
  if (env_path_copy == NULL)
  {
    printf("Couldn't malloc a copy of the path environment variable\n");
    ret = false;
    goto cleanup;
  }

  cur_full_path = malloc(BUFFER_SIZE);
  if (cur_full_path == NULL)
  {
    printf("Couldn't malloc a buffer for storing a full path\n");
    ret = false;
    goto cleanup;
  }

  // Copy path for using strtok which inserts '\0' at each token
  memset(env_path_copy, 0, path_len);
  strncpy(env_path_copy, env_path, path_len);

  cur_env_path = strtok(env_path_copy, PATH_SPLIT_TOKEN);
  while (cur_env_path != NULL && strlen(cur_env_path) < BUFFER_SIZE && list_idx < list_len)
  {
    if ((dp = opendir(cur_env_path)) != NULL)
    {
      while ((ep = readdir(dp)) != NULL)
      {
        if (ep->d_type == DT_REG)
        {
          res = snprintf(cur_full_path, BUFFER_SIZE, PATH_JOIN_FORMAT, cur_env_path, ep->d_name);

          if (res > 0 && res < BUFFER_SIZE && access(cur_full_path, X_OK) == 0)
          {
            len = strlen(ep->d_name) + 1;

            if ((cur_executable = malloc(len)) != NULL)
            {
              memset(cur_executable, 0, len);
              strncpy(cur_executable, ep->d_name, len);
              list[list_idx++] = cur_executable;
            }
          }
        }
      }

      if (closedir(dp) != 0)
      {
        printf("Couldn't close directory");
      }
    }

    cur_env_path = strtok(NULL, PATH_SPLIT_TOKEN);
  }

  list[list_idx] = malloc(1);
  memset(list[list_idx], 0, 1);
  list_idx++;

cleanup:
  if (cur_full_path != NULL)
  {
    free(cur_full_path);
  }

  if (env_path_copy != NULL)
  {
    free(env_path_copy);
  }

  return ret;
}
