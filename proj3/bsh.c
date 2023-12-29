#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

//accept up to 16 command-line arguments
#define MAXARG 16

//allow up to 64 environment variables
#define MAXENV 64

//keep the last 500 commands in history
#define HISTSIZE 500

//accept up to 1024 bytes in one command
#define MAXLINE 1024

// this is a variable defined by unistd.h and is a variable declared at runtime
// It allows the access and manipulation of environment variables
// this is only a declaration and not a definition
extern char **environ; 

// history array
char *history[HISTSIZE];
int histCount = 0;

char *buildString(char *args[]) {
    // Calculate the total length of the command line
    int totalLength = 0;
    for (int i = 0; args[i] != NULL; ++i) {
        totalLength += strlen(args[i]) + 1; // +1 for space between arguments
    }

    // Allocate memory for the command line string
    char *cmdString = (char *)malloc(totalLength);
    if (cmdString == NULL) {
        perror("buildString: cmdString is NULL");
        exit(1);
    }

    // Build the command line string
    int offset = 0;
    for (int i = 0; args[i] != NULL; ++i) {
        strcpy(cmdString + offset, args[i]);
        offset += strlen(args[i]);
        cmdString[offset++] = ' '; // Add space between arguments
    }

    // Remove the trailing space
    cmdString[offset - 1] = '\0';

    return cmdString;
}

static char **parseCmd(char cmdLine[]) {
  char **cmdArg, *ptr;
  int i;

  //(MAXARG + 1) because the list must be terminated by a NULL ptr
  cmdArg = (char **) malloc(sizeof(char *) * (MAXARG + 1));
  if (cmdArg == NULL) {
    perror("parseCmd: cmdArg is NULL");
    exit(1);
  }
  for (i = 0; i <= MAXARG; i++) //note the equality
    cmdArg[i] = NULL;
  i = 0;
  
  // Updated to allow function of parsing user commands without spaces
  while ((ptr = strsep(&cmdLine, " \t|<>")) != NULL) {
        // Skip empty tokens
        if (strlen(ptr) > 0) {
            cmdArg[i] = (char *)malloc(sizeof(char) * (strlen(ptr) + 1));
            if (cmdArg[i] == NULL) {
                perror("parseCmd: cmdArg[i] is NULL");
                exit(1);
            }
            strcpy(cmdArg[i++], ptr);
            if (i == MAXARG)
                break;
        }
    }
    return cmdArg;

}

// Function to search for the executable in the directories listed in PATH
char *findExecutable(char *command, char *paths) {
    char *pathCopy = strdup(paths);  // Make a copy of PATH to avoid modifying the original
    char *token, *executablePath = NULL;

    // Iterate through the directories in PATH
    while ((token = strsep(&pathCopy, ":")) != NULL) {
        // Construct the absolute path by appending the command to the current directory
        char *userPath = (char *)malloc(strlen(token) + strlen(command) + 2);  // +2 for '/' and '\0'
        sprintf(userPath, "%s/%s", token, command);

        // Check if the constructed path is a valid executable
        if (access(userPath, X_OK) == 0) {
            executablePath = userPath;
            break;
        }

        free(userPath);
    }

    free(pathCopy);  // Free the copy of PATH

    return executablePath;
}

