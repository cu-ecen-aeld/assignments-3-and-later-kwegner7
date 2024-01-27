#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <stdlib.h>

/**********************************************************************
 * First argument is the full path to a file
 * Second argument is the string to be written to the file
 **********************************************************************/
void writer(const char fullPath[], const char stringToWrite[])
{
  int fd = open (fullPath, O_WRONLY | O_CREAT | O_TRUNC, 0664);
  if (fd == -1)
  {
    syslog(LOG_ERR, "ERROR: COULD NOT OPEN FILE %s FOR WRITING", fullPath);
  }
  else
  {
    // const char* buf = stringToWrite; // delete this line?
    ssize_t nr;
    nr = write (fd, stringToWrite, strlen(stringToWrite));
    if (nr == -1)
    {
      syslog(LOG_ERR, "ERROR: FAILURE WHILE WRITING TO FILE %s", fullPath);
    }
    else
    {
      syslog(LOG_DEBUG, "Writing %s to %s", stringToWrite, fullPath);
    }
  }
}

/**********************************************************************
 * main
 **********************************************************************/
int main(int argc, char**argv)
{
  if(argc == 1)
    return 1;

  if(argc == 2)
    return 1;

  writer(argv[1], argv[2]);
  return 0;
}























