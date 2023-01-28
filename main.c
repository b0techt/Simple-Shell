#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#include "list.h"

/*
 * Authors
 * Stage 1: Andrew Richmond, Matej Vrecar and Emmanuel Onanuga
 * Stage 2: Andrew Richmond, Matej Vrecar and Emmanuel Onanuga
 * Stage 3: Andrew Richmond, Matej Vrecar, Emmanuel Onanuga and Chris Holland
 * Stage 4: Andrew Richmond, Matej Vrecar, Emmanuel Onanuga and Chris Holland
 * James joined after Stage 4
 * Stage 5: Andrew Richmond, Matej Vrecar, Emmanuel Onanuga, Chris Holland and James McCafferty
 * Stage 6: Andrew Richmond, Matej Vrecar and James McCafferty
 * Stage 7: James McCafferty
 * Stage 8: James McCafferty
 * Stage 9: James McCafferty
 */

int main(void) {

    // Find the user home directory from the environment & set current working directory to user home directory
    chdir(getenv("HOME"));

    // Save the current (original) path
    char originalPath[512];
    strcpy(originalPath, getenv("PATH"));

    // Load aliases
    struct aliasListPair aliasList[10];     // aliasList consists of an array of two strings: the alias, and the matching command
    int aliasIndex = 0;                     // aliasIndex points to the first un-used slot in the alias list
    aliasIndex = loadAliasFile(aliasList);

    // Global variables
    int historyIndex = -1;      // Index, which points to last entry in the history table, with allowed values -1...19. historyIndex of 19 means table full and histIndex of -1 means nothing in the table.
    char *history[20] = {NULL};

    // Allocates memory for History
    for (int i = 0; i < 20; i++) {
        history[i] = malloc(512 * sizeof(char));
        *history[i] = '\0';
    }

    // Load History
    historyIndex = loadHistoryFile(history);
    if (historyIndex < 0) {
        printf("Main: Can't load history file\n");
    }

    // Do while shell has not terminated
    int exit = 0;                   // Controls state of the while loop

    while (exit != 1) {
        char inputString[512];      // Holds user input
        char *tokenArray[50];       // Holds input split into tokens
        int internalProcess = 0;    // 1 if the process is internal
        int historyResult = 1;      // 0 if bad history invocation, 1 if good history invocation, 2 if the invocation is of "history"
        int tokenised = 0;          // Holds state of tokenisation
        char commandString[512];    // For holding the command string after looking up an alias

        // Display prompt
        printCWD();
        printf(">> ");

        // Read and parse user input
        if (fgets(inputString, 512, stdin) == NULL) {
            exit = 1;
        }

        // Execute if input != enter key
        else if (strcmp("\n", inputString)) { 

            // While the command is a history invocation or alias then replace it with the appropriate command from history or the aliased command respectively add inputted string to history
            historyResult = getHistoryCommand(history, historyIndex, inputString); // If command starts with a '!' then get the invoked command

            if (historyResult == 1) {       // Write command to into history but only if it's not an invoked "history" command
                historyIndex = updateHistory(history, historyIndex, inputString);
            }

            // Tokenise inputStrings
            if (historyResult != 0) {
                tokenise(tokenArray, inputString);
            }

            // If tokenArray[0] is an alias then substitute it with the real command
            if (getAliasCommand(tokenArray[0], commandString, aliasList, aliasIndex) == 0) {
                for (int i = 0; i < 50; i++) {              // Wipe tokenArray
                    tokenArray[i] = NULL;
                }
                tokenise(tokenArray, commandString);        // Retokenise commandString incase commandString includes parameters as well as a command
            }

            // Exit condition
            if (historyResult != 0) {
                exit = checkExit(tokenArray[0]);
            }

            // If command is built-in invoke appropriate function
            if (tokenised != 1 && historyResult != 0) {
                internalProcess = checkInternal(tokenArray[0]);     // Checks if input = internal function
            }

            // Executes internal process if == 1
            if (exit == 0 && internalProcess == 1 && historyResult != 0) {
                execute(tokenArray[0], tokenArray[1], tokenArray[2], tokenArray[3], history, historyIndex, aliasList, &aliasIndex);
            }

            // Else execute command as an external process
            else if (exit == 0 && internalProcess == 0 && historyResult != 0) {
                pid_t pid;
                pid = fork();

                // Fork error
                if (pid < 0) {
                    fprintf(stderr, "Fork Failed");
                    return 1;
                }

                // Child process
                else if (pid == 0) {
                    if (execvp(tokenArray[0], &tokenArray[0]) == -1) {   // Error case
                        perror(tokenArray[0]);
                        _exit(1);
                    }
                    // Else execute command as external process
                    else {
                        execvp(tokenArray[0], &tokenArray[0]);
                    }
                }

                // Parent process
                else {
                    wait(NULL);
                }
            }
        }
    // End while
    } 

    // Exit message
    printf("Program exited!\n");

    // Save History
    saveHistoryFile(history, historyIndex);

    // Save aliases
    saveAliasFile(aliasList, aliasIndex);

    // Restore original path & print
    setenv("PATH", originalPath, 1);
    printf("Printing original path... \n");
    printf("%s\n", getenv("PATH"));

    return 0;
}
