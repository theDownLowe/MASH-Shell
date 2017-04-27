// Trevor N. Lowe
// Assignment 0
// TCSS 422: Operating Systems
// Winter 2017

// 1) Write code that captures a user provided strings from the console to
//	collect 3 individual commands and a file name.

// User commands do not exceed 255 chars.
// File name will not exceed 255 chars.
// For each command the max number of arguments including the command itself will not exceed 5.
// An error should be shown if exec fails.
// Does not accept command line arguments.
// Should not wait for each command to complete before next execution.

// 2) Chop individual words from the user provided commands to extract the 
// 	command arguments so they can be provided to exec(). 
//	For example a user may provide "grep -ci the". 
//	This string will be chopped into three strings: 
//	"grep", "-ci", and "the". These strings can be hard coded in an execlp call as follows: 
//	execlp("grep", "-ci", "the", (char *) NULL);
// 	or you can use ececvp() which accepts a pointer to a NULL terminated array
//	of char pointers(char **) Each char pointer points to a null terminated word.

// 3) Implement fork() and wait() successfully with 3 levels of nesting.
//	Without nesting only one fork() would execute at any given time causing all three
//	commands to run sequentially. This would result in a slower "mash".

// 4) Wait for children to finish to allow the parent to gracefully exit.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>

#define CMD_SIZE 255
#define MAX_ARGS 6

int pids[3];

// Reads a line of input and converts to String
char *readLine(void) {
	char * line = NULL;
	ssize_t size = 0;
	getline(&line, &size, stdin);
	return line;
}

// Tokenizes the given command and adds file to end.
char **getTokens(char *command, char *file) {

	char **tokens;
	int i = 0;
	
	// Allocation
	tokens = (char**)malloc(sizeof(char*) * MAX_ARGS);
	tokens[0] = (char*)malloc(sizeof(char) * CMD_SIZE * MAX_ARGS);

	for (i = 0; i < MAX_ARGS; i++) {
		tokens[i] = (*tokens + CMD_SIZE * i);
	}
	
	// Tokenizing
	i = 0;
	tokens[i] = strtok(command, " \n");
	while (tokens[i] != NULL && i < MAX_ARGS) {
		i++;
		tokens[i] = strtok(NULL, " \n");
	}
	
	tokens[i] = file;
	tokens[i+1] = NULL;

	return tokens;
}

// Executes the commands given
int runProcess(char **commands, int shellNum) {
	if (execvp(commands[0], commands) == -1) {
		char error[CMD_SIZE * 2];
		snprintf(error, sizeof(error), "[Shell %d] STATUS CODE=-1", shellNum);
		perror(error);
	}

	return 0;
}

int printExit() {
	printf("Done waiting on children: %d %d %d.", pids[0], pids[0]+1, pids[0]+2);
	
	return 0;
}

int main (int argc, char * argv[]) {

	// Read in commands and file
	printf("mash-1>");
	char *c1 = readLine();

	printf("mash-2>");
	char *c2 = readLine();

	printf("mash-3>");
	char *c3 = readLine();

	printf("file>");
	char *f = readLine();
	f = strtok(f, "\n");
	
	// Tokenizes commands
	char **t1 = getTokens(c1, f);
	char **t2 = getTokens(c2, f);
	char **t3 = getTokens(c3, f);

	// Forks and runs commands
	int i = 0;
	
	int fork1 = fork();
	if (fork1 < 0) { // Fork Fail
		fprintf(stderr, "Fork failed\n");
		exit(1);
	} else if (fork1 == 0) { // Child Process
		int fork2 = fork();
		if (fork2 < 0) { // Fork Failed
			fprintf(stderr, "Fork failed\n");
			exit(1);
		} else if (fork2 == 0) { // Child Process
			runProcess(t3, 3);
			pids[i] = getpid();
			i++;
		} else { // Parent
			runProcess(t2, 2);
			pids[i] = getpid();
			i++;
			int wc = wait(NULL);
		}		
	} else { // Parent
		runProcess(t1, 1);
		pids[i] = getpid();
		i++;
		int wc = wait(NULL);
		printExit();
	}

	return 0;
}
