#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "alias.h"
#include "history.h"

void tokenise(char **tokenArray, char *inputString);

int checkInternal(char *tok1);

int checkExit(char *inputString);

void execute(char *tok1, char *tok2, char *tok3, char *tok4, char **history, int historyIndex, struct aliasListPair *aliasList, int *aliasIndex);

void getpath();

void setpath(char *path);

void cd();

void cdDir(char *dir);

void printHistory(char **history, int historyIndex);

void printCWD();