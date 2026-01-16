#ifndef __CMD_SEARCH__
#define __CMD_SEARCH__

#include <stdbool.h>

bool cmd_search(char* cmd, char* full_path, int full_path_len);

bool get_cmd_list(char** list, int list_len);

#endif
