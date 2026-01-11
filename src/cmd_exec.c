#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

bool execute(char* cmd, char** argv)
{
  int pid = fork();

  if (pid == 0)
  {
    if (execvp(cmd, argv))
    {
      exit(127);
    }
  }
  else if (pid > 0)
  {
    waitpid(pid, NULL, 0);
  }
  else
  {
    return false;
  }

  return true;
}
