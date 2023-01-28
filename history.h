#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int getHistoryCommand(char **history, int historyIndex, char *inputstring);

int loadHistoryFile(char **history);

int updateHistory(char **history, int historyIndex, char *inputstring);

void saveHistoryFile(char **history, int historyIndex);
