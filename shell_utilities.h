#ifndef SHELL_UTILITIES_H
#define SHELL_UTILITIES_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>


#define MAX_PROMPT 1024

void printPrompt();
char* readPrompt();
int splitLine(char *prompt, char *tokens[]);
int processPrompt(char *prompt, char *tokens[]);


#endif






