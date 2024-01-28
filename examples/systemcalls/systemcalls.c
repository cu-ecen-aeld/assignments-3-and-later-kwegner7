#include "systemcalls.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{
  int ret;
  ret = system(cmd);
  if (ret == 0)
    return true;
  else
    return false;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
  /*
   * Read in the string arguments that represent a command
   * These are placed in the string array command[]
   */
  va_list args;
  va_start(args, count);
  char* command[count+1]; // arguments as strings
  int i;
  printf("\ndo_exec\n");
  for(i=0; i<count; i++)
  {
      command[i] = va_arg(args, char *);
      if (true) printf("  %d %s\n", i, command[i]);
  }
  command[count] = NULL;
  printf("\n");
  va_end(args);

  //
  // check that all arguments are correct
  //
  const char firstCharCommand = command[0][0];
  if (firstCharCommand != '/')
  {
    printf("ERROR: Path is not absolute, begins with: %c\n", firstCharCommand);
    return false;
  }

  /*
   * Execute a fork to create a child process that replicates the parent
   * You may notice printf() output duplicated after your fork() call.  
   * Use fflush(stdout) before the fork() call to avoid duplicate prints.
   */
  fflush(stdout);
  pid_t pid = fork();

  /*
   * If the fork is not successful, then return false so that the parent
   * can handle the situation
   */
  if (pid == -1)
    return false;

  /*
   * The child is running.
   * The value 0 is returned in the child if fork was successful
   */
  if (pid == 0) 
  { 
    if (false)
      printf("Parent PID: %d, Child PID: %d\n", getppid(), getpid());

    /*
     * This code is running in the child process. The child process currently
     * is a duplication of the parent. We want to replace parent behavior
     * with the command specified in command[] using exec().
     */

    /*
     * the command must be absolute
     */
    const char firstCharCommand = command[0][0];
    if (firstCharCommand != '/')
    {
      printf("ERROR: Path is not absolute, begins with:%c\n", firstCharCommand);
    }

    /*
     * replace the parent process with this new binary executable
     */
    const int returnCode = execv (command[0], command);
    printf("RETURN CODE FROM execf is %d\n", returnCode);
    if (returnCode == -1)
    {
      return false;
    }

    /*
     * The child process should never return. It will terminate when
     * it has completed its mission.
     */
  }

  /*
   * This code is running in the parent process. The 2 processes are
   * running in parallel. The parent must wait for the child to terminate.
   */
  int status;
  const int returnCode = waitpid (pid, &status, 0);
  if (false) printf("Return code: %d, status: %d\n", returnCode, status);
  if ((returnCode == -1) || (status != 0))
  {
    printf ("ERROR WHILE WAITING %d %d %d\n", pid, getppid(), getpid());
    return false;
  }

  const bool childTerminatedNormally = WIFEXITED(status);
  if (childTerminatedNormally)
  {
    printf ("CHILD TERMINATED NORMALLY %d %d %d\n", pid, getppid(), getpid());
    return true;
  }
  return false;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    printf("\ndo_exec_redirect\n");
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
        if (true) printf("  %d %s\n", i, command[i]);
    }
    command[count] = NULL;
    printf("\n");
    va_end(args);

    //
    // check that all arguments are correct
    //
    const char firstCharCommand = command[0][0];
    if (firstCharCommand != '/')
    {
      printf("ERROR: Path is not absolute, begins with: %c\n", firstCharCommand);
      return false;
    }

    /*
    * Fork and exec in the child process
    */
    fflush(stdout);
    pid_t pid;

    //
    // open the file for writing
    //
    int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
    if (fd < 0) 
    {
       perror("open"); 
       abort();
    }

    //
    // fork a new child process
    //
    switch (pid = fork()) 
    {
      case -1: 
        perror("fork"); abort();

      case 0:
        //
        // child process is running
        // copy the file descriptor so that stdout points to the file in file table
        //
        if (dup2(fd, STDOUT_FILENO) < 0)
        { 
          perror("dup2"); 
          abort();
        }
        close(fd); // not needed since STDOUT_FILENO points correctly

        //
        // exec() the new child process
        //
        const int returnCode =  execv(command[0], command);
        printf("RETURN CODE FROM execv is %d\n", returnCode);
        if (returnCode == -1)
        {
          return false;
        }

        // should not get here
        perror("execvp");
        abort();

      default:
        close(fd);
        /* do whatever the parent wants to do. */
    }

    /*
    * Parent waits
    */
    int status;
    if (waitpid (pid, &status, 0) == -1)
    {
      return false;
    }

    const bool childTerminatedNormally = WIFEXITED(status);
    if (childTerminatedNormally)
    {
      printf ("CHILD TERMINATED NORMALLY EXIT STATUS %d\n", status);
      return true;
    }
    return false;
}






