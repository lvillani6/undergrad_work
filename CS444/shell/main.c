#include "tok.h" // Grabs characters from stdin and creates tokens
#include "io.h" // Importing syscalls & file handling including printing & scanning
#include "shtypes.h" // Importing the command struct
#include "parser.h" // Consumes tokens and creates command structs
#include "exec.h" // Functions that handle built-ins and run programs (execvp)
#include "syscalls.h" // Import more syscalls and define fork1
#include "jobs.h"

#include <pwd.h>

char *history[20];	// The running command history
extern struct Node* jobs;
int counter = 1;

/*argc : number of argumnets, argv : the arguments itself */
int main(int argc, char **argv)
{
	char *input; // User input
	int input_len; // Length of user input
	struct cmd *c; // Pointer to a command struct
	int child; // Child process pid

	// Already written: Prints out a banner telling the user about the shell.
	printf("simple sh, " __DATE__ " " __TIME__ "\nPress Control+C or Control+D to exit.\n");

	// Already written: Initialize the tokenizer.
	advance();

	size_t line_len = 0;	//Will be used for getting lines.

	while(1)
	{
		// TODO for students: You need to create the prompt, which consists of the
		// username, hostname and current working directory
		char *login;
		char hostname[255];
		char cwd[255];

		login = getpwuid(getuid()) -> pw_name;	//Apparently better to use than getlogin
		gethostname(hostname, 254);
		getcwd(cwd, 254);
		printf("%s@%s:%s$ ", login, hostname, cwd); // Implementation

		// TODO for students: Get input from the user.
		// Hint: You will want to use the "input" and "input_len" variables for this,
		// and then store it somewhere so that it can be recalled later for history.
		input = NULL;
		input_len = getline(&input, &line_len, stdin);

		for(int i = 19; i >= 0; i--)
			history[i] = history[i-1];
		// Add input to history
		history[0] = strdup(input);

		// Already written: Tokenizing and parsing the input into the command structure.
		push_str(input, input_len);
		c = parse();
		print_cmd(c);

		// TODO: You need to complete the implementation of this function inside exec.c
		// built-ins need to be handled by the shell process
		if (handle_builtin(c))
			goto done;


		// Fork : Copy yourself to a child and a parent
		// Already written: fork1() is essentialy fork() but also quits the program
		// if fork() fails
		if((child = fork1()))
		{
			// Already written: This block runs in the parent, with variable child equal
			// to the PID of the child process in the else block below.
			int code;
			if(c->tp == BACK)
				push(&jobs, counter++, child);
			// Already written: Wait for specifically the child with the given PID
			// to die. This *should* be our only child, but it doesn't hurt to be safe. */
			while(wait(&code) != child);

			// Already written: Prints exit code of last command.
			printf("command exited with code %d\n", WEXITSTATUS(code));
		}
		else
		{
			// Already written: This block runs in the child. runs exec_cmd and
			// exits with the return value of exec_cmd, which is supposed to return 0.
			// Note: You will need to fill in the implementation of exec_cmd inside exec.c.
			exit(exec_cmd(c));
		}


done:
		// Already written: Release any memory associated with the command.
		free_cmd(c);
		free(input);

		//TODO for students: This is a good place to free memory you allocated in this loop and you don't need anymore
		//use gdb to see leaks

		// return 0; dont return to keep loop open
	}
	// free(history);
	return 0;		//return here is fine
}
