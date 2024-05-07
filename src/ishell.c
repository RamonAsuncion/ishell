#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "wrappers.h"


/*
 * This program implements a simple Unix shell. The shell supports command history, which allows
 * the user to navigate through previously entered commands using the up and down arrow keys.
 *
 * To use the history feature, simply press the up arrow key after typing a few commands. The
 * shell will display the last command you entered. Press the up arrow key again to see the
 * command before that, and so on. You can also press the down arrow key to navigate in the
 * opposite direction.
 *
 * The history is stored in a file called .history in the user's home directory. The shell reads
 * the history from this file when it starts, and writes it back to the file when it exits.
 *
 *
 * Note: I did the challange Problem 4 so you have to use two tabs.
 */


/**
*
*  while(TRUE) {
*   read_command(command, parameters);
*   if (fork() != 0) {
*   parent code
*   waitpid(-1, &status, 0);
*   } else {
*   execve(command, parameters, 0);
*   }
*  }
*/



int main(int argc, char *argv[])
{
  // Bind the tab key to the rl_complete function
  rl_bind_key('\t', rl_complete);

  // Clear the screen with Ctrl + L
  rl_bind_keyseq("\\C-l", rl_clear_screen);

  // Get the home directory
  char *home = getenv("HOME");

  // Initialize the history list
  char history_path[1024];
  sprintf(history_path, "%s/.history", home);
  read_history(history_path);

  while (true) {
    char *command = readline("ishell> ");
    add_history(command);

    if (strcmp(command, "exit") == 0) break;

    if (strchr(command, ';') != NULL) {
      char *cmd = strtok(command, ";");
      char **cmds = malloc(sizeof(char*));
      int i = 0;
      while (cmd != NULL) {
        cmds[i] = cmd;
        cmd = strtok(NULL, ";");
        cmds = realloc(cmds, (i+2) * sizeof(char*));
        i++;
      }
      cmds[i] = NULL;

      for (int j = 0; j < i; j++) {
        char *arg = strtok(cmds[j], " \n");
        char **args = malloc(sizeof(char*));
        int k = 0;
        while (arg != NULL) {
          args[k] = arg;
          arg = strtok(NULL, " \n");
          args = realloc(args, (k+2) * sizeof(char*));
          k++;
        }
        args[k] = NULL;

        pid_t pid = Fork();
        if (pid == 0) {
          Execvp(args[0], args);
        } else {
          int status;
          Wait(&status);
          if (WEXITSTATUS(status) == 0) {
            printf("[ishell: program terminated successfully]\n");
          } else {
            printf("[ishell: program terminated abnormally %d]\n", WEXITSTATUS(status));
          }
        }
      }
    } else {
      char *arg = strtok(command, " \n");
      char **args = malloc(sizeof(char*));
      int i = 0;
      while (arg != NULL) {
        args[i] = arg;
        arg = strtok(NULL, " \n");
        args = realloc(args, (i+2) * sizeof(char*));
        i++;
      }
      args[i] = NULL;

      pid_t pid = Fork();
      if (pid == 0) {
        Execvp(args[0], args);
      } else {
        int status;
        Wait(&status);
        if (WEXITSTATUS(status) == 0) {
          printf("[ishell: program terminated successfully]\n");
        } else {
          printf("[ishell: program terminated abnormally %d]\n", WEXITSTATUS(status));
        }
      }
    }

    free(command);
  }

  return 0;
}


