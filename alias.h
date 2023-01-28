#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// aliasList consists of 2 arrays of matching strings: the alias, and the matching command.
struct aliasListPair {
    char aliasName[512];
    char commandName[512];
};

// Takes a potential alias, check if it is an alias, if so, sets the corresponding command. Otherwise return code non-zero
int getAliasCommand(char *alias, char *command, struct aliasListPair *aliasList, int aliasIndex);

// Remove an alias from aliasList, updating aliasIndex.  If alias doesn't exist in list, return code non-zero
int removeAlias(char *alias, struct aliasListPair *aliasList, int *aliasIndex);

// Add alias to aliasList with the given command name, updating aliasList and aliasIndex. Returns code non-zero is can't add alias
int addAlias(char *alias, char *command, struct aliasListPair *aliasList, int *aliasIndex);

// Print alias list
void printAlias(struct aliasListPair *aliasList, int aliasIndex);

// Writes aliases to .aliases file.
void saveAliasFile(struct aliasListPair *aliasList, int aliasIndex);

/* Read aliases from .aliases file and initialises the aliasList data structure with them.
 * Returns the index to the next free record (aliasIndex)
 * Returns 0 if it can't read any records from the alias file
 */
int loadAliasFile(struct aliasListPair *aliasList);