int main(int argc, char *argv[]) {
  char cmdLine[MAXLINE], **cmdArg;
  int status, i, debug;
  pid_t pid;
  char *environment[MAXENV]; // array of strings
  int envVarCount = 0; // environment variable counter

  // this is an array to hold the environment variables in the environ variable declared at runtime
  // iterates through the envrion array until the NULL character is hit, or envVarCount reaches the limit
  for (char **env = environ; *env != NULL && envVarCount < MAXENV; ++env){
    // strdup duplicates a string env and sets it to an index of the environment array
    environment[envVarCount++] = strdup(*env);
  }

  debug = 0;
  i = 1;
  while (i < argc) {
    if (! strcmp(argv[i], "-d") )
      debug = 1;
    i++;
  }
  while (( 1 )) {
    printf("bsh> ");                      //prompt
    fgets(cmdLine, MAXLINE, stdin);       //get a line from keyboard
    cmdLine[strlen(cmdLine) - 1] = '\0';  //strip '\n'
    cmdArg = parseCmd(cmdLine);

    //adding command to history
    if (histCount < HISTSIZE){
      history[histCount] = buildString(cmdArg);
      histCount++;
    }
    else {
      free(history[0]);
      for (int i = 0; i <= HISTSIZE; i++){
        history[i] = history[i + 1];
      }
      history[HISTSIZE] = buildString(cmdArg);
    }

    if (debug) {
      i = 0;
      while (cmdArg[i] != NULL) {
        printf("\t%d (%s)\n", i, cmdArg[i]);
        i++;
      }
    }

    //built-in command exit
    if (strcmp(cmdArg[0], "exit") == 0) {
      if (debug)
	printf("exiting\n");
      break;
    }
    //built-in command env
    else if (strcmp(cmdArg[0], "env") == 0) {
      // prints out the copied environment variables
      for (int i = 0; i < envVarCount; i++) {
        if (strlen(environment[i]) > 0){
          printf("%s\n", environment[i]);        
        }
      }
    }
    //built-in command setenv
    else if (strcmp(cmdArg[0], "setenv") == 0) {
    if (cmdArg[1] != NULL && cmdArg[2] != NULL) {
        // Search for the variable
        int found = 0;
        for (int i = 0; i < envVarCount; i++) {
            if (strncmp(environment[i], cmdArg[1], strlen(cmdArg[1])) == 0) {
                // Variable found, free old value
                free(environment[i + 1]);

                // Concatenate variable name and value
                char *newEntry = malloc(strlen(cmdArg[1]) + strlen(cmdArg[2]) + 2); // +2 for '=' and null terminator
                sprintf(newEntry, "%s=%s", cmdArg[1], cmdArg[2]);

                // Allocate space for new value
                environment[i] = strdup(cmdArg[2]);
                found = 1;
                break;
            }
        }

        // If the variable is not found, add it
        if (!found && envVarCount < MAXENV - 1) {
            // Concatenate variable name and value
            char *newEntry = malloc(strlen(cmdArg[1]) + strlen(cmdArg[2]) + 2); // +2 for '=' and null terminator
            sprintf(newEntry, "%s=%s", cmdArg[1], cmdArg[2]);

            // Save the new entry
            environment[envVarCount++] = newEntry;
            envVarCount++; // Increment for the value entry
        }
    }
}

    //built-in command unsetenv
    else if (strcmp(cmdArg[0], "unsetenv") == 0) {
        if (cmdArg[1] != NULL) {
            for (int i = 0; i < envVarCount; i++) {
                if (strncmp(environment[i], cmdArg[1], strlen(cmdArg[1])) == 0) {
                    // If variable exists then we delete it
                    free(environment[i]);

                    // Shift the remaining variables
                    for (int j = i; j < envVarCount - 2; j += 2) {
                        environment[j] = environment[j + 2];
                        environment[j + 1] = environment[j + 3];
                    }

                    // Set the last two entries to NULL
                    environment[envVarCount - 2] = NULL;
                    environment[envVarCount - 1] = NULL;

                    envVarCount -= 2;
                    break;
                }
            }
        } else {
            printf("Usage: unsetenv <variable>\n");
        }
    }

    //built-in command cd
    else if (strcmp(cmdArg[0], "cd") == 0) {
      char *newDir;

      // If no arguments, go to the home directory
      if (cmdArg[1] == NULL) {
          newDir = getenv("HOME");
      } else {
          newDir = cmdArg[1];
      }

      // Use chdir to change the working directory
      if (chdir(newDir) != 0) {
          perror("cd");
      } else {
          // Update the environment variable PWD
          char *cwd = getcwd(NULL, 0);
          setenv("PWD", cwd, 1);
          free(cwd);
      }
    }
    //built-in command history
    else if (strcmp(cmdArg[0], "history") == 0) {
      for (int i = 0; i < histCount; i++){
        printf("%d: %s\n", i + 1, history[i]);
      }
    }
    // built-in command pwd
    else if (strcmp(cmdArg[0], "pwd") == 0) {
      char *cwd = getcwd(NULL, 0);
      if (cwd != NULL) {
          printf("%s\n", cwd);
          free(cwd);
      } else {
          perror("pwd");
      }
    } 

    else if (strcmp(cmdArg[0], "pipe") == 0) {
            // Pipe command
            if (cmdArg[1] != NULL && cmdArg[2] != NULL) {
                int pipefd[2];
                if (pipe(pipefd) == -1) {
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }

                pid_t pid1, pid2;
                int status1, status2;

                // First command
                pid1 = fork();
                if (pid1 == 0) {
                    // Child process: set up output redirection to the write end of the pipe
                    close(pipefd[0]); // Close read
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[1]); // Close write

                    // Execute the first command
                    execvp(cmdArg[1], cmdArg + 1);
                    perror("execvp");
                    exit(EXIT_FAILURE);
                } else if (pid1 > 0) {
                  // this is copy/paste of the above
                    // Parent process

                    // Wait for the first command to finish
                    waitpid(pid1, &status1, 0);

                    // Second command
                    pid2 = fork();
                    if (pid2 == 0) {
                        // Child process: set up input redirection from the read end of the pipe
                        close(pipefd[1]);
                        dup2(pipefd[0], STDIN_FILENO);
                        close(pipefd[0]);

                        // Execute the second command
                        execvp(cmdArg[2], cmdArg + 2);
                        perror("execvp");
                        exit(EXIT_FAILURE);
                    } else if (pid2 > 0) {
                        // Parent process

                        // Close both ends of the pipe
                        close(pipefd[0]);
                        close(pipefd[1]);

                        // Wait for the second command to finish
                        waitpid(pid2, &status2, 0);
                    } else {
                        perror("fork");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
            } else {
                printf("Usage: pipe <command1> <command2>\n");
            }
        }

    //implement how to execute Minix commands here
    // This may work? I have no idea how to test it
    else {
      // Check if the command is in the PATH and execute if found
      char *executablePath = findExecutable(cmdArg[0], getenv("PATH"));
      if (executablePath != NULL) {
          pid = fork();
          if (pid != 0) {
              waitpid(pid, &status, 0);
          } else {
              // Execute the command in the child process
              status = execv(executablePath, cmdArg);
              if (status) {
                  printf("\tno such command (%s)\n", cmdArg[0]);
                  return 1;
              }
          }

          free(executablePath);
      }
    }

    //clean up before running the next command
    i = 0;
    while (cmdArg[i] != NULL)
      free( cmdArg[i++] );
    free(cmdArg);
  }

  // Clean up history array
  for (int i = 0; i < HISTSIZE; ++i) {
      free(history[i]);
  }

  return 0;
}
