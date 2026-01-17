#include <readline/history.h>
#include <string.h>

#include "cmd_history.h"

#define BUFFER_SIZE (1024)

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

bool cmd_history_write(char* hist_file, HISTORY_STATE* hs, HIST_ENTRY** he)
{
  FILE* file = fopen(hist_file, "w");

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
  return true;
}

bool cmd_history_append(char* hist_file, HISTORY_STATE* hs, HIST_ENTRY** he, int* idx)
{
  FILE* file = fopen(hist_file, "a");
  int i;

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
  return true;
}
