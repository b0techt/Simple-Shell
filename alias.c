#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "list.h"

// takes a potential alias, check if it is an alias, if so, sets the corresponding command. Otherwise return code non-zero
int getAliasCommand(char *alias, char *command, struct aliasListPair *aliasList, int aliasIndex) {
	int i = 0;
	int j = 0;
	int k = 0;

	// aliasIndex must not be greater than 10. If aliasIndex is 10 then the aliasList is full
	if (aliasIndex > 10)
		aliasIndex = 10;

	// Look for alias
	for (i = 0; i < aliasIndex; i++) {
		if (strcmp(aliasList[i].aliasName, alias) == 0) {
			strcpy(command, aliasList[i].commandName);

			for (j = 0; j < aliasIndex; j++) {		// check if alias is an alias of an alias
				if (strcmp(aliasList[j].aliasName, command) == 0) {
					strcpy(command, aliasList[j].commandName);

					for (k = 0; k < aliasIndex; k++) {		// check if alias is an alias of an alias of an alias
						if (strcmp(aliasList[k].aliasName, command) == 0) {
							strcpy(command, aliasList[k].commandName);
							return 0;
						}
					}
				}
			}
			return 0;
		}
	}
	strcpy(command, ""); // if alias is not found then set command to be the null string
	return 1;
}

// remove an alias from aliasList, updating aliasIndex.  If alias doesn't exist in list, return code non-zero
int removeAlias(char *alias, struct aliasListPair *aliasList, int *aliasIndex) {
	int i = 0;
	int j = 0;

	// aliasIndex must not be greater than 10. If aliasIndex is 10 then the aliasList is full
	if (*aliasIndex > 10)
		*aliasIndex = 10;

	// Look for alias
	for (i = 0; i < (*aliasIndex); i++) {
		if (strcmp(aliasList[i].aliasName, alias) == 0) {
			for (j = i; j < (*aliasIndex) - 1; j++) {	// if removing alias then shuffle everthing down one place
				strcpy(aliasList[j].aliasName, aliasList[j + 1].aliasName);
				strcpy(aliasList[j].commandName, aliasList[j + 1].commandName);
			}
			(*aliasIndex)--;
			return 0;
		}
	}
	return 1;
}

// add alias to aliasList with the given command name, updating aliasList and aliasIndex. Returns code non-zero if can't add alias
int addAlias(char *alias, char *command, struct aliasListPair *aliasList, int *aliasIndex) {
	int i = 0;

	// aliasIndex must not be greater than 10. If aliasIndex is 10 then the aliasList is full and can't add alias
	if (*aliasIndex >= 10) {
		*aliasIndex = 10;
		return 1;
	}

	// If alias already exists then return non-zero code
	for (i = 0; i < (*aliasIndex); i++) {
		if (strcmp(aliasList[i].aliasName, alias) == 0) {		// compares string to everthing in alias table
			printf("Alias %s exists already\n", alias);
			return 1;
		}
	}

	// Add alias to list
	strcpy(aliasList[(*aliasIndex)].aliasName, alias);
	strcpy(aliasList[(*aliasIndex)].commandName, command);

	// And increment aliasIndex
	(*aliasIndex)++;
	return 0;
}

// Print alias list
void printAlias(struct aliasListPair *aliasList, int aliasIndex) {
	int i = 0;

	// aliasIndex must not be greater than 10. If aliasIndex is 10 then the aliasList is full
	if (aliasIndex > 10)
		aliasIndex = 10;

	if (aliasIndex <= 0) {
		printf("printAlias: No aliases\n");
		return;
	}

	for (i = 0; i < aliasIndex; i++) {
		printf("Alias = %s; Command = %s\n", aliasList[i].aliasName, aliasList[i].commandName);
	}
}

// writes aliases to .aliases file.
void saveAliasFile(struct aliasListPair *aliasList, int aliasIndex) {
	int i = 0;
	char alias_list_pname[512];

	strcpy(alias_list_pname, getenv("HOME"));
	strcat(alias_list_pname, "/.aliases");

	// open alias file for writing
	FILE *file = fopen(alias_list_pname, "w");

	// check .alias_file opens
	if (file == NULL) {
		printf("Can't create .aliases file \n");
	}
	else {
		for (i = 0; i < aliasIndex; i++) { // for each alias line, print to file
			fprintf(file, "%s %s\n", aliasList[i].aliasName, aliasList[i].commandName);
		}
		fclose(file);
	}
}

/* Read aliases from .aliases file and initialise the aliasList data structure with them.
 * Returns the index to the next free record.
 * Returns 0 if it can't read any records from the alias file
*/
int loadAliasFile(struct aliasListPair *aliasList) {
	int i = 0; 		// Start with index meaning no record in aliasList table.
	char alias_list_pname[512];

	strcpy(alias_list_pname, getenv("HOME"));
	strcat(alias_list_pname, "/.aliases");

	FILE *file = fopen(alias_list_pname, "r");

	// check .alias_file exists
	if (file == NULL) {
		return 0;
	}

	// while not end of file, read each line
	while (i < 10 && (fscanf(file, "%s %[^\n]", aliasList[i].aliasName, aliasList[i].commandName) == 2)) { // read number and command
		i++;
	}

	fclose(file);
	return i; // return index to the next free record
}
