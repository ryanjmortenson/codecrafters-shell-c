#ifndef __CMD_HISTORY__
#define __CMD_HISTORY__

#include <readline/history.h>
bool cmd_history_read(char* hist_file);
bool cmd_history_write(char* hist_file, HISTORY_STATE* hs, HIST_ENTRY** he);
bool cmd_history_append(char* hist_file, HISTORY_STATE* hs, HIST_ENTRY** he, int* idx);

#endif
