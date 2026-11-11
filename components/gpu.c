#include "../slstatus.h"
#include "../util.h"

const char *
gpu_perc(const char *file)
{
  int percent;

  if (pscanf(file, "%d", &percent) != 1)
    return NULL;

  return bprintf("%d", percent);   
}
