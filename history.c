#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "history.h"

/* check if the inputString is a history invocation, and if so, update inputstring with the invoked command.
 * returns 0 if bad history
 * returns 1 if history invocation is OK.
 * returns 2 if history invocation is OK and is the history command itself.
 */
int getHistoryCommand(char **history, int historyIndex, char *inputString) {
    int invoke;     // <n> for history invocation

    // Is this an Invoke command for history?
    if (inputString[0] == '!') {
        if (historyIndex < 0) { // If nothing in history table
            printf("GetHistoryCommand: no history available\n");
            return 0;
        }
        if (inputString[1] == '!') {
            strcpy(inputString, history[historyIndex]);
            if (strcmp("history\n", inputString) == 0)
                return 2;
            else
                return 1;
        }
        // Read number after ! can be positive or negative
        else if (sscanf(inputString, "!%d", &invoke) == 1) {
            if (invoke < 0 && invoke > -20 && invoke + historyIndex >= 0) {
                strcpy(inputString, history[historyIndex + 1 + invoke]);
                if (strcmp("history\n", inputString) == 0)
                    return 2;
                else
                    return 1;
            }
            else if (invoke > 0 && invoke <= 20 && invoke <= historyIndex + 1) {
                strcpy(inputString, history[invoke - 1]);
                if (strcmp("history\n", inputString) == 0)
                    return 2;
                else
                    return 1;
            }
            else {
                printf("GetHistoryCommand: error getting history\n");
                return 0;
            }
        }
        else {
            printf("GetHistoryCommand: error getting history\n");
            return 0;
        }
    }
    return 1;
}

/* read history from .hist_list file and initialise the history data structure with them.
 * returns the index to the last record it has read.
 */
int loadHistoryFile(char **history) {
    int i = -1;     // Start with index meaning no record in history table.
    int j = 0;
    char temp[512];
    char hist_list_pname[512];

    strcpy(hist_list_pname, getenv("HOME"));
    strcat(hist_list_pname, "/.hist_list");

    FILE *file = fopen(hist_list_pname, "r");

    // Check .hist_file exists
    if (file == NULL) {
        return -1;
    }

    // While not end of file, read each line
    while (i < 20 && (fscanf(file, "%d %[^\n]", &j, temp) == 2)) { // Read number and command
        i++;
        sprintf(history[i], "%s\n", temp); // Update history data structure
    }
    fclose(file);
    return i; // Return index to the last record read
}

// Writes history to .hist_list file.
void saveHistoryFile(char **history, int historyIndex) {
    int i = 0;
    char hist_list_pname[512];

    strcpy(hist_list_pname, getenv("HOME"));
    strcat(hist_list_pname, "/.hist_list");

    // Open history file for writing
    FILE *file = fopen(hist_list_pname, "w");

    // Check .hist_file opens
    if (file == NULL) {
        printf("Can't create .hist_file \n");
    }
    else {
        for (i = 0; i <= historyIndex; i++) // for each history line, print to file
            if (*history[i] != '\0')
                fprintf(file, "%d %s", i + 1, history[i]);
    }
    fclose(file);
}

/* copy the inputString into the history table, and update the historyIndex
 * so that we only hold upto 20 history items
 * Returns the updated historyIndex for the updated history table
 */
int updateHistory(char **history, int historyIndex, char *inputString) {

    // If nothing in history table, then put inputstring into entry zero
    if (historyIndex < 0) {
        strcpy(history[0], inputString);
        return 0;
    }

    // If we've reached the max number of history items, shuffle everything down one place
    if (historyIndex == 19) {
        for (int i = 0; i < historyIndex; i++) {
            strcpy(history[i], history[i + 1]);
        }
    }
    else
        historyIndex++;

    strcpy(history[historyIndex], inputString);
    return historyIndex;
}