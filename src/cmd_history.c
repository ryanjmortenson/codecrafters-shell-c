#include <readline/history.h>
#include <string.h>
#include <stdlib.h>

#include "cmd_history.h"

#define BUFFER_SIZE (1024)

int last_hist_appended = 0;

bool cmd_history_read(char* hist_file)
{
  FILE* file = fopen(hist_file, "r");
  char buf[BUFFER_SIZE];

  if (file != NULL)
  {
    while(fgets(buf, BUFFER_SIZE, file) != NULL)
    {
      if (buf[0] != '\n')
      {
        buf[strlen(buf) - 1] = '\0';
        add_history(buf);
      }
    }
  }
  else
  {
    printf("Couldn't open history file: %s\n", hist_file);
    return false;
  }

  return true;
}

bool cmd_history_write(char* hist_file)
{
  HISTORY_STATE* hs = history_get_history_state();
  HIST_ENTRY** he = history_list();
  FILE* file;

  if (hs != NULL && he != NULL && *he != NULL)
  {
    file = fopen(hist_file, "w");
    if (file != NULL)
    {
      for (int i = 0; i < hs->length; i++)
      {
        fputs(he[i]->line, file);
        fputc('\n', file);
      }
      fclose(file);
    }
    else
    {
      printf("Couldn't open history file: %s\n", hist_file);
      return false;
    }
  }
  else
  {
    printf("Couldn't get history information");
    return false;
  }

  return true;
}

bool cmd_history_append(char* hist_file, int* idx)
{
  HISTORY_STATE* hs = history_get_history_state();
  HIST_ENTRY** he = history_list();
  FILE* file;
  int i;

  if (hs != NULL && he != NULL && *he != NULL)
  {
    file = fopen(hist_file, "a");
    if (file != NULL)
    {
      for (i = *idx; i < hs->length; i++)
      {
        fputs(he[i]->line, file);
        fputc('\n', file);
      }
      fclose(file);
      *idx = i;
    }
    else
    {
      printf("Couldn't open history file: %s\n", hist_file);
      return false;
    }
  }
  else
  {
    printf("Couldn't get history information");
    return false;
  }

  return true;
}


bool cmd_history_handle(char** argv)
{
  HISTORY_STATE* hs = history_get_history_state();
  HIST_ENTRY** he = history_list();
  int num_to_show;

  if (hs == NULL || he == NULL || *he == NULL)
  {
    printf("Couldn't get history\n");
  }

  num_to_show = hs->length;

  if (argv[1] != NULL)
  {
    if (strcmp(argv[1], "-r") == 0)
    {
      return cmd_history_read(argv[2]);
    }
    else if (strcmp(argv[1], "-w") == 0)
    {
      return cmd_history_write(argv[2]);
    }
    else if (strcmp(argv[1], "-a") == 0)
    {
      return cmd_history_append(argv[2], &last_hist_appended);
    }
    else
    {
      num_to_show = atoi(argv[1]);
    }
  }

  for (int i = hs->length - num_to_show; i < hs->length; i++)
  {
    printf("\t%d %s\n", i+1, he[i]->line);
  }
  return true;
}
