#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "list.h"

// prints Current Working Directory next to prompt
void printCWD() {
	char s[512];
	printf("%s ", getcwd(s, 512));
}

// Tokenise function, returns inputstring as tokens
void tokenise(char **tokenArray, char *inputString) {
	char delimiters[10] = " \t\n|><&;";
	char *token;
	int index = 0;

	token = strtok(inputString, delimiters);
	tokenArray[index] = token;
	index++;

	while (token != NULL) {
		token = strtok(NULL, delimiters);
		tokenArray[index] = token;
		index++;
	}
}

// exit condition. Returns 1 if input is exit command and returns 0 otherwise.
int checkExit(char *inputString) {
	int compare;
	compare = strcmp("exit", inputString);
	if (compare == 0) {
		return 1;
	}
	else {
		return 0;
	}
}

// checks if the the first token is an internal command, helper for exec
int checkInternal(char *tok1) {
	if (strcmp(tok1, "getpath") == 0 ||
		strcmp(tok1, "setpath") == 0 ||
		strcmp(tok1, "cd") == 0 ||
		strcmp("history", tok1) == 0 ||
		strcmp("alias", tok1) == 0 ||
		strcmp("unalias", tok1) == 0) {
		return 1;
	}
	else {
		return 0;
	}
}

// getpath command
void getpath() {
	printf("%s", getenv("PATH"));
}

// setpath command
void setpath(char *path) {
	if (setenv("PATH", path, 1) == -1) {
		printf("error\n");
	}
	else if (setenv("PATH", path, 1) == 0) {
		setenv("PATH", path, 1);
	}
}

// cd on its own, changes directory to home
void cd() {
	chdir(getenv("HOME"));
}

// cd dir, when cd is provided with a directory, changes to the specified directory
void cdDir(char *dir) {
	if (chdir(dir) == -1) {
		perror(dir);
	}
	else {
		chdir(dir);
	}
}

// Prints the elements stored in history
void printHistory(char **history, int historyIndex) {
	for (int i = 0; i <= historyIndex; i++) {
		if (*history[i] != '\0') {
			printf("%d %s", i + 1, history[i]);
		}
	}
}

// executes internal commands
void execute(char *tok1, char *tok2, char *tok3, char *tok4, char **history, int historyIndex, struct aliasListPair *aliasList, int *aliasIndex) {

	// execute getpath
	if (strcmp("getpath", tok1) == 0) {
		if (tok2 != NULL) {
			printf("getpath should not have any parameters, invalid command.\n");
		}
		else {
			getpath();
			printf("\n");
		}
	}

	// execute setpath
	else if (strcmp("setpath", tok1) == 0) {
		if (tok2 == NULL) {
			printf("setpath should have one parameter, invalid command.\n");
		}
		else if (tok3 != NULL) {
			printf("setpath should not have more than one parameter, invalid command.\n");
		}
		else {
			setpath(tok2);
		}
	}

	// execute cd - no arguments, defaults to home directory
	else if ((strcmp("cd", tok1) == 0) && tok2 == NULL) {
		cd();
	}

	// execute cd dir
	else if ((strcmp("cd", tok1) == 0) && tok2 != NULL) {
		if (tok3 != NULL) {
			printf("Error: cd: too many arguments\n");
		}
		else {
			cdDir(tok2);
		}
	}

	// execute history
	else if (strcmp("history", tok1) == 0) {
		if (tok2 != NULL) {
			printf("history should not have any parameters, invalid command.\n");
		}
		else
			printHistory(history, historyIndex);
	}

	// execute alias
	else if (strcmp("alias", tok1) == 0) {
		if (tok2 == NULL) {
			printAlias(aliasList, *aliasIndex);
		}
		else if (tok3 == NULL) {
			printf("Error in alias command\n");
		}
		else {
			char tempParameters[512];
			strcpy(tempParameters, tok3);
			if (tok4 != NULL) {
				strcat(tempParameters, " ");
				strcat(tempParameters, tok4);
			}

			// The alias can be a history invocation!!
			if (getHistoryCommand(history, historyIndex - 1, tempParameters) == 0)
				printf("Not a valid history invocation\n");

			// Finally add alias and parameters to table
			if (addAlias(tok2, tempParameters, aliasList, aliasIndex) != 0) {
				printf("Can't add alias\n"); 	// Add alias returns non-zero value therefore can't add alias
			}
		}
	}

	// Execute unalias
	else if (strcmp("unalias", tok1) == 0) {
		if (tok2 == NULL) {
			printf("unalias requires a parameter\n");
		}
		else {
			if (removeAlias(tok2, aliasList, aliasIndex) != 0)
				printf("Alias does not exist, can't remove alias\n");
		}
	}
}