#if 0
================================================================================
OBSOLETE
================================================================================
      // works except for redirected3.txt
      if (false) // TODO KURT
      {
        // execl( "/bin/sh", "/bin/sh", "-c", commandRedirect, (char *)NULL );
        char run[100] = "/usr/bin/echo NARY RAYNA LEVI > ";
        strcat(run, outputfile);


        execl( "/bin/sh", "/bin/sh", "-c", run, (char *)NULL );
      }

  // success =  do_exec_redirect("./redirected3.txt", 3, "/bin/sh", "-c", "echo home is $HOME");

      // using dup2
      if (false) // TODO KURT
      {
        int fd = open(outputfile, O_CREAT | O_TRUNC | O_WRONLY);
        dup2(fd, STDOUT_FILENO);
        close(fd);
        execl( "/bin/sh", "/bin/sh", "-c", commandRedirect, (char *)NULL );
      }

        // execv (command[0], command);

        char run1[100] = "/usr/bin/echo NARY RAYNA LEVI KAYLA > ";
        strcat(run1, outputfile);

        char run[100] = "/usr/bin/cat file1.txt > ";
        strcat(run, outputfile);

        #if 0
        char * commandNew[5];
        commandNew[0] = "/bin/sh";
        commandNew[1] = "/bin/sh";
        commandNew[2] = "-c";
        commandNew[3] = "\"/usr/bin/echo DDD EEE FFF\"";
        commandNew[4] = NULL;
        for(i=0; i<5-1; i++)
        {
          printf("%s ", commandNew[i]);
        }
        printf("\n");

        execv (commandNew[0], commandNew);
        #endif

    #if 0
    pid_t pid = fork();
    if (pid == -1) return false;

    // now in the child
    if (pid == 0)
    { 
      if (false)
      {
        char idea[50] = "/bin/sh -c \"echo home is $HOME\" > ";
        strcat(idea, outputfile);

        char useThis[50];
        strcpy(useThis, commandRedirect);

        if (strcmp(command[0], "/bin/sh") == 0)
            strcpy(useThis, idea); 

        /*
         * the command must be absolute
         */
        const char firstCharCommand = commandRedirect[0];
        if (firstCharCommand != '/')
        {
          printf("ERROR: Path is not absolute, begins with:%c\n", firstCharCommand);
        }

        // "The $HOME parameter should be expanded when using /bin/sh with do_exec()
        // "The variable $HOME should not be expanded using execv()

        // I see, do not use /bin/sh !!!!!!!!!!!!!!!!
        // pass the correct command
        char* commandExecv[] = { "/bin/sh", "-c", useThis, NULL };
        //execv (commandExecv[0], commandExecv);

        execl( "/bin/echo", "/bin/echo home is $HOME > ./redirected4.txt", NULL);
      }

    #endif

    /*************************************************************
    * combine each part of the command into a single string
    * and redirect to the output file
    **************************************************************/
    char commandRedirect[200];   
    strcpy(commandRedirect, command[0]);
    for(i=1; i<count; i++)
    {
        if (false) printf("%s\n", command[i]);
        strcat(commandRedirect, " ");
        strcat(commandRedirect, command[i]);
    }
    strcat(commandRedirect, " > ");
    strcat(commandRedirect, outputfile); 

    printf("COMMAND: %s\n", commandRedirect);


        char* cmnd[] = {"/bin/echo", "home is $HOME", NULL};








#endif